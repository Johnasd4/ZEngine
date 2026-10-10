/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#define SOCKET_DLLFILE
#include "drive/d_pch.h"

#include "z_io_context.h"

namespace zengine {
namespace socket {

ZIOContext::ZIOContext() noexcept
    : SuperType_()
    , io_context_()
    , running_thread_count_(0)
{}

NODISCARD TOptional<TArray<ZTCPEndpoint>> ZIOContext::ResolveTCP(
    ZStringView _host_str,
    ZStringView _port_str
) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    // resolve endpoints
    asio::ip::tcp::resolver resolver(io_context_);
    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(
        asio::string_view(_host_str.GetDataPtr(), _host_str.GetSize()),
        asio::string_view(_port_str.GetDataPtr(), _port_str.GetSize()),
        error_code
    );

    if (error_code) {
        ret_val = error_code::kZIOContextErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: {} address: {} port: {}",
            error_code.message(),
            _host_str,
            _port_str
        );
        return ret_val;
    }

    TArray<ZTCPEndpoint> endpoint_array;

    endpoint_array.Resize(endpoints.size());
    SizeType array_index = 0ULL;
    for (
        auto endpoint_iterator = endpoints.begin(); 
        endpoint_iterator != endpoints.end(); 
        ++endpoint_iterator, ++array_index
    ) {
        endpoint_array[array_index].endpoint_ = endpoint_iterator->endpoint();
    }
    return endpoint_array;
}

NODISCARD TOptional<TArray<ZUDPEndpoint>> ZIOContext::ResolveUDP(
    ZStringView _host_str,
    ZStringView _port_str
) noexcept {
    ReturnType ret_val = kOK;
    std::error_code error_code;

    // resolve endpoints
    asio::ip::udp::resolver resolver(io_context_);
    asio::ip::udp::resolver::results_type endpoints = resolver.resolve(
        asio::string_view(_host_str.GetDataPtr(), _host_str.GetSize()),
        asio::string_view(_port_str.GetDataPtr(), _port_str.GetSize()),
        error_code
    );

    if (error_code) {
        ret_val = error_code::kZIOContextErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: {} address: {} port: {}",
            error_code.message(),
            _host_str,
            _port_str
        );
        return ret_val;
    }

    TArray<ZUDPEndpoint> endpoint_array;

    endpoint_array.Resize(endpoints.size());
    SizeType array_index = 0ULL;
    for (
        auto endpoint_iterator = endpoints.begin();
        endpoint_iterator != endpoints.end();
        ++endpoint_iterator, ++array_index
        ) {
        endpoint_array[array_index].endpoint_ = endpoint_iterator->endpoint();
    }
    return endpoint_array;
}

Void ZIOContext::AsyncResolveTCP(
    ZStringView _host_str,
    ZStringView _port_str,
    TFunction<Void(TOptional<TArray<ZTCPEndpoint>>&&)> _handle_func
) noexcept {
    auto resolver = MakeShared<asio::ip::tcp::resolver>(io_context_);

    ZString host_str(_host_str.GetDataPtr(), _host_str.GetSize());
    ZString port_str(_port_str.GetDataPtr(), _port_str.GetSize());

    resolver->async_resolve(
        asio::string_view(_host_str.GetDataPtr(), _host_str.GetSize()),
        asio::string_view(_port_str.GetDataPtr(), _port_str.GetSize()),
        [resolver, handle_func = std::move(_handle_func), host_str, port_str](
            const std::error_code& _error_code,
            asio::ip::tcp::resolver::results_type _endpoints
        ) {
            if (_error_code) {
                ReturnType ret_val = error_code::kZIOContextErrorCode_SystemOrLibraryError;
                Z_LOG_ERROR(
                    ret_val, _error_code.value(),
                    "System error! error info: {} address: {} port: {}",
                    _error_code.message(),
                    host_str,
                    port_str
                );

                if (handle_func) {
                    handle_func(ret_val);
                }
                return;
            }

            TOptional<TArray<ZTCPEndpoint>> endpoint_array;
            endpoint_array->Resize(_endpoints.size());
            SizeType array_index = 0ULL;
            for (
                auto endpoint_iterator = _endpoints.begin();
                endpoint_iterator != _endpoints.end();
                ++endpoint_iterator, ++array_index
            ) {
                (*endpoint_array)[array_index].endpoint_ = endpoint_iterator->endpoint();
            }

            if (handle_func) {
                handle_func(std::move(endpoint_array));
            }
        }
    );
}

Void ZIOContext::AsyncResolveUDP(
    ZStringView _host_str,
    ZStringView _port_str,
    TFunction<Void(TOptional<TArray<ZUDPEndpoint>>&&)> _handle_func
) noexcept {
    auto resolver = MakeShared<asio::ip::udp::resolver>(io_context_);

    ZString host_str(_host_str.GetDataPtr(), _host_str.GetSize());
    ZString port_str(_port_str.GetDataPtr(), _port_str.GetSize());

    resolver->async_resolve(
        asio::string_view(_host_str.GetDataPtr(), _host_str.GetSize()),
        asio::string_view(_port_str.GetDataPtr(), _port_str.GetSize()),
        [resolver, handle_func = std::move(_handle_func), host_str, port_str](
            const std::error_code& _error_code,
            asio::ip::udp::resolver::results_type _endpoints
            ) {
                if (_error_code) {
                    ReturnType ret_val = error_code::kZIOContextErrorCode_SystemOrLibraryError;
                    Z_LOG_ERROR(
                        ret_val, _error_code.value(),
                        "System error! error info: {} address: {} port: {}",
                        _error_code.message(),
                        host_str,
                        port_str
                    );

                    if (handle_func) {
                        handle_func(ret_val);
                    }
                    return;
                }

                TOptional<TArray<ZUDPEndpoint>> endpoint_array;
                endpoint_array->Resize(_endpoints.size());
                SizeType array_index = 0ULL;
                for (
                    auto endpoint_iterator = _endpoints.begin();
                    endpoint_iterator != _endpoints.end();
                    ++endpoint_iterator, ++array_index
                    ) {
                    (*endpoint_array)[array_index].endpoint_ = endpoint_iterator->endpoint();
                }

                if (handle_func) {
                    handle_func(std::move(endpoint_array));
                }
        }
    );
}

Void ZIOContext::Stop() noexcept {
    io_context_.stop();
}

NODISCARD ReturnType ZIOContext::Restart() noexcept {
    ReturnType ret_val = kOK;

    if (running_thread_count_.Load() != 0) {
        ret_val = error_code::kZIOContextErrorCode_OperationNotAllowed;
        Z_LOG_ERROR(
            ret_val, 0,
            "Attempting to restart io_context while threads are still running!"
        );
        return ret_val;
    }

    if (io_context_.stopped()) {
        io_context_.restart();
    }

    return ret_val;
}

Void ZIOContext::Run() noexcept {
    ++running_thread_count_;
    io_context_.run();
    if (--running_thread_count_ == 0) {
        running_thread_count_.NotifyAll();
    }
}

Void ZIOContext::Join() noexcept {
    Int32 current_count = running_thread_count_.Load();
    while (current_count > 0) {
        running_thread_count_.Wait(current_count);
        current_count = running_thread_count_.Load();
    }

}

} // socket
} // zengine