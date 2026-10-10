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
    Contact: 1152325286@qq.com85
*/
#define SOCKET_DLLFILE
#include "drive/d_pch.h"

#include "internal/z_socket_delete_manager.h"

#include "z_tcp_socket.h"
#include "z_udp_socket.h"

namespace zengine {
namespace socket {
namespace internal {

NODISCARD ZTCPSocketDeleteManager& ZTCPSocketDeleteManager::GetInstance() noexcept {
    static ZTCPSocketDeleteManager manager;
    return manager;
}

NODISCARD Void ZTCPSocketDeleteManager::DeleteTickFunc(Float32 _delta_sec) noexcept {
    TStackSafe<SocketData_>& new_socket_data_stack = GetInstance().new_socket_ptr_stack_;
    TList<SocketData_>& socket_data_list = GetInstance().socket_ptr_list_;

    for (auto socket_iter = socket_data_list.Begin(); socket_iter != socket_data_list.End(); ) {
        ZTCPSocket* socket_ptr = socket_iter->socket_ptr_;
        if (socket_ptr->async_operation_count_ == 0) {
            socket_ptr->~ZTCPSocket();
            if (socket_iter->destruct_func_) {
                socket_iter->destruct_func_();
            }
            else {
                ZTCPSocket::SuperType_::operator delete(socket_ptr);
            }
            socket_iter = GetInstance().socket_ptr_list_.Erase(socket_iter);
        }
        else {
            ++socket_iter;
        }
    }

    SocketData_ socket_data;
    while (new_socket_data_stack.TryPopBack(&socket_data)) {
        if (socket_data.socket_ptr_->async_operation_count_ == 0) {
            socket_data.socket_ptr_->~ZTCPSocket();
            if (socket_data.destruct_func_) {
                socket_data.destruct_func_();
            }
            else {
                ZTCPSocket::SuperType_::operator delete(socket_data.socket_ptr_);
            }
        }
        else {
            socket_data_list.PushBack(std::move(socket_data));
        }
    }
} 

ZTCPSocketDeleteManager::ZTCPSocketDeleteManager() noexcept
    : socket_ptr_list_()
{
    ZTickManager::GetInstance().AddTickFunction(DeleteTickFunc);
}
    
ZTCPSocketDeleteManager::~ZTCPSocketDeleteManager() noexcept {
    ZTickManager::GetInstance().RemoveTickFunctionImmediately(DeleteTickFunc);
}

Void ZTCPSocketDeleteManager::DeleteSocket(
    ZTCPSocket* _socket_ptr, 
    TFunction<Void()>&& _destruct_func
) noexcept {
    new_socket_ptr_stack_.EmplaceBack(_socket_ptr, std::move(_destruct_func));
}

NODISCARD ZUDPSocketDeleteManager& ZUDPSocketDeleteManager::GetInstance() noexcept {
    static ZUDPSocketDeleteManager manager;
    return manager;
}

NODISCARD Void ZUDPSocketDeleteManager::DeleteTickFunc(Float32 _delta_sec) noexcept {
    TStackSafe<SocketData_>& new_socket_data_stack = GetInstance().new_socket_ptr_stack_;
    TList<SocketData_>& socket_data_list = GetInstance().socket_ptr_list_;

    for (auto socket_iter = socket_data_list.Begin(); socket_iter != socket_data_list.End(); ) {
        ZUDPSocket* socket_ptr = socket_iter->socket_ptr_;
        if (socket_ptr->async_operation_count_ == 0) {
            socket_ptr->~ZUDPSocket();
            if (socket_iter->destruct_func_) {
                socket_iter->destruct_func_();
            }
            else {
                ZUDPSocket::SuperType_::operator delete(socket_ptr);
            }
            socket_iter = GetInstance().socket_ptr_list_.Erase(socket_iter);
        }
        else {
            ++socket_iter;
        }
    }

    SocketData_ socket_data;
    while (new_socket_data_stack.TryPopBack(&socket_data)) {
        if (socket_data.socket_ptr_->async_operation_count_ == 0) {
            socket_data.socket_ptr_->~ZUDPSocket();
            if (socket_data.destruct_func_) {
                socket_data.destruct_func_();
            }
            else {
                ZUDPSocket::SuperType_::operator delete(socket_data.socket_ptr_);
            }
        }
        else {
            socket_data_list.PushBack(std::move(socket_data));
        }
    }
}

ZUDPSocketDeleteManager::ZUDPSocketDeleteManager() noexcept
    : socket_ptr_list_()
{
    ZTickManager::GetInstance().AddTickFunction(DeleteTickFunc);
}

ZUDPSocketDeleteManager::~ZUDPSocketDeleteManager() noexcept {
    ZTickManager::GetInstance().RemoveTickFunctionImmediately(DeleteTickFunc);
}

Void ZUDPSocketDeleteManager::DeleteSocket(
    ZUDPSocket* _socket_ptr,
    TFunction<Void()>&& _destruct_func
) noexcept {
    new_socket_ptr_stack_.EmplaceBack(_socket_ptr, std::move(_destruct_func));
}

}// internal
}// socket
}// zengine