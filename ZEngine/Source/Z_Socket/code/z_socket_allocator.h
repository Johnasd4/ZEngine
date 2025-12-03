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

}//socket
}//zengine