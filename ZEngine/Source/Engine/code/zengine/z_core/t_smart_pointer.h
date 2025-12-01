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

#include <memory>

#include "t_allocator.h"
#include "t_atom.h"
#include "z_object.h"

/*
    A simple fast smart ptr. Does not support custom deleter and constructor.
*/

namespace zengine {

namespace internal {

struct TControlBlockP : ZObject {
public:
    template<typename _ObjectType>
    static Void DeleteFuncP(Void* _obj_ptr) noexcept {
        delete static_cast<_ObjectType*>(_obj_ptr);
    }

    template<typename _ObjectType>
    FORCEINLINE TControlBlockP(_ObjectType* _obj_ptr) noexcept 
        : SuperType_()
        , obj_ptr_(_obj_ptr)
        , obj_delete_func_(DeleteFuncP<_ObjectType>)
        , use_count_(1)
        , weak_count_(0) {}
    virtual ~TControlBlockP() noexcept {}

    NODISCARD FORCEINLINE static Void* operator new(SizeType _size) noexcept {
        return memory_pool::ApplySmartPointerMemory();
    }
    NODISCARD FORCEINLINE static Void operator delete(Void* _memory_ptr) noexcept {
        memory_pool::ReleaseSmartPointerMemory(_memory_ptr);
    }

    NODISCARD FORCEINLINE virtual Void DestroyObject() noexcept {
        obj_delete_func_(obj_ptr_);
    }

protected:
    using SuperType_ = ZObject;

public:
    Void* obj_ptr_;
    Void(*obj_delete_func_)(Void*);
#ifdef SMART_POINTER_THREAD_SAFE
    TAtom<Int32> use_count_;
    TAtom<Int32> weak_count_;
#else
    Int32 use_count_;
    Int32 weak_count_;
#endif//SMART_POINTER_THREAD_SAFE
};

}//internal

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
    FORCEINLINE TUniquePointer() noexcept : SuperType_(), ptr_(nullptr) {}
    FORCEINLINE TUniquePointer(NullptrType _ptr) noexcept : SuperType_(), ptr_(nullptr) {}

    template<typename _SrcObjectType>
    FORCEINLINE TUniquePointer(_SrcObjectType* _ptr) noexcept : SuperType_(), ptr_(static_cast<_ObjectType*>(_ptr)) {}

    template<typename _SrcObjectType>
    TUniquePointer(TUniquePointer<_SrcObjectType>&& _ptr) noexcept 
        : SuperType_()
    {
        ptr_ = static_cast<_ObjectType*>(_ptr.ptr_);
        _ptr.ptr_ = nullptr;
    }

    ~TUniquePointer() noexcept {
        if (ptr_ != nullptr) {
            delete ptr_;
            ptr_ = nullptr;
        }
    }

    template<typename _SrcObjectType>
    FORCEINLINE TUniquePointer& operator=(TUniquePointer<_SrcObjectType>&& _ptr) noexcept {
        MoveP(std::forward<TUniquePointer<_SrcObjectType>>(_ptr));
        return *this;
    }

    template<typename _PtrType>
    FORCEINLINE Bool operator==(_PtrType _ptr) const noexcept {
        return ptr_ == _ptr;
    }

    NODISCARD FORCEINLINE _ObjectType& operator*() noexcept { return *ptr_; }
    NODISCARD FORCEINLINE const _ObjectType& operator*() const noexcept { return *ptr_; }
    NODISCARD FORCEINLINE _ObjectType* operator->() noexcept { return ptr_; }
    NODISCARD FORCEINLINE const _ObjectType* operator->() const noexcept { return ptr_; }
    NODISCARD FORCEINLINE operator Bool() const noexcept { return ptr_ != nullptr; }

    NODISCARD FORCEINLINE _ObjectType* GetPtr() noexcept { return ptr_; }
    NODISCARD FORCEINLINE const _ObjectType* GetPtr() const noexcept { return ptr_; }

    NODISCARD FORCEINLINE _ObjectType* Release() noexcept { 
        _ObjectType* ret_ptr = ptr_;
        ptr_ = nullptr;
        return ret_ptr;
    }

