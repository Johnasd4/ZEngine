/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#define SOCKET_DLLFILE
#include "drive/d_pch.h"

#include "z_http_response_resolver.h"

#include "data/z_http_11_response_resolver_data.h"

namespace zengine {
namespace socket {

ZHTTP11ResponseResolver::ZHTTP11ResponseResolver() noexcept
    : SuperType_()
    , data_ptr_(MakeUnique<internal::ZHTTP11ResponseResolverData>())
{}

ZHTTP11ResponseResolver::~ZHTTP11ResponseResolver() noexcept {}

NODISCARD ReturnType ZHTTP11ResponseResolver::Resolve(ZConstBuffer _buffer) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    //reset the response resolver
    if (data_ptr_->response_parser_.is_done()) {
        data_ptr_ = MakeUnique<internal::ZHTTP11ResponseResolverData>();
    }
    SizeType bytes_used = data_ptr_->response_parser_.put(
        boost::asio::buffer(_buffer.DataPtr<const Void>(), _buffer.Size()), error_code
    );
    if (error_code) {
        ret_val = error_code::kHTTPErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr()
        );
        return ret_val;
    }

    data_ptr_->response_ = data_ptr_->response_parser_.release();

    return kOK;
}

NODISCARD UInt32 ZHTTP11ResponseResolver::GetVersion() noexcept {
    return data_ptr_->response_.version();
}
NODISCARD UInt32 ZHTTP11ResponseResolver::GetResult() noexcept {
    return data_ptr_->response_.result_int();
}
NODISCARD ZStringView ZHTTP11ResponseResolver::GetReason() noexcept {
    return ZStringView(data_ptr_->response_.reason().data(), data_ptr_->response_.reason().size());
}

