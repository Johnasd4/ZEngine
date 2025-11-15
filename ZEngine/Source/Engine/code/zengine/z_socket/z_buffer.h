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

#include "drive.h"

#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_object.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZBufferStreamData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    Buffer type, initialize with a base buffer size.
*/
class ZBuffer : public ZObject {
public:
    FORCEINLINE ZBuffer() noexcept
        : SuperType_()
        , data_ptr_(nullptr)
        , size_(0ULL)
    {}

    FORCEINLINE ZBuffer(const ZBuffer& _buffer) noexcept
        : SuperType_()
        , data_ptr_(_buffer.data_ptr_)
        , size_(_buffer.size_)
    {}

    template<typename PointerType>
    FORCEINLINE ZBuffer(PointerType* _buffer_ptr, SizeType _size) noexcept
        : SuperType_()
        , data_ptr_(_buffer_ptr)
        , size_(_size)
    {}

    FORCEINLINE ZBuffer& operator=(const ZBuffer& _buffer) noexcept {
        SuperType_::operator=(_buffer);
        data_ptr_ = _buffer.data_ptr_;
        size_ = _buffer.size_;
        return *this;
    }

    template<typename PointerType>
    NODISCARD FORCEINLINE Void SetBuffer(PointerType* _data_ptr, SizeType _size) noexcept {
        data_ptr_ = _data_ptr;
        size_ = _size;
    }

    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType BufferPtr() noexcept {
        return reinterpret_cast<_ObjectType>(data_ptr_);
    }
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType BufferPtr() const noexcept {
        return reinterpret_cast<_ObjectType>(data_ptr_);
    }

    NODISCARD FORCEINLINE SizeType Size() const noexcept { return size_; }

protected:
    using SuperType_ = ZObject;
    friend class ZBufferStream;

private:
    Void* data_ptr_;
    SizeType size_;
};

/*
    Const buffer type, initialize with a base buffer size.
*/
class ZConstBuffer : public ZObject {
public:
    FORCEINLINE ZConstBuffer() noexcept
        : SuperType_()
        , data_ptr_(nullptr)
        , size_(0ULL)
    {}

    FORCEINLINE ZConstBuffer(const ZConstBuffer& _buffer) noexcept
        : SuperType_()
        , data_ptr_(_buffer.data_ptr_)
        , size_(_buffer.size_)
    {}

    FORCEINLINE ZConstBuffer(const ZBuffer& _buffer) noexcept
        : SuperType_()
        , data_ptr_(_buffer.BufferPtr<Void*>())
        , size_(_buffer.Size())
    {}

    template<typename PointerType>
    FORCEINLINE ZConstBuffer(const PointerType* _buffer_ptr, SizeType _size) noexcept
        : SuperType_()
        , data_ptr_(_buffer_ptr)
        , size_(_size)
    {}

    FORCEINLINE ZConstBuffer& operator=(const ZConstBuffer& _buffer) noexcept {
        SuperType_::operator=(_buffer);
        data_ptr_ = _buffer.data_ptr_;
        size_ = _buffer.size_;
        return *this;
    }

    FORCEINLINE ZConstBuffer& operator=(const ZBuffer& _buffer) noexcept {
        SuperType_::operator=(_buffer);
        data_ptr_ = _buffer.BufferPtr<Void*>();
        size_ = _buffer.Size();
        return *this;
    }

    template<typename PointerType>
    NODISCARD FORCEINLINE Void SetBuffer(const PointerType* _data_ptr, SizeType _size) noexcept {
        data_ptr_ = _data_ptr;
        size_ = _size;
    }

    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType BufferPtr() const noexcept {
        return reinterpret_cast<_ObjectType>(data_ptr_);
    }

    NODISCARD FORCEINLINE SizeType Size() const noexcept { return size_; }

protected:
    using SuperType_ = ZObject;
    friend class ZBufferStream;

private:
    const Void* data_ptr_;
    SizeType size_;
};

}//socket
}//zengine