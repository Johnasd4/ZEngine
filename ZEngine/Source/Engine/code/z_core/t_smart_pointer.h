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

#include "internal/z_drive.h"

#include <memory>

#include "t_allocator.h"
#include "z_object.h"

namespace zengine {

template<typename _ObjectType>
class TUniquePointer;
template<typename _ObjectType>
class TSharedPointer;
template<typename _ObjectType>
class TWeakPointer;

/*
    Unique smart pointer.
*/
template<typename _ObjectType>
class TUniquePointer : public ZObject {
public:
    using STDUniquePointer_ = std::unique_ptr<_ObjectType>;

    FORCEINLINE TUniquePointer() noexcept : SuperType_(), ptr_() {}
    FORCEINLINE TUniquePointer(NullptrType _ptr) noexcept : SuperType_(), ptr_(_ptr) {}
    FORCEINLINE TUniquePointer(TUniquePointer&& _ptr) noexcept 
        : SuperType_(std::forward<TUniquePointer>(_ptr)), ptr_(std::move(_ptr.ptr_)) {}
    template<typename _OtherObjectType>
    FORCEINLINE TUniquePointer(TUniquePointer<_OtherObjectType>&& _ptr) noexcept 
        : SuperType_(std::forward<TUniquePointer<_OtherObjectType>>(_ptr)), ptr_(std::move(_ptr.ptr_)) {}
    FORCEINLINE TUniquePointer(_ObjectType* _ptr) noexcept : SuperType_(), ptr_(_ptr) {}

    FORCEINLINE ~TUniquePointer() noexcept {}

    FORCEINLINE TUniquePointer& operator=(TUniquePointer&& _ptr) noexcept { 
        SuperType_::operator=(std::forward<TUniquePointer>(_ptr));
        ptr_ = std::move(_ptr.ptr_);
        return *this;
    }
    template<typename _OtherObjectType>
    FORCEINLINE TUniquePointer& operator=(TUniquePointer<_OtherObjectType>&& _ptr) noexcept {
        SuperType_::operator=(std::forward<TUniquePointer<_OtherObjectType>>(_ptr));
        ptr_ = std::move(_ptr.ptr_);
        return *this;
    }
    template<typename _PtrType>
    FORCEINLINE TUniquePointer& operator==(_PtrType _ptr) noexcept {
        return ptr_ == _ptr;
    }

    NODISCARD FORCEINLINE decltype(auto) operator*() const noexcept { return *ptr_; }
    NODISCARD FORCEINLINE _ObjectType* operator->() const noexcept { return ptr_; }
    NODISCARD FORCEINLINE operator Bool() const noexcept { return ptr_; }

    NODISCARD FORCEINLINE _ObjectType* Get() const noexcept { return ptr_.get(); }
    NODISCARD FORCEINLINE _ObjectType* Release() noexcept { return ptr_.release(); }

    FORCEINLINE Void Reset() noexcept { ptr_.reset(nullptr); }
    FORCEINLINE Void Reset(_ObjectType* _ptr) noexcept { ptr_.reset(_ptr); }
    FORCEINLINE Void Swap(TUniquePointer& _ptr) noexcept { ptr_.swap(_ptr.ptr_); }

protected:
    using SuperType_ = ZObject;

private:
    TUniquePointer(const TUniquePointer&) = delete;
    TUniquePointer& operator=(const TUniquePointer&) = delete;

    STDUniquePointer_ ptr_;
};

/*
    Shared smart pointer.
*/
template<typename _ObjectType>
class TSharedPointer : public ZObject {
public:
    using STDSharedPointer_ = std::shared_ptr<_ObjectType>;

    FORCEINLINE TSharedPointer() noexcept : SuperType_(), ptr_() {}
    FORCEINLINE TSharedPointer(NullptrType _ptr) noexcept : SuperType_(), ptr_(_ptr) {}
    FORCEINLINE TSharedPointer(const TSharedPointer& _ptr) noexcept : SuperType_(_ptr), ptr_(_ptr.ptr_) {}
    FORCEINLINE TSharedPointer(TSharedPointer&& _ptr) noexcept 
        : SuperType_(std::forward<TSharedPointer>(_ptr)), ptr_(std::move(_ptr.ptr_)) {}
    template<typename _OtherObjectType>
    FORCEINLINE TSharedPointer(const TSharedPointer<_OtherObjectType>& _ptr) noexcept
        : SuperType_(_ptr), ptr_(_ptr.ptr_) {}
    template<typename _OtherObjectType>
    FORCEINLINE TSharedPointer(const TWeakPointer<_OtherObjectType>& _ptr) noexcept
        : SuperType_(_ptr), ptr_(_ptr.ptr_) {}
    template<typename _OtherObjectType>
    FORCEINLINE TSharedPointer(TSharedPointer<_OtherObjectType>&& _ptr) noexcept 
        : SuperType_(std::forward<TSharedPointer<_OtherObjectType>>(_ptr)), ptr_(std::move(_ptr.ptr_)) {}
    template<typename _OtherObjectType>
    FORCEINLINE TSharedPointer(TUniquePointer<_OtherObjectType>&& _ptr) noexcept
        : SuperType_(std::forward<TSharedPointer<_OtherObjectType>>(_ptr))
        , ptr_(std::allocate_shared<_ObjectType>(TSmartPointerAllocator<_ObjectType>(), std::move(*_ptr.Release()))) {}