NODISCARD ReturnType ZHTTP11ResponseResolver::GetInt32(Int32* _value_ptr, ZStringView _name) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    Z_CHECK(
        _value_ptr == nullptr,
        error_code::kHTTPErrorCode_NullptrParam,
        "_io_context_ptr is nullptr!"
    );
    try {
        auto string_view = data_ptr_->response_.at(
            boost::core::string_view(_name.DataPtr(), _name.Size())
        );
        ZString string(string_view.data(), string_view.size());
        link_code = string.ToNumber(_value_ptr);
        if (link_code != kOK) {
            ret_val = error_code::kHTTPErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZString::ToInt32() link error!"
            );
            return ret_val;
        }
    }
    catch (const std::exception& error_code) {
        ret_val = error_code::kHTTPErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! error info: %ls",
            string::StringToWString(error_code.what()).DataPtr()
        );
    }
    return ret_val;
}
NODISCARD ReturnType ZHTTP11ResponseResolver::GetInt64(Int64* _value_ptr, ZStringView _name) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    Z_CHECK(
        _value_ptr == nullptr,
        error_code::kHTTPErrorCode_NullptrParam,
        "_io_context_ptr is nullptr!"
    );
    try {
        auto string_view = data_ptr_->response_.at(
            boost::core::string_view(_name.DataPtr(), _name.Size())
        );
        ZString string(string_view.data(), string_view.size());
        link_code = string.ToNumber(_value_ptr);
        if (link_code != kOK) {
            ret_val = error_code::kHTTPErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZString::ToInt64() link error!"
            );
            return ret_val;
        }
    }
    catch (const std::exception& error_code) {
        ret_val = error_code::kHTTPErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! error info: %ls",
            string::StringToWString(error_code.what()).DataPtr()
        );
    }
    return ret_val;
}
NODISCARD ReturnType ZHTTP11ResponseResolver::GetUInt32(UInt32* _value_ptr, ZStringView _name) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    Z_CHECK(
        _value_ptr == nullptr,
        error_code::kHTTPErrorCode_NullptrParam,
        "_io_context_ptr is nullptr!"
    );
    try {
        auto string_view = data_ptr_->response_.at(
            boost::core::string_view(_name.DataPtr(), _name.Size())
        );
        ZString string(string_view.data(), string_view.size());
        link_code = string.ToNumber(_value_ptr);
        if (link_code != kOK) {
            ret_val = error_code::kHTTPErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZString::ToUInt32() link error!"
            );
            return ret_val;
        }
    }
    catch (const std::exception& error_code) {
        ret_val = error_code::kHTTPErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! error info: %ls",
            string::StringToWString(error_code.what()).DataPtr()
        );
    }
    return ret_val;
}
NODISCARD ReturnType ZHTTP11ResponseResolver::GetUInt64(UInt64* _value_ptr, ZStringView _name) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    Z_CHECK(
        _value_ptr == nullptr,
        error_code::kHTTPErrorCode_NullptrParam,
        "_io_context_ptr is nullptr!"
    );
    try {
        auto string_view = data_ptr_->response_.at(
            boost::core::string_view(_name.DataPtr(), _name.Size())
        );
        ZString string(string_view.data(), string_view.size());
        link_code = string.ToNumber(_value_ptr);
        if (link_code != kOK) {
            ret_val = error_code::kHTTPErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZString::ToUInt64() link error!"
            );
            return ret_val;
        }
    }
    catch (const std::exception& error_code) {
        ret_val = error_code::kHTTPErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! error info: %ls",
            string::StringToWString(error_code.what()).DataPtr()
        );
    }
    return ret_val;
}
NODISCARD ReturnType ZHTTP11ResponseResolver::GetFloat32(Float32* _value_ptr, ZStringView _name) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    Z_CHECK(
        _value_ptr == nullptr,
        error_code::kHTTPErrorCode_NullptrParam,
        "_io_context_ptr is nullptr!"
    );
    try {
        auto string_view = data_ptr_->response_.at(
            boost::core::string_view(_name.DataPtr(), _name.Size())
        );
        ZString string(string_view.data(), string_view.size());
        link_code = string.ToNumber(_value_ptr);
        if (link_code != kOK) {
            ret_val = error_code::kHTTPErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZString::ToFloat32() link error!"
            );
            return ret_val;
        }
    }
    catch (const std::exception& error_code) {
        ret_val = error_code::kHTTPErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! error info: %ls",
            string::StringToWString(error_code.what()).DataPtr()
        );
    }
    return ret_val;
}
NODISCARD ReturnType ZHTTP11ResponseResolver::GetFloat64(Float64* _value_ptr, ZStringView _name) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    Z_CHECK(
        _value_ptr == nullptr,
        error_code::kHTTPErrorCode_NullptrParam,
        "_io_context_ptr is nullptr!"
    );
    try {
        auto string_view = data_ptr_->response_.at(
            boost::core::string_view(_name.DataPtr(), _name.Size())
        );
        ZString string(string_view.data(), string_view.size());
        link_code = string.ToNumber(_value_ptr);
        if (link_code != kOK) {
            ret_val = error_code::kHTTPErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZString::ToFloat64() link error!"
            );
            return ret_val;
        }
    }
    catch (const std::exception& error_code) {
        ret_val = error_code::kHTTPErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! error info: %ls",
            string::StringToWString(error_code.what()).DataPtr()
        );
    }
    return ret_val;
}
NODISCARD ReturnType ZHTTP11ResponseResolver::GetStringView(ZStringView* _value_ptr, ZStringView _name) noexcept{
    ReturnType ret_val = kOK;
    Z_CHECK(
        _value_ptr == nullptr,
        error_code::kHTTPErrorCode_NullptrParam,
        "_io_context_ptr is nullptr!"
    );
    try {
        auto string_view_raw = data_ptr_->response_.at(
            boost::core::string_view(_name.DataPtr(), _name.Size())
        );
        _value_ptr->Assign(string_view_raw.data(), string_view_raw.size());
    }
    catch (const std::exception& error_code) {
        
        Z_LOG_ERROR(
            error_code::kHTTPErrorCode_SystemError, 0,
            "System error! error info: %ls",
            string::StringToWString(error_code.what()).DataPtr()
        );
    }
    return ret_val;
}
NODISCARD ReturnType ZHTTP11ResponseResolver::GetString(ZString* _value_ptr, ZStringView _name) noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        _value_ptr == nullptr,
        error_code::kHTTPErrorCode_NullptrParam,
        "_io_context_ptr is nullptr!"
    );
    try {
        auto string_view_raw = data_ptr_->response_.at(
            boost::core::string_view(_name.DataPtr(), _name.Size())
        );
        _value_ptr->Assign(string_view_raw.data(), string_view_raw.size());
    }
    catch (const std::exception& error_code) {

        Z_LOG_ERROR(
            error_code::kHTTPErrorCode_SystemError, 0,
            "System error! error info: %ls",
            string::StringToWString(error_code.what()).DataPtr()
        );
    }
    return ret_val;
}

}//socket
}//zengine