    Void Reset() noexcept { 
        if (ptr_) {
            delete ptr_;
            ptr_ = nullptr;
        }
    }
    Void Reset(_ObjectType* _ptr) noexcept { 
        if (ptr_) {
            delete ptr_;
        }
        ptr_ = _ptr;
    }
    FORCEINLINE Void Swap(TUniquePointer& _ptr) noexcept { 
        _ObjectType* temp_ptr = _ptr.ptr_;
        _ptr.ptr_ = ptr_;
        ptr_ = temp_ptr;
    }

protected:
    using SuperType_ = ZObject;
    template<typename _SrcObjectType>
    friend class TUniquePointer;

private:
    TUniquePointer(const TUniquePointer&) = delete;
    TUniquePointer& operator=(const TUniquePointer&) = delete;

    template<typename _SrcObjectType>
    FORCEINLINE Void MoveP(TUniquePointer<_SrcObjectType>&& _ptr) noexcept {
        if (ptr_ != nullptr) {
            delete ptr_;
        }
        ptr_ = static_cast<_ObjectType*>(_ptr.ptr_);
        _ptr.ptr_ = nullptr;
    }

    _ObjectType* ptr_;
};

/*
    Shared smart pointer.
*/
template<typename _ObjectType>
class TSharedPointer : public ZObject {
public:
    FORCEINLINE TSharedPointer() noexcept 
        : SuperType_(), obj_ptr_(nullptr), ctrl_block_ptr_(nullptr) {}
    FORCEINLINE TSharedPointer(NullptrType _ptr) noexcept 
        : SuperType_(), obj_ptr_(nullptr), ctrl_block_ptr_(nullptr) {}
    TSharedPointer(_ObjectType* _ptr) noexcept
        : SuperType_()
        , obj_ptr_(_ptr)
    {
        if (_ptr != nullptr) {
            ctrl_block_ptr_ = new internal::TControlBlockP(_ptr);
        }
    }
    template<typename _SrcObjectType>
    TSharedPointer(const TSharedPointer<_SrcObjectType>& _ptr) noexcept
        : SuperType_()
    {
        CopyP(_ptr);
        AddShareP();
    }
    template<typename _SrcObjectType>
    TSharedPointer(TSharedPointer<_SrcObjectType>&& _ptr) noexcept
        : SuperType_()
    {
        MoveP(std::forward<TSharedPointer<_SrcObjectType>>(_ptr));
    }
    template<typename _SrcObjectType>
    TSharedPointer(const TWeakPointer<_SrcObjectType>& _ptr) noexcept
        : SuperType_()
    {
        if (!_ptr.Expired()) {
            obj_ptr_ = reinterpret_cast<_ObjectType*>(_ptr.ctrl_block_ptr_->obj_ptr_);
            ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
            AddShareP();
        }
    }
    template<typename _SrcObjectType>
    TSharedPointer(TUniquePointer<_SrcObjectType>&& _ptr) noexcept
        : SuperType_()
        , obj_ptr_(_ptr.Release())
        , ctrl_block_ptr_(new internal::TControlBlockP(obj_ptr_)) {}

    ~TSharedPointer() noexcept {
        RemoveShareP();
    }

    template<typename _SrcObjectType>
    TSharedPointer& operator=(const TSharedPointer<_SrcObjectType>& _ptr) noexcept {
        SuperType_::operator=(_ptr);
        RemoveShareP();
        CopyP(_ptr);
        AddShareP();
        return *this;
    }
    template<typename _SrcObjectType>
    TSharedPointer& operator=(TSharedPointer<_SrcObjectType>&& _ptr) noexcept {
        SuperType_::operator=(std::forward<TSharedPointer<_SrcObjectType>>(_ptr));
        RemoveShareP();
        MoveP(std::forward<TSharedPointer<_SrcObjectType>>(_ptr));
        return *this;
    }

    template<typename _PtrType>
    FORCEINLINE Bool operator==(_PtrType _ptr) const noexcept {
        return obj_ptr_ == _ptr;
    }
    template<typename _SrcObjectType>
    FORCEINLINE Bool operator==(const TSharedPointer<_SrcObjectType>& _ptr) const noexcept {
        return obj_ptr_ == _ptr.obj_ptr_;
    }

