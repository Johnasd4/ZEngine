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
#pragma once

#include "drive/d_pch.h"

namespace zengine {
namespace socket {

using SocketAllocatorType = TAllocator<Void>;

template <typename _HandleFunction>
class ZSocketHandlerAllocator {
public:
    ZSocketHandlerAllocator(const _HandleFunction& _handle_func) : handle_func_(_handle_func) {}

    //operate func
    template <typename... ArgTypes>
    FORCEINLINE Void operator()(ArgTypes&&... args) noexcept {
        handle_func_(std::forward<ArgTypes>(args)...);
    }

    //socket allocator interface
    friend Void* asio_handler_allocate(
        SizeType _size, ZSocketHandlerAllocator<_HandleFunction>* this_handler
    ) {
        return SocketAllocatorType::Instance().allocate(_size);
    }

    friend Void asio_handler_deallocate(
        Void* pointer, SizeType _size, ZSocketHandlerAllocator<_HandleFunction>* this_handler
    ) {
        SocketAllocatorType::Instance().deallocate(pointer);
    }

private:
    _HandleFunction handle_func_;
};

template <typename _HandleFunction>
FORCEINLINE ZSocketHandlerAllocator<_HandleFunction> MakeSocketHandlerAllocator(const _HandleFunction& _handle_func) {
    return ZSocketHandlerAllocator<_HandleFunction>(_handle_func);
}

} // socket
} // zengine