    FORCEINLINE ~TSharedPointer() noexcept {}

    FORCEINLINE TSharedPointer& operator=(const TSharedPointer& _ptr) noexcept {
        SuperType_::operator=(_ptr);
        ptr_ = _ptr.ptr_;
        return *this;
    }
    FORCEINLINE TSharedPointer& operator=(TSharedPointer&& _ptr) noexcept { 
        SuperType_::operator=(std::forward<TSharedPointer>(_ptr));
        ptr_ = std::move(_ptr.ptr_);
        return *this;
    }
    template<typename _OtherObjectType>
    FORCEINLINE TSharedPointer& operator=(const TSharedPointer<_OtherObjectType>& _ptr) noexcept {
        SuperType_::operator=(_ptr);
        ptr_ = _ptr.ptr_;
        return *this;
    }
    template<typename _OtherObjectType>
    FORCEINLINE TSharedPointer& operator=(TSharedPointer<_OtherObjectType>&& _ptr) noexcept {
        SuperType_::operator=(std::forward<TSharedPointer<_OtherObjectType>>(_ptr));
        ptr_ = std::move(_ptr.ptr_);
        return *this;
    }

    NODISCARD FORCEINLINE decltype(auto) operator*() const noexcept { return *ptr_; }
    NODISCARD FORCEINLINE _ObjectType* operator->() const noexcept { return ptr_; }
    NODISCARD FORCEINLINE operator Bool() const noexcept { return ptr_; }

    NODISCARD FORCEINLINE _ObjectType* Get() const noexcept { return ptr_.get(); }
    NODISCARD FORCEINLINE _ObjectType* Release() noexcept { return ptr_.release(); }

    NODISCARD FORCEINLINE Int32 UseCount() const noexcept { return ptr_.use_count(); }
    NODISCARD FORCEINLINE Bool Unique() const noexcept { return ptr_.use_count() == 1; }

    FORCEINLINE Void Reset() noexcept { ptr_.reset(); }
    template<typename _OtherObjectType>
    FORCEINLINE Void Reset(_OtherObjectType* _ptr) noexcept { ptr_.reset(_ptr); }
    FORCEINLINE Void Swap(TSharedPointer& _ptr) noexcept { ptr_.swap(_ptr.ptr_); }

protected:
    using SuperType_ = ZObject;

private:
    template<typename _ObjectType>
    friend class TWeakPointer;
    template<typename _ObjectType, typename... _ArgsType>
    friend NODISCARD FORCEINLINE TSharedPointer<_ObjectType> MakeShared(_ArgsType&&... _args) noexcept;
    template<typename _ObjectType, typename... _ArgsType>
    friend NODISCARD FORCEINLINE TSharedPointer<_ObjectType> MakeShared(_ArgsType&&... _args) noexcept;
    template<typename _ObjectType, typename _OtherObjectType>
    friend NODISCARD FORCEINLINE TSharedPointer<_ObjectType> StaticPointerCast(
        const TSharedPointer<_OtherObjectType>& _ptr
    ) noexcept;
    template<typename _ObjectType, typename _OtherObjectType>
    friend NODISCARD FORCEINLINE TSharedPointer<_ObjectType> StaticPointerCast(
        TSharedPointer<_OtherObjectType>&& _ptr
    ) noexcept;
    template<typename _ObjectType, typename _OtherObjectType>
    friend NODISCARD FORCEINLINE TSharedPointer<_ObjectType> ConstPointerCast(
        const TSharedPointer<_OtherObjectType>& _ptr
    ) noexcept;
    template<typename _ObjectType, typename _OtherObjectType>
    friend NODISCARD FORCEINLINE TSharedPointer<_ObjectType> ConstPointerCast(
        TSharedPointer<_OtherObjectType>&& _ptr
    ) noexcept;
    template<typename _ObjectType, typename _OtherObjectType>
    friend NODISCARD FORCEINLINE TSharedPointer<_ObjectType> ReinterpretPointerCast(
        const TSharedPointer<_OtherObjectType>& _ptr
    ) noexcept;
    template<typename _ObjectType, typename _OtherObjectType>
    friend NODISCARD FORCEINLINE TSharedPointer<_ObjectType> ReinterpretPointerCast(
        TSharedPointer<_OtherObjectType>&& _ptr
    ) noexcept;
    template<typename _ObjectType, typename _OtherObjectType>
    friend NODISCARD FORCEINLINE TSharedPointer<_ObjectType> DynamicPointerCast(
        const TSharedPointer<_OtherObjectType>& _ptr
    ) noexcept;
    template<typename _ObjectType, typename _OtherObjectType>
    friend NODISCARD FORCEINLINE TSharedPointer<_ObjectType> DynamicPointerCast(
        TSharedPointer<_OtherObjectType>&& _ptr
    ) noexcept;