    NODISCARD FORCEINLINE _ObjectType& operator*() noexcept { return *obj_ptr_; }
    NODISCARD FORCEINLINE const _ObjectType& operator*() const noexcept { return *obj_ptr_; }
    NODISCARD FORCEINLINE _ObjectType* operator->() noexcept { return obj_ptr_; }
    NODISCARD FORCEINLINE const _ObjectType* operator->() const noexcept { return obj_ptr_; }
    NODISCARD FORCEINLINE operator Bool() const noexcept { return obj_ptr_ != nullptr; }

    NODISCARD FORCEINLINE _ObjectType* GetPtr() noexcept { return obj_ptr_; }
    NODISCARD FORCEINLINE const _ObjectType* GetPtr() const noexcept { return obj_ptr_; }

    NODISCARD _ObjectType* Release() noexcept { 
        _ObjectType* ret_ptr = obj_ptr_;
        RemoveShareP();
        obj_ptr_ = nullptr;
        ctrl_block_ptr_ = nullptr;
        return ret_ptr;
    }

    NODISCARD FORCEINLINE Int32 UseCount() const noexcept { return ctrl_block_ptr_->use_count_; }
    NODISCARD FORCEINLINE Bool Unique() const noexcept { return ctrl_block_ptr_->use_count_ == 1; }

    Void Reset() noexcept {
        RemoveShareP();
        obj_ptr_ = nullptr;
        ctrl_block_ptr_ = nullptr;
    }
    Void Reset(NullptrType _ptr) noexcept {
        RemoveShareP();
        obj_ptr_ = nullptr;
        ctrl_block_ptr_ = nullptr;
    }   
    template<typename _SrcObjectType>
    Void Reset(_SrcObjectType* _ptr) noexcept {
        RemoveShareP();
        obj_ptr_ = _ptr;
        ctrl_block_ptr_ = new internal::TControlBlockP(_ptr);
    }
    Void Swap(TSharedPointer& _ptr) noexcept { 
        _ObjectType* temp_obj_ptr = _ptr.obj_ptr_;
        internal::TControlBlockP* temp_ctrl_block_ptr;
        obj_ptr_ = _ptr.obj_ptr_;
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        _ptr.obj_ptr_ = temp_obj_ptr;
        _ptr.ctrl_block_ptr_ = temp_ctrl_block_ptr;
    }

protected:
    using SuperType_ = ZObject;

    template<typename _ObjectType>
    friend class TSharedPointer;
    template<typename _ObjectType>
    friend class TWeakPointer;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType> StaticPointerCast(
        TSharedPointer<_SrcObjectType>& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType> StaticPointerCast(
        TSharedPointer<_SrcObjectType>&& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType> ConstPointerCast(
        const TSharedPointer<_SrcObjectType>& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType> ConstPointerCast(
        TSharedPointer<_SrcObjectType>&& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType> ReinterpretPointerCast(
        const TSharedPointer<_SrcObjectType>& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType> ReinterpretPointerCast(
        TSharedPointer<_SrcObjectType>&& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType> DynamicPointerCast(
        const TSharedPointer<_SrcObjectType>& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType> DynamicPointerCast(
        TSharedPointer<_SrcObjectType>&& _ptr
    ) noexcept;

private:
    template<typename _SrcObjectType>
    FORCEINLINE Void MoveP(TSharedPointer<_SrcObjectType>&& _ptr) noexcept {
        obj_ptr_ = static_cast<_ObjectType*>(_ptr.obj_ptr_);
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        _ptr.obj_ptr_ = nullptr;
        _ptr.ctrl_block_ptr_ = nullptr;
    }

    template<typename _SrcObjectType>
    FORCEINLINE Void CopyP(const TSharedPointer<_SrcObjectType>& _ptr) noexcept {
        obj_ptr_ = static_cast<_ObjectType*>(_ptr.obj_ptr_);
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    }

    FORCEINLINE Void AddShareP() noexcept {
        if (ctrl_block_ptr_ != nullptr && obj_ptr_ != nullptr) {
            ++ctrl_block_ptr_->use_count_;
        }
    }

    FORCEINLINE Void RemoveShareP() noexcept {
        if (ctrl_block_ptr_ != nullptr && obj_ptr_ != nullptr) {
            --ctrl_block_ptr_->use_count_;
            //delete object
            if (ctrl_block_ptr_->use_count_ == 0) {
                ctrl_block_ptr_->DestroyObject();
                //delete control block
                if (ctrl_block_ptr_->weak_count_ == 0) {
                    delete ctrl_block_ptr_;
                }
            }
        }
    }

    _ObjectType* obj_ptr_;
    internal::TControlBlockP* ctrl_block_ptr_;
};

/*
    Weak smart pointer.
*/
template<typename _ObjectType>
class TWeakPointer : public ZObject {
public:
    FORCEINLINE TWeakPointer() noexcept : SuperType_(), ctrl_block_ptr_(nullptr) {}
    template<typename _SrcObjectType>
    TWeakPointer(const TWeakPointer<_SrcObjectType>& _ptr) noexcept
        : SuperType_(_ptr)
    {
        CopyP();
        AddWeakP();
    }
    template<typename _SrcObjectType>
    FORCEINLINE TWeakPointer(TWeakPointer<_SrcObjectType>&& _ptr) noexcept
        : SuperType_(std::forward<TWeakPointer<_SrcObjectType>>(_ptr)) 
    {
        MoveP();
    }

    template<typename _SrcObjectType>
    TWeakPointer(const TSharedPointer<_SrcObjectType>& _ptr) noexcept
        : SuperType_(_ptr)
        , ctrl_block_ptr_(_ptr.ctrl_block_ptr_)
    {
        AddWeakP();
    }

    ~TWeakPointer() noexcept {
        RemoveWeakP();
    }

    template<typename _SrcObjectType>
    TWeakPointer& operator=(const TWeakPointer<_SrcObjectType>& _ptr) noexcept {
        SuperType_::operator=(_ptr);
        RemoveWeakP();
        CopyP(_ptr);
        AddWeakP();
        return *this;
    }
    template<typename _SrcObjectType>
    TWeakPointer& operator=(TWeakPointer<_SrcObjectType>&& _ptr) noexcept {
        SuperType_::operator=(std::forward<TWeakPointer<_SrcObjectType>>(_ptr));
        RemoveWeakP();
        MoveP(_ptr);
        return *this;
    }
    template<typename _SrcObjectType>
    TWeakPointer& operator=(const TSharedPointer<_SrcObjectType>& _ptr) noexcept {
        SuperType_::operator=(_ptr);
        RemoveWeakP();
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        AddWeakP();
        return *this;
    }

    TSharedPointer<_ObjectType> Lock() const noexcept {
        TSharedPointer<_ObjectType> ptr;
        if (!Expired()) {
            ptr.obj_ptr_ = reinterpret_cast<_ObjectType*>(ctrl_block_ptr_->obj_ptr_);
            ptr.ctrl_block_ptr_ = ctrl_block_ptr_;
            ptr.AddShareP();
        }
        return ptr;
    }
    NODISCARD FORCEINLINE Int32 UseCount() const noexcept { 
        return  ctrl_block_ptr_ == nullptr ? 0 : ctrl_block_ptr_->use_count_; 
    }
    NODISCARD FORCEINLINE Bool Expired() const noexcept { 
        return ctrl_block_ptr_ == nullptr ? true : (ctrl_block_ptr_->use_count_ == 0);
    }

    Void Reset() noexcept { 
        RemoveWeakP();
        ctrl_block_ptr_ = nullptr;
    }
    Void Swap(TWeakPointer& _ptr) noexcept { 
        internal::TControlBlockP* temp_ctrl_block_ptr;
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        _ptr.ctrl_block_ptr_ = temp_ctrl_block_ptr;
    }

protected:
    using SuperType_ = ZObject;

private:
    template<typename _SrcObjectType>
    FORCEINLINE Void MoveP(TWeakPointer<_SrcObjectType>&& _ptr) noexcept {
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        _ptr.ctrl_block_ptr_ = nullptr;
    }

    template<typename _SrcObjectType>
    FORCEINLINE Void CopyP(const TWeakPointer<_SrcObjectType>& _ptr) noexcept {
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    }

    FORCEINLINE Void AddWeakP() noexcept {
        if (ctrl_block_ptr_ != nullptr) {
            ++ctrl_block_ptr_->weak_count_;
        }
    }

    FORCEINLINE Void RemoveWeakP() noexcept {
        if (ctrl_block_ptr_ != nullptr) {
            //delete control block
            if (ctrl_block_ptr_->use_count_ == 0 && ctrl_block_ptr_->weak_count_ == 1) {
                delete ctrl_block_ptr_;
                ctrl_block_ptr_ = nullptr;
            }
            else {
                --ctrl_block_ptr_->weak_count_;
            }
        }
    }

    internal::TControlBlockP* ctrl_block_ptr_;
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
    return TUniquePointer<_ObjectType>(new _ObjectType(std::forward<_ArgsType>(_args)...));
}


template<typename _ObjectType, typename... _ArgsType>
NODISCARD FORCEINLINE TSharedPointer<_ObjectType> MakeShared(_ArgsType&&... _args) noexcept {
    return TSharedPointer<_ObjectType>(new _ObjectType(std::forward<_ArgsType>(_args)...));
}

template<typename _ResultObjectType, typename _SrcObjectType>
NODISCARD TSharedPointer<_ResultObjectType> StaticPointerCast(
    TSharedPointer<_SrcObjectType>& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType> ptr;
    ptr.obj_ptr_ = (static_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    ptr.AddShareP();
    return ptr;
}

template<typename _ResultObjectType, typename _SrcObjectType>
NODISCARD TSharedPointer<_ResultObjectType> StaticPointerCast(
    TSharedPointer<_SrcObjectType>&& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType> ptr;
    ptr.obj_ptr_ = (static_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    _ptr.obj_ptr_ = nullptr;
    _ptr.ctrl_block_ptr_ = nullptr;
    return ptr;
}

template<typename _ResultObjectType, typename _SrcObjectType>
NODISCARD TSharedPointer<_ResultObjectType> ConstPointerCast(
    const TSharedPointer<_SrcObjectType>& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType> ptr;
    ptr.obj_ptr_ = (const_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    ptr.AddShareP();
    return ptr;
}

template<typename _ResultObjectType, typename _SrcObjectType>
NODISCARD TSharedPointer<_ResultObjectType> ConstPointerCast(
    TSharedPointer<_SrcObjectType>&& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType> ptr;
    ptr.obj_ptr_ = (const_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    _ptr.obj_ptr_ = nullptr;
    _ptr.ctrl_block_ptr_ = nullptr;
    return ptr;
}

template<typename _ResultObjectType, typename _SrcObjectType>
NODISCARD TSharedPointer<_ResultObjectType> ReinterpretPointerCast(
    const TSharedPointer<_SrcObjectType>& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType> ptr;
    ptr.obj_ptr_ = (reinterpret_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    ptr.AddShareP();
    return ptr;
}

template<typename _ResultObjectType, typename _SrcObjectType>
NODISCARD TSharedPointer<_ResultObjectType> ReinterpretPointerCast(
    TSharedPointer<_SrcObjectType>&& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType> ptr;
    ptr.obj_ptr_ = (reinterpret_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    _ptr.obj_ptr_ = nullptr;
    _ptr.ctrl_block_ptr_ = nullptr;
    return ptr;
}

template<typename _ResultObjectType, typename _SrcObjectType>
NODISCARD TSharedPointer<_ResultObjectType> DynamicPointerCast(
    const TSharedPointer<_SrcObjectType>& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType> ptr;
    ptr.obj_ptr_ = (dynamic_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    if (ptr.obj_ptr_ != nullptr) {
        ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        ptr.AddShareP();
    }
    return ptr;
}

template<typename _ResultObjectType, typename _SrcObjectType>
NODISCARD TSharedPointer<_ResultObjectType> DynamicPointerCast(
    TSharedPointer<_SrcObjectType>&& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType> ptr;
    ptr.obj_ptr_ = (reinterpret_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    if (ptr.obj_ptr_ != nullptr) {
        ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        _ptr.obj_ptr_ = nullptr;
        _ptr.ctrl_block_ptr_ = nullptr;
    }
    return ptr;
}

}//zengine