    STDSharedPointer_ ptr_;
};

/*
    Weak smart pointer.
*/
template<typename _ObjectType>
class TWeakPointer : public ZObject {
public:
    using STDWeakPointer_ = std::weak_ptr<_ObjectType>;

    FORCEINLINE TWeakPointer() noexcept : SuperType_(), ptr_() {}
    FORCEINLINE TWeakPointer(NullptrType _ptr) noexcept : SuperType_(), ptr_(_ptr) {}
    FORCEINLINE TWeakPointer(const TWeakPointer& _ptr) noexcept : SuperType_(_ptr), ptr_(_ptr.ptr_) {}
    FORCEINLINE TWeakPointer(TWeakPointer&& _ptr) noexcept 
        : SuperType_(std::forward<TWeakPointer>(_ptr)), ptr_(std::move(_ptr.ptr_)) {}
    template<typename _OtherObjectType>
    FORCEINLINE TWeakPointer(const TWeakPointer<_OtherObjectType>& _ptr) noexcept
        : SuperType_(_ptr), ptr_(_ptr.ptr_) {}
    template<typename _OtherObjectType>
    FORCEINLINE TWeakPointer(const TSharedPointer<_OtherObjectType>& _ptr) noexcept
        : SuperType_(_ptr), ptr_(_ptr.ptr_) {}
    template<typename _OtherObjectType>
    FORCEINLINE TWeakPointer(TWeakPointer<_OtherObjectType>&& _ptr) noexcept 
        : SuperType_(std::forward<TWeakPointer<_OtherObjectType>>(_ptr)), ptr_(std::move(_ptr.ptr_)) {}
    FORCEINLINE TWeakPointer(_ObjectType* _ptr) noexcept : SuperType_(), ptr_(_ptr) {}

    FORCEINLINE ~TWeakPointer() noexcept {}

    FORCEINLINE TWeakPointer& operator=(const TWeakPointer& _ptr) noexcept {
        SuperType_::operator=(_ptr);
        ptr_ = _ptr.ptr_;
        return *this;
    }
    FORCEINLINE TWeakPointer& operator=(TWeakPointer&& _ptr) noexcept { 
        SuperType_::operator=(std::forward<TWeakPointer>(_ptr));
        ptr_ = std::move(_ptr.ptr_);
        return *this;
    }
    template<typename _OtherObjectType>
    FORCEINLINE TWeakPointer& operator=(const TWeakPointer<_OtherObjectType>& _ptr) noexcept {
        SuperType_::operator=(_ptr);
        ptr_ = _ptr.ptr_;
        return *this;
    }
    template<typename _OtherObjectType>
    FORCEINLINE TWeakPointer& operator=(const TSharedPointer<_OtherObjectType>& _ptr) noexcept {
        SuperType_::operator=(_ptr);
        ptr_ = _ptr.ptr_;
        return *this;
    }
    template<typename _OtherObjectType>
    FORCEINLINE TWeakPointer& operator=(TWeakPointer<_OtherObjectType>&& _ptr) noexcept {
        SuperType_::operator=(std::forward<TWeakPointer<_OtherObjectType>>(_ptr));
        ptr_ = std::move(_ptr.ptr_);
        return *this;
    }

    NODISCARD FORCEINLINE TSharedPointer<_ObjectType> Lock() const noexcept { 
        TSharedPointer<_ObjectType> ptr;
        ptr.ptr_ = ptr_.lock();
        return ptr;
    }
    NODISCARD FORCEINLINE Int32 UseCount() const noexcept { return ptr_.use_count(); }
    NODISCARD FORCEINLINE Bool Expired() const noexcept { return ptr_.expired(); }

    FORCEINLINE Void Reset() noexcept { ptr_.reset(); }
    FORCEINLINE Void Swap(TWeakPointer& _ptr) noexcept { ptr_.swap(_ptr.ptr_); }

protected:
    using SuperType_ = ZObject;

private:
    STDWeakPointer_ ptr_;
};

template<typename _ObjectType>
NODISCARD FORCEINLINE Void Swap(
    TUniquePointer<_ObjectType>& _left, TUniquePointer<_ObjectType>& _right
) noexcept {
    _left.Swap(_right);
}

template<typename _ObjectType>
NODISCARD FORCEINLINE Void Swap(
    TSharedPointer<_ObjectType>& _left, TSharedPointer<_ObjectType>& _right
) noexcept {
    _left.Swap(_right);
}

template<typename _ObjectType>
NODISCARD FORCEINLINE Void Swap(
    TWeakPointer<_ObjectType>& _left, TWeakPointer<_ObjectType>& _right
) noexcept {
    _left.Swap(_right);
}

template<typename _ObjectType, typename... _ArgsType>
NODISCARD FORCEINLINE TUniquePointer<_ObjectType> MakeUnique(_ArgsType&&... _args) noexcept {
    return TUniquePointer(new _ObjectType(std::forward<_ArgsType>(_args)...));
}


template<typename _ObjectType, typename... _ArgsType>
NODISCARD FORCEINLINE TSharedPointer<_ObjectType> MakeShared(_ArgsType&&... _args) noexcept {
    TSharedPointer<_ObjectType> ptr;
    ptr.ptr_ = std::allocate_shared<_ObjectType>(
        TSmartPointerAllocator<_ObjectType>(), std::forward<_ArgsType>(_args)...);
    return ptr;
}

template<typename _ObjectType, typename _OtherObjectType>
NODISCARD FORCEINLINE TSharedPointer<_ObjectType> StaticPointerCast(
    const TSharedPointer<_OtherObjectType>& _ptr
) noexcept {
    TSharedPointer<_ObjectType> ptr;
    ptr.ptr_ = std::static_pointer_cast<_ObjectType>(_ptr.ptr_);
    return ptr;
}

template<typename _ObjectType, typename _OtherObjectType>
NODISCARD FORCEINLINE TSharedPointer<_ObjectType> StaticPointerCast(
    TSharedPointer<_OtherObjectType>&& _ptr
) noexcept {
    TSharedPointer<_ObjectType> ptr;
    ptr.ptr_ = std::static_pointer_cast<_ObjectType>(std::move(_ptr.ptr_));
    return ptr;
}

template<typename _ObjectType, typename _OtherObjectType>
NODISCARD FORCEINLINE TSharedPointer<_ObjectType> ConstPointerCast(
    const TSharedPointer<_OtherObjectType>& _ptr
) noexcept {
    TSharedPointer<_ObjectType> ptr;
    ptr.ptr_ = std::const_pointer_cast<_ObjectType>(_ptr.ptr_);
    return ptr;
}

template<typename _ObjectType, typename _OtherObjectType>
NODISCARD FORCEINLINE TSharedPointer<_ObjectType> ConstPointerCast(
    TSharedPointer<_OtherObjectType>&& _ptr
) noexcept {
    TSharedPointer<_ObjectType> ptr;
    ptr.ptr_ = std::const_pointer_cast<_ObjectType>(std::move(_ptr.ptr_));
    return ptr;
}

template<typename _ObjectType, typename _OtherObjectType>
NODISCARD FORCEINLINE TSharedPointer<_ObjectType> ReinterpretPointerCast(
    const TSharedPointer<_OtherObjectType>& _ptr
) noexcept {
    TSharedPointer<_ObjectType> ptr;
    ptr.ptr_ = std::reinterpret_pointer_cast<_ObjectType>(_ptr.ptr_);
    return ptr;
}

template<typename _ObjectType, typename _OtherObjectType>
NODISCARD FORCEINLINE TSharedPointer<_ObjectType> ReinterpretPointerCast(
    TSharedPointer<_OtherObjectType>&& _ptr
) noexcept {
    TSharedPointer<_ObjectType> ptr;
    ptr.ptr_ = std::reinterpret_pointer_cast<_ObjectType>(std::move(_ptr.ptr_));
    return ptr;
}

template<typename _ObjectType, typename _OtherObjectType>
NODISCARD FORCEINLINE TSharedPointer<_ObjectType> DynamicPointerCast(
    const TSharedPointer<_OtherObjectType>& _ptr
) noexcept {
    TSharedPointer<_ObjectType> ptr;
    ptr.ptr_ = std::dynamic_pointer_cast<_ObjectType>(_ptr.ptr_);
    return ptr;
}

template<typename _ObjectType, typename _OtherObjectType>
NODISCARD FORCEINLINE TSharedPointer<_ObjectType> DynamicPointerCast(
    TSharedPointer<_OtherObjectType>&& _ptr
) noexcept {
    TSharedPointer<_ObjectType> ptr;
    ptr.ptr_ = std::dynamic_pointer_cast<_ObjectType>(std::move(_ptr.ptr_));
    return ptr;
}

}//zengine