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
#pragma once

#include "drive.h"

#include "t_atom.h"
#include "z_object.h"

/*
    A simple fast smart ptr. Does not support custom deleter and constructor.
*/

namespace zengine {

namespace internal {

/**
 * @brief Control block for shared and weak pointers to manage reference counts and object lifetime.
 * @tparam _IsGlobal Whether to use the global memory pool for allocation.
 */
template<Bool _IsGlobal>
struct TControlBlockP {
public:
    /**
     * @brief Static function to delete the managed object.
     * @tparam _ObjectType The type of the object being managed.
     * @param _obj_ptr Pointer to the object to be deleted.
     */
    template< typename _ObjectType>
    FORCEINLINE static Void DeleteFuncP(Void* _obj_ptr) noexcept {
        if constexpr (_IsGlobal) {
            delete reinterpret_cast<_ObjectType*>(_obj_ptr);
        }
        else {
            DELETE_LOCAL(reinterpret_cast<_ObjectType*>(_obj_ptr));
        }
    }

    /**
     * @brief Constructor for the control block.
     * @tparam _ObjectType The type of the object being managed.
     * @param _obj_ptr Pointer to the managed object.
     */
    template< typename _ObjectType>
    FORCEINLINE TControlBlockP(_ObjectType* _obj_ptr) noexcept 
        : obj_ptr_(_obj_ptr)
        , obj_delete_func_(DeleteFuncP<_ObjectType>)
        , use_count_(1)
        , weak_count_(0) {}

    /**
     * @brief Destructor for the control block.
     */
    virtual ~TControlBlockP() noexcept = default;

    /**
     * @brief Overloaded new operator to allocate memory from the smart pointer control block pool.
     * @param _size The size of memory to allocate.
     * @return Void* Pointer to the allocated memory.
     */
    NODISCARD FORCEINLINE static Void* operator new(SizeType _size) noexcept {
        return memory_pool::ApplySmartPointerControlBlockMemory();
    }

    /**
     * @brief Overloaded delete operator to release memory to the smart pointer control block pool.
     * @param _memory_ptr Pointer to the memory to release.
     */
    NODISCARD FORCEINLINE static Void operator delete(Void* _memory_ptr) noexcept {
        memory_pool::ReleaseSmartPointerControlBlockMemory(_memory_ptr);
    }

    /**
     * @brief Destroys the managed object using the stored delete function.
     */
    NODISCARD FORCEINLINE virtual Void DestroyObject() noexcept {
        obj_delete_func_(obj_ptr_);
    }

public:
    /** @brief Pointer to the actual managed object. */
    Void* obj_ptr_;
    /** @brief Function pointer to the deletion function for the object. */
    Void(*obj_delete_func_)(Void*);
#ifdef SMART_POINTER_THREAD_SAFE
    /** @brief Atomic reference count for shared owners. */
    TAtom<Int32> use_count_;
    /** @brief Atomic reference count for weak observers. */
    TAtom<Int32> weak_count_;
#else
    /** @brief Reference count for shared owners. */
    Int32 use_count_;
    /** @brief Reference count for weak observers. */
    Int32 weak_count_;
#endif // SMART_POINTER_THREAD_SAFE
};

} // internal

template<typename _ObjectType, Bool _IsGlobal>
class TUniquePointer;
template<typename _ObjectType, Bool _IsGlobal>
class TSharedPointer;
template<typename _ObjectType, Bool _IsGlobal>
class TWeakPointer;

/*
    Unique smart pointer.
*/
/**
 * @brief Unique ownership smart pointer.
 * This class implements RAII (Resource Acquisition Is Initialization) for exclusive object ownership.
 * It ensures that the pointed object is deleted when the TUniquePointer goes out of scope.
 * It cannot be copied, only moved.
 * @tparam _ObjectType The type of the object to manage.
 * @tparam _IsGlobal Whether to use the global memory pool for allocation.
 */
template< typename _ObjectType, Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL>
class TUniquePointer {
    NO_NEW_DELETE
public:
    /**
     * @brief Default constructor. Initializes the pointer to nullptr.
     */
    FORCEINLINE TUniquePointer() noexcept : ptr_(nullptr) {}

    /**
     * @brief Constructor from nullptr.
     * @param _ptr Nullptr constant.
     */
    FORCEINLINE TUniquePointer(NullptrType _ptr) noexcept : ptr_(nullptr) {}

    /**
     * @brief Constructor from a raw pointer.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The raw pointer to take ownership of.
     */
    template<typename _SrcObjectType>
    FORCEINLINE TUniquePointer(_SrcObjectType* _ptr) noexcept : ptr_(static_cast<_ObjectType*>(_ptr)) {}

    /**
     * @brief Move constructor. Transfers ownership from another TUniquePointer.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source unique pointer to move from.
     */
    template<typename _SrcObjectType>
    TUniquePointer(TUniquePointer<_SrcObjectType, _IsGlobal>&& _ptr) noexcept {
        ptr_ = static_cast<_ObjectType*>(_ptr.ptr_);
        _ptr.ptr_ = nullptr;
    }

    /**
     * @brief Destructor. Deletes the managed object if it exists.
     */
    ~TUniquePointer() noexcept {
        if (ptr_ != nullptr) {
            if constexpr (_IsGlobal) {
                delete ptr_;
            } else {
                DELETE_LOCAL(ptr_);
            }
            ptr_ = nullptr;
        }
    }

    /**
     * @brief Move assignment operator. Transfers ownership from another TUniquePointer.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source unique pointer to move from.
     * @return TUniquePointer& Reference to this unique pointer.
     */
    template<typename _SrcObjectType>
    FORCEINLINE TUniquePointer& operator=(TUniquePointer<_SrcObjectType, _IsGlobal>&& _ptr) noexcept {
        if (this == &_ptr) {
            return *this;
        }
        if constexpr (_IsGlobal) {
            delete ptr_;
        }
        else {
            DELETE_LOCAL(ptr_);
        }
        MoveP(std::move(_ptr));
        return *this;
    }

    /**
     * @brief Equality comparison with a raw pointer or another compatible pointer type.
     * @tparam _PtrType The type of the pointer to compare with.
     * @param _ptr The pointer to compare.
     * @return Bool True if the pointers are equal, otherwise false.
     */
    template<typename _PtrType>
    FORCEINLINE Bool operator==(_PtrType _ptr) const noexcept {
        return ptr_ == _ptr;
    }

    /**
     * @brief Dereference operator.
     * @return _ObjectType& Reference to the managed object.
     */
    NODISCARD FORCEINLINE _ObjectType& operator*() noexcept { return *ptr_; }

    /**
     * @brief Const dereference operator.
     * @return const _ObjectType& Const reference to the managed object.
     */
    NODISCARD FORCEINLINE const _ObjectType& operator*() const noexcept { return *ptr_; }

    /**
     * @brief Member access operator.
     * @return _ObjectType* Pointer to the managed object.
     */
    NODISCARD FORCEINLINE _ObjectType* operator->() noexcept { return ptr_; }

    /**
     * @brief Const member access operator.
     * @return const _ObjectType* Const pointer to the managed object.
     */
    NODISCARD FORCEINLINE const _ObjectType* operator->() const noexcept { return ptr_; }

    /**
     * @brief Conversion operator to Bool.
     * @return Bool True if the pointer is not null, otherwise false.
     */
    NODISCARD FORCEINLINE operator Bool() const noexcept { return ptr_ != nullptr; }

    /**
     * @brief Gets the raw pointer.
     * @return _ObjectType* The raw pointer.
     */
    NODISCARD FORCEINLINE _ObjectType* GetPtr() noexcept { return ptr_; }

    /**
     * @brief Gets the raw pointer (const version).
     * @return const _ObjectType* The const raw pointer.
     */
    NODISCARD FORCEINLINE const _ObjectType* GetPtr() const noexcept { return ptr_; }

    /**
     * @brief Releases ownership of the managed object without deleting it.
     * @return _ObjectType* The raw pointer to the object.
     */
    NODISCARD FORCEINLINE _ObjectType* Release() noexcept { 
        _ObjectType* ret_ptr = ptr_;
        ptr_ = nullptr;
        return ret_ptr;
    }

    /**
     * @brief Resets the unique pointer, deleting the managed object.
     */
    Void Reset() noexcept { 
        if (ptr_) {
            if constexpr (_IsGlobal) {
                delete ptr_;
            }
            else {
                DELETE_LOCAL(ptr_);
            }
            ptr_ = nullptr;
        }
    }

    /**
     * @brief Resets the unique pointer with a new pointer, deleting the old object.
     * @param _ptr The new raw pointer to take ownership of.
     */
    Void Reset(_ObjectType* _ptr) noexcept { 
        if (ptr_) {
            if constexpr (_IsGlobal) {
                delete ptr_;
            }
            else {
                DELETE_LOCAL(ptr_);
            }
        }
        ptr_ = _ptr;
    }

    /**
     * @brief Swaps the contents with another TUniquePointer.
     * @param _ptr The other unique pointer to swap with.
     */
    FORCEINLINE Void Swap(TUniquePointer& _ptr) noexcept { 
        _ObjectType* temp_ptr = _ptr.ptr_;
        _ptr.ptr_ = ptr_;
        ptr_ = temp_ptr;
    }

protected:
    template<typename _OtherObjectType, Bool _IsAlsoGlobal>
    friend class TUniquePointer;

private:
    TUniquePointer(const TUniquePointer&) = delete;
    TUniquePointer& operator=(const TUniquePointer&) = delete;

    /**
     * @brief Internal helper to move ownership.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source unique pointer to move from.
     */
    template<typename _SrcObjectType>
    FORCEINLINE Void MoveP(TUniquePointer<_SrcObjectType, _IsGlobal>&& _ptr) noexcept {
        ptr_ = static_cast<_ObjectType*>(_ptr.ptr_);
        _ptr.ptr_ = nullptr;
    }

    /** @brief The raw pointer to the managed object. */
    _ObjectType* ptr_;
};

/*
    Shared smart pointer.
*/
/**
 * @brief Shared ownership smart pointer.
 * This class implements reference counting. The managed object is deleted only when the last 
 * TSharedPointer owning it is destroyed or reset. It uses a separate control block to track usage.
 * @tparam _ObjectType The type of the object to manage.
 * @tparam _IsGlobal Whether to use the global memory pool for allocation.
 */
template< typename _ObjectType, Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL>
class TSharedPointer {
    NO_NEW_DELETE
private:
    using ControlBlockType_ = internal::TControlBlockP<_IsGlobal>;

public:
    /**
     * @brief Default constructor. Initializes the pointer to nullptr.
     */
    FORCEINLINE TSharedPointer() noexcept 
        : obj_ptr_(nullptr), ctrl_block_ptr_(nullptr) {}

    /**
     * @brief Constructor from nullptr.
     * @param _ptr Nullptr constant.
     */
    FORCEINLINE TSharedPointer(NullptrType _ptr) noexcept 
        : obj_ptr_(nullptr), ctrl_block_ptr_(nullptr) {}

    /**
     * @brief Constructor from a raw pointer. Allocates a new control block.
     * @param _ptr The raw pointer to take ownership of.
     */
    TSharedPointer(_ObjectType* _ptr) noexcept
        : obj_ptr_(_ptr)
    {
        if (_ptr != nullptr) {
            ctrl_block_ptr_ = new ControlBlockType_(_ptr);
        }
        else {
            ctrl_block_ptr_ = nullptr;
        }
    }

    /**
     * @brief Copy constructor. Increases the shared reference count.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source shared pointer to copy.
     */
    template<typename _SrcObjectType>
    TSharedPointer(const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr) noexcept {
        CopyP(_ptr);
        AddShareP();
    }

    /**
     * @brief Move constructor. Transfers ownership and control block without changing counts.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source shared pointer to move.
     */
    template<typename _SrcObjectType>
    TSharedPointer(TSharedPointer<_SrcObjectType, _IsGlobal>&& _ptr) noexcept {
        MoveP(std::move(_ptr));
    }

    /**
     * @brief Constructor from a weak pointer. Attempts to lock the weak pointer.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The weak pointer to create a shared pointer from.
     */
    template<typename _SrcObjectType>
    TSharedPointer(const TWeakPointer<_SrcObjectType, _IsGlobal>& _ptr) noexcept {
        if (!_ptr.Expired()) {
            obj_ptr_ = reinterpret_cast<_ObjectType*>(_ptr.ctrl_block_ptr_->obj_ptr_);
            ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
            AddShareP();
        }
    }

    /**
     * @brief Constructor from a unique pointer. Takes ownership and allocates a control block.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The unique pointer to move from.
     */
    template<typename _SrcObjectType>
    TSharedPointer(TUniquePointer<_SrcObjectType, _IsGlobal>&& _ptr) noexcept
        : obj_ptr_(_ptr.Release())
        , ctrl_block_ptr_(new ControlBlockType_(obj_ptr_)) 
    {
        if (obj_ptr_ != nullptr) {
            ctrl_block_ptr_ = new ControlBlockType_(obj_ptr_);
        }
    }

    /**
     * @brief Destructor. Decrements the reference count and destroys object if count reaches zero.
     */
    ~TSharedPointer() noexcept {
        RemoveShareP();
    }

    /**
     * @brief Copy assignment operator.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source shared pointer to copy.
     * @return TSharedPointer& Reference to this shared pointer.
     */
    template<typename _SrcObjectType>
    TSharedPointer& operator=(const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr) noexcept {
        if (this == &_ptr) {
            return *this;
        }
        RemoveShareP();
        CopyP(_ptr);
        AddShareP();
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source shared pointer to move.
     * @return TSharedPointer& Reference to this shared pointer.
     */
    template<typename _SrcObjectType>
    TSharedPointer& operator=(TSharedPointer<_SrcObjectType, _IsGlobal>&& _ptr) noexcept {
        if (this == &_ptr) {
            return *this;
        }
        RemoveShareP();
        MoveP(std::move(_ptr));
        return *this;
    }

    /**
     * @brief Equality comparison with a raw pointer or compatible type.
     * @tparam _PtrType The type of the pointer to compare.
     * @param _ptr The pointer to compare with.
     * @return Bool True if equal, otherwise false.
     */
    template<typename _PtrType>
    FORCEINLINE Bool operator==(_PtrType _ptr) const noexcept {
        return obj_ptr_ == _ptr;
    }

    /**
     * @brief Equality comparison with another shared pointer.
     * @tparam _SrcObjectType The type of the other shared pointer's object.
     * @param _ptr The other shared pointer to compare with.
     * @return Bool True if they point to the same object, otherwise false.
     */
    template<typename _SrcObjectType>
    FORCEINLINE Bool operator==(const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr) const noexcept {
        return obj_ptr_ == _ptr.obj_ptr_;
    }

    /**
     * @brief Dereference operator.
     * @return _ObjectType& Reference to the managed object.
     */
    NODISCARD FORCEINLINE _ObjectType& operator*() noexcept { return *obj_ptr_; }

    /**
     * @brief Const dereference operator.
     * @return const _ObjectType& Const reference to the managed object.
     */
    NODISCARD FORCEINLINE const _ObjectType& operator*() const noexcept { return *obj_ptr_; }

    /**
     * @brief Member access operator.
     * @return _ObjectType* Pointer to the managed object.
     */
    NODISCARD FORCEINLINE _ObjectType* operator->() noexcept { return obj_ptr_; }

    /**
     * @brief Const member access operator.
     * @return const _ObjectType* Const pointer to the managed object.
     */
    NODISCARD FORCEINLINE const _ObjectType* operator->() const noexcept { return obj_ptr_; }

    /**
     * @brief Conversion operator to Bool.
     * @return Bool True if the pointer is not null, otherwise false.
     */
    NODISCARD FORCEINLINE operator Bool() const noexcept { return obj_ptr_ != nullptr; }

    /**
     * @brief Gets the raw pointer.
     * @return _ObjectType* The raw pointer.
     */
    NODISCARD FORCEINLINE _ObjectType* GetPtr() noexcept { return obj_ptr_; }

    /**
     * @brief Gets the raw pointer (const version).
     * @return const _ObjectType* The const raw pointer.
     */
    NODISCARD FORCEINLINE const _ObjectType* GetPtr() const noexcept { return obj_ptr_; }

    /**
     * @brief Releases ownership, resetting the shared pointer without destroying the object (if other references exist).
     * Warning: This implementation detaches the current pointer but returns the raw pointer. 
     * Since this is a SharedPointer, the object might still be owned by others or deleted if this was the last one.
     * @return _ObjectType* The raw pointer.
     */
    NODISCARD _ObjectType* Release() noexcept { 
        _ObjectType* ret_ptr = obj_ptr_;
        RemoveShareP();
        obj_ptr_ = nullptr;
        ctrl_block_ptr_ = nullptr;
        return ret_ptr;
    }

    /**
     * @brief Returns the current reference count.
     * @return Int32 The number of shared pointers managing this object.
     */
    NODISCARD FORCEINLINE Int32 UseCount() const noexcept { return ctrl_block_ptr_->use_count_; }

    /**
     * @brief Checks if this is the only shared pointer managing the object.
     * @return Bool True if use count is 1, otherwise false.
     */
    NODISCARD FORCEINLINE Bool Unique() const noexcept { return ctrl_block_ptr_->use_count_ == 1; }

    /**
     * @brief Resets the shared pointer to nullptr, decrementing the reference count.
     */
    Void Reset() noexcept {
        RemoveShareP();
        obj_ptr_ = nullptr;
        ctrl_block_ptr_ = nullptr;
    }

    /**
     * @brief Resets the shared pointer to nullptr.
     * @param _ptr Nullptr constant.
     */
    Void Reset(NullptrType _ptr) noexcept {
        RemoveShareP();
        obj_ptr_ = nullptr;
        ctrl_block_ptr_ = nullptr;
    }   

    /**
     * @brief Resets the shared pointer to manage a new object.
     * @tparam _SrcObjectType The type of the new object.
     * @param _ptr The new raw pointer to manage.
     */
    template<typename _SrcObjectType>
    Void Reset(_SrcObjectType* _ptr) noexcept {
        RemoveShareP();
        obj_ptr_ = _ptr;
        ctrl_block_ptr_ = new ControlBlockType_(_ptr);
    }

    /**
     * @brief Swaps contents with another TSharedPointer.
     * @param _ptr The other shared pointer to swap with.
     */
    Void Swap(TSharedPointer& _ptr) noexcept { 
        _ObjectType* temp_obj_ptr = _ptr.obj_ptr_;
        ControlBlockType_* temp_ctrl_block_ptr;
        obj_ptr_ = _ptr.obj_ptr_;
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        _ptr.obj_ptr_ = temp_obj_ptr;
        _ptr.ctrl_block_ptr_ = temp_ctrl_block_ptr;
    }

protected:
    template<typename _OtherObjectType, Bool _IsAlsoGlobal>
    friend class TSharedPointer;
    template<typename _OtherObjectType, Bool _IsAlsoGlobal>
    friend class TWeakPointer;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> StaticPointerCast(
        const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> StaticPointerCast(
        TSharedPointer<_SrcObjectType, _IsGlobal>&& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> ConstPointerCast(
        const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> ConstPointerCast(
        TSharedPointer<_SrcObjectType, _IsGlobal>&& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> ReinterpretPointerCast(
        const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> ReinterpretPointerCast(
        TSharedPointer<_SrcObjectType, _IsGlobal>&& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> DynamicPointerCast(
        const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr
    ) noexcept;

    template<typename _ResultObjectType, typename _SrcObjectType>
    friend NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> DynamicPointerCast(
        TSharedPointer<_SrcObjectType, _IsGlobal>&& _ptr
    ) noexcept;

private:
    /**
     * @brief Internal helper to move data from another shared pointer.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source shared pointer.
     */
    template<typename _SrcObjectType>
    FORCEINLINE Void MoveP(TSharedPointer<_SrcObjectType, _IsGlobal>&& _ptr) noexcept {
        obj_ptr_ = static_cast<_ObjectType*>(_ptr.obj_ptr_);
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        _ptr.obj_ptr_ = nullptr;
        _ptr.ctrl_block_ptr_ = nullptr;
    }

    /**
     * @brief Internal helper to copy data from another shared pointer.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source shared pointer.
     */
    template<typename _SrcObjectType>
    FORCEINLINE Void CopyP(const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr) noexcept {
        obj_ptr_ = static_cast<_ObjectType*>(_ptr.obj_ptr_);
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    }

    /**
     * @brief Increments the shared reference count.
     */
    FORCEINLINE Void AddShareP() noexcept {
        if (ctrl_block_ptr_ != nullptr && obj_ptr_ != nullptr) {
            ++ctrl_block_ptr_->use_count_;
        }
    }

    /**
     * @brief Decrements the shared reference count and handles deletion if zero.
     */
    FORCEINLINE Void RemoveShareP() noexcept {
        if (ctrl_block_ptr_ != nullptr && obj_ptr_ != nullptr) {
            --ctrl_block_ptr_->use_count_;
            // delete object
            if (ctrl_block_ptr_->use_count_ == 0) {
                ctrl_block_ptr_->DestroyObject();
                // delete control block
                if (ctrl_block_ptr_->weak_count_ == 0) {
                    delete ctrl_block_ptr_;
                }
            }
        }
    }

    /** @brief Pointer to the managed object. */
    _ObjectType* obj_ptr_;
    /** @brief Pointer to the control block. */
    ControlBlockType_* ctrl_block_ptr_;
};

/*
    Weak smart pointer.
*/
/**
 * @brief Weak reference smart pointer.
 * This class holds a non-owning reference to an object managed by TSharedPointer.
 * It prevents reference cycles by allowing access to the object without increasing the ownership count.
 * @tparam _ObjectType The type of the object to point to.
 * @tparam _IsGlobal Whether to use the global memory pool for allocation.
 */
template< typename _ObjectType, Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL>
class TWeakPointer {
    NO_NEW_DELETE
private:
    using ControlBlockType_ = internal::TControlBlockP<_IsGlobal>;

public:
    /**
     * @brief Default constructor. Initializes with no control block.
     */
    FORCEINLINE TWeakPointer() noexcept : ctrl_block_ptr_(nullptr) {}

    /**
     * @brief Copy constructor. Increments the weak reference count.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source weak pointer to copy.
     */
    template<typename _SrcObjectType>
    TWeakPointer(const TWeakPointer<_SrcObjectType, _IsGlobal>& _ptr) noexcept {
        CopyP();
        AddWeakP();
    }

    /**
     * @brief Move constructor. Transfers the weak reference.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source weak pointer to move.
     */
    template<typename _SrcObjectType>
    FORCEINLINE TWeakPointer(TWeakPointer<_SrcObjectType, _IsGlobal>&& _ptr) noexcept {
        MoveP();
    }

    /**
     * @brief Constructor from a shared pointer. Creates a weak reference to the shared object.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The shared pointer to observe.
     */
    template<typename _SrcObjectType>
    TWeakPointer(const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr) noexcept
        : ctrl_block_ptr_(_ptr.ctrl_block_ptr_)
    {
        AddWeakP();
    }

    /**
     * @brief Destructor. Decrements the weak reference count.
     */
    ~TWeakPointer() noexcept {
        RemoveWeakP();
    }

    /**
     * @brief Copy assignment operator.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source weak pointer to copy.
     * @return TWeakPointer& Reference to this weak pointer.
     */
    template<typename _SrcObjectType>
    TWeakPointer& operator=(const TWeakPointer<_SrcObjectType, _IsGlobal>& _ptr) noexcept {
        if (this == &_ptr) {
            return *this;
        }
        RemoveWeakP();
        CopyP(_ptr);
        AddWeakP();
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source weak pointer to move.
     * @return TWeakPointer& Reference to this weak pointer.
     */
    template<typename _SrcObjectType>
    TWeakPointer& operator=(TWeakPointer<_SrcObjectType, _IsGlobal>&& _ptr) noexcept {
        if (this == &_ptr) {
            return *this;
        }
        RemoveWeakP();
        MoveP(_ptr);
        return *this;
    }

    /**
     * @brief Assignment operator from a shared pointer.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The shared pointer to observe.
     * @return TWeakPointer& Reference to this weak pointer.
     */
    template<typename _SrcObjectType>
    TWeakPointer& operator=(const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr) noexcept {
        RemoveWeakP();
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        AddWeakP();
        return *this;
    }

    /**
     * @brief Attempts to upgrade the weak pointer to a shared pointer.
     * @return TSharedPointer<_ObjectType, _IsGlobal> A shared pointer to the object if it exists, otherwise empty.
     */
    TSharedPointer<_ObjectType, _IsGlobal> Lock() const noexcept {
        TSharedPointer<_ObjectType, _IsGlobal> ptr;
        if (!Expired()) {
            ptr.obj_ptr_ = reinterpret_cast<_ObjectType*>(ctrl_block_ptr_->obj_ptr_);
            ptr.ctrl_block_ptr_ = ctrl_block_ptr_;
            ptr.AddShareP();
        }
        return ptr;
    }

    /**
     * @brief Gets the number of shared pointers managing the object.
     * @return Int32 The use count.
     */
    NODISCARD FORCEINLINE Int32 UseCount() const noexcept { 
        return  ctrl_block_ptr_ == nullptr ? 0 : ctrl_block_ptr_->use_count_; 
    }

    /**
     * @brief Checks if the managed object has been deleted (use count is 0).
     * @return Bool True if expired or empty, otherwise false.
     */
    NODISCARD FORCEINLINE Bool Expired() const noexcept { 
        return ctrl_block_ptr_ == nullptr ? true : (ctrl_block_ptr_->use_count_ == 0);
    }

    /**
     * @brief Resets the weak pointer to empty.
     */
    Void Reset() noexcept { 
        RemoveWeakP();
        ctrl_block_ptr_ = nullptr;
    }

    /**
     * @brief Swaps contents with another TWeakPointer.
     * @param _ptr The other weak pointer to swap with.
     */
    Void Swap(TWeakPointer& _ptr) noexcept { 
        ControlBlockType_* temp_ctrl_block_ptr;
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        _ptr.ctrl_block_ptr_ = temp_ctrl_block_ptr;
    }

private:
    /**
     * @brief Internal helper to move data from another weak pointer.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source weak pointer.
     */
    template<typename _SrcObjectType>
    FORCEINLINE Void MoveP(TWeakPointer<_SrcObjectType, _IsGlobal>&& _ptr) noexcept {
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        _ptr.ctrl_block_ptr_ = nullptr;
    }

    /**
     * @brief Internal helper to copy data from another weak pointer.
     * @tparam _SrcObjectType The type of the source object.
     * @param _ptr The source weak pointer.
     */
    template<typename _SrcObjectType>
    FORCEINLINE Void CopyP(const TWeakPointer<_SrcObjectType, _IsGlobal>& _ptr) noexcept {
        ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    }

    /**
     * @brief Increments the weak reference count.
     */
    FORCEINLINE Void AddWeakP() noexcept {
        if (ctrl_block_ptr_ != nullptr) {
            ++ctrl_block_ptr_->weak_count_;
        }
    }

    /**
     * @brief Decrements the weak reference count and deletes the control block if necessary.
     */
    FORCEINLINE Void RemoveWeakP() noexcept {
        if (ctrl_block_ptr_ != nullptr) {
            // delete control block
            if (ctrl_block_ptr_->use_count_ == 0 && ctrl_block_ptr_->weak_count_ == 1) {
                delete ctrl_block_ptr_;
                ctrl_block_ptr_ = nullptr;
            }
            else {
                --ctrl_block_ptr_->weak_count_;
            }
        }
    }

    /** @brief Pointer to the control block. */
    ControlBlockType_* ctrl_block_ptr_;
};

/**
 * @brief Swaps two TUniquePointers.
 * @tparam _ObjectType The type of the object.
 * @tparam _IsGlobal Whether to use the global memory pool.
 * @param _left The first unique pointer.
 * @param _right The second unique pointer.
 */
template<typename _ObjectType, Bool _IsGlobal>
NODISCARD FORCEINLINE Void Swap(
    TUniquePointer<_ObjectType, _IsGlobal>& _left, TUniquePointer<_ObjectType, _IsGlobal>& _right
) noexcept {
    _left.Swap(_right);
}

/**
 * @brief Swaps two TSharedPointers.
 * @tparam _ObjectType The type of the object.
 * @tparam _IsGlobal Whether to use the global memory pool.
 * @param _left The first shared pointer.
 * @param _right The second shared pointer.
 */
template<typename _ObjectType, Bool _IsGlobal>
NODISCARD FORCEINLINE Void Swap(
    TSharedPointer<_ObjectType, _IsGlobal>& _left, TSharedPointer<_ObjectType, _IsGlobal>& _right
) noexcept {
    _left.Swap(_right);
}

/**
 * @brief Swaps two TWeakPointers.
 * @tparam _ObjectType The type of the object.
 * @tparam _IsGlobal Whether to use the global memory pool.
 * @param _left The first weak pointer.
 * @param _right The second weak pointer.
 */
template<typename _ObjectType, Bool _IsGlobal>
NODISCARD FORCEINLINE Void Swap(
    TWeakPointer<_ObjectType, _IsGlobal>& _left, TWeakPointer<_ObjectType, _IsGlobal>& _right
) noexcept {
    _left.Swap(_right);
}

/**
 * @brief Creates a TUniquePointer by constructing an object with the given arguments.
 * @tparam _ObjectType The type of the object to create.
 * @tparam _IsGlobal Whether to use the global memory pool.
 * @tparam _ArgsType The types of the arguments for the constructor.
 * @param _args The arguments forwarded to the object constructor.
 * @return TUniquePointer<_ObjectType, _IsGlobal> The created unique pointer.
 */
template<typename _ObjectType, Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL, typename... _ArgsType>
NODISCARD FORCEINLINE TUniquePointer<_ObjectType, _IsGlobal> MakeUnique(_ArgsType&&... _args) noexcept {
    if constexpr (_IsGlobal) {
        return TUniquePointer<_ObjectType, _IsGlobal>(new _ObjectType(std::forward<_ArgsType>(_args)...));
    }
    else {
        return TUniquePointer<_ObjectType, _IsGlobal>(NEW_LOCAL _ObjectType(std::forward<_ArgsType>(_args)...));
    }
}

/**
 * @brief Creates a TSharedPointer by constructing an object with the given arguments.
 * @tparam _ObjectType The type of the object to create.
 * @tparam _IsGlobal Whether to use the global memory pool.
 * @tparam _ArgsType The types of the arguments for the constructor.
 * @param _args The arguments forwarded to the object constructor.
 * @return TSharedPointer<_ObjectType, _IsGlobal> The created shared pointer.
 */
template<typename _ObjectType, Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL, typename... _ArgsType>
NODISCARD FORCEINLINE TSharedPointer<_ObjectType, _IsGlobal> MakeShared(_ArgsType&&... _args) noexcept {
    if constexpr (_IsGlobal) {
        return TSharedPointer<_ObjectType, _IsGlobal>(new _ObjectType(std::forward<_ArgsType>(_args)...));
    }
    else {
        return TSharedPointer<_ObjectType, _IsGlobal>(NEW_LOCAL _ObjectType(std::forward<_ArgsType>(_args)...));
    }
}

/**
 * @brief Performs a static cast on a TSharedPointer (copy version).
 * @tparam _ResultObjectType The target type of the cast.
 * @tparam _SrcObjectType The source type of the pointer.
 * @tparam _IsGlobal Whether to use the global memory pool.
 * @param _ptr The source shared pointer.
 * @return TSharedPointer<_ResultObjectType, _IsGlobal> The casted shared pointer.
 */
template<typename _ResultObjectType, typename _SrcObjectType, Bool _IsGlobal>
NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> StaticPointerCast(
    const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType, _IsGlobal> ptr;
    ptr.obj_ptr_ = (static_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    ptr.AddShareP();
    return ptr;
}

/**
 * @brief Performs a static cast on a TSharedPointer (move version).
 * @tparam _ResultObjectType The target type of the cast.
 * @tparam _SrcObjectType The source type of the pointer.
 * @tparam _IsGlobal Whether to use the global memory pool.
 * @param _ptr The source shared pointer.
 * @return TSharedPointer<_ResultObjectType, _IsGlobal> The casted shared pointer.
 */
template<typename _ResultObjectType, typename _SrcObjectType, Bool _IsGlobal>
NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> StaticPointerCast(
    TSharedPointer<_SrcObjectType, _IsGlobal>&& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType, _IsGlobal> ptr;
    ptr.obj_ptr_ = (static_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    _ptr.obj_ptr_ = nullptr;
    _ptr.ctrl_block_ptr_ = nullptr;
    return ptr;
}

/**
 * @brief Performs a const cast on a TSharedPointer (copy version).
 * @tparam _ResultObjectType The target type of the cast.
 * @tparam _SrcObjectType The source type of the pointer.
 * @tparam _IsGlobal Whether to use the global memory pool.
 * @param _ptr The source shared pointer.
 * @return TSharedPointer<_ResultObjectType, _IsGlobal> The casted shared pointer.
 */
template<typename _ResultObjectType, typename _SrcObjectType, Bool _IsGlobal>
NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> ConstPointerCast(
    const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType, _IsGlobal> ptr;
    ptr.obj_ptr_ = (const_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    ptr.AddShareP();
    return ptr;
}

/**
 * @brief Performs a const cast on a TSharedPointer (move version).
 * @tparam _ResultObjectType The target type of the cast.
 * @tparam _SrcObjectType The source type of the pointer.
 * @tparam _IsGlobal Whether to use the global memory pool.
 * @param _ptr The source shared pointer.
 * @return TSharedPointer<_ResultObjectType, _IsGlobal> The casted shared pointer.
 */
template<typename _ResultObjectType, typename _SrcObjectType, Bool _IsGlobal>
NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> ConstPointerCast(
    TSharedPointer<_SrcObjectType, _IsGlobal>&& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType, _IsGlobal> ptr;
    ptr.obj_ptr_ = (const_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    _ptr.obj_ptr_ = nullptr;
    _ptr.ctrl_block_ptr_ = nullptr;
    return ptr;
}

/**
 * @brief Performs a reinterpret cast on a TSharedPointer (copy version).
 * @tparam _ResultObjectType The target type of the cast.
 * @tparam _SrcObjectType The source type of the pointer.
 * @tparam _IsGlobal Whether to use the global memory pool.
 * @param _ptr The source shared pointer.
 * @return TSharedPointer<_ResultObjectType, _IsGlobal> The casted shared pointer.
 */
template<typename _ResultObjectType, typename _SrcObjectType, Bool _IsGlobal>
NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> ReinterpretPointerCast(
    const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType, _IsGlobal> ptr;
    ptr.obj_ptr_ = (reinterpret_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    ptr.AddShareP();
    return ptr;
}

/**
 * @brief Performs a reinterpret cast on a TSharedPointer (move version).
 * @tparam _ResultObjectType The target type of the cast.
 * @tparam _SrcObjectType The source type of the pointer.
 * @tparam _IsGlobal Whether to use the global memory pool.
 * @param _ptr The source shared pointer.
 * @return TSharedPointer<_ResultObjectType, _IsGlobal> The casted shared pointer.
 */
template<typename _ResultObjectType, typename _SrcObjectType, Bool _IsGlobal>
NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> ReinterpretPointerCast(
    TSharedPointer<_SrcObjectType, _IsGlobal>&& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType, _IsGlobal> ptr;
    ptr.obj_ptr_ = (reinterpret_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
    _ptr.obj_ptr_ = nullptr;
    _ptr.ctrl_block_ptr_ = nullptr;
    return ptr;
}

/**
 * @brief Performs a dynamic cast on a TSharedPointer (copy version).
 * @tparam _ResultObjectType The target type of the cast.
 * @tparam _SrcObjectType The source type of the pointer.
 * @tparam _IsGlobal Whether to use the global memory pool.
 * @param _ptr The source shared pointer.
 * @return TSharedPointer<_ResultObjectType, _IsGlobal> The casted shared pointer, or empty if cast fails.
 */
template<typename _ResultObjectType, typename _SrcObjectType, Bool _IsGlobal>
NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> DynamicPointerCast(
    const TSharedPointer<_SrcObjectType, _IsGlobal>& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType, _IsGlobal> ptr;
    ptr.obj_ptr_ = (dynamic_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    if (ptr.obj_ptr_ != nullptr) {
        ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        ptr.AddShareP();
    }
    return ptr;
}

/**
 * @brief Performs a dynamic cast on a TSharedPointer (move version).
 * @tparam _ResultObjectType The target type of the cast.
 * @tparam _SrcObjectType The source type of the pointer.
 * @tparam _IsGlobal Whether to use the global memory pool.
 * @param _ptr The source shared pointer.
 * @return TSharedPointer<_ResultObjectType, _IsGlobal> The casted shared pointer, or empty if cast fails.
 */
template<typename _ResultObjectType, typename _SrcObjectType, Bool _IsGlobal>
NODISCARD TSharedPointer<_ResultObjectType, _IsGlobal> DynamicPointerCast(
    TSharedPointer<_SrcObjectType, _IsGlobal>&& _ptr
) noexcept {
    TSharedPointer<_ResultObjectType, _IsGlobal> ptr;
    ptr.obj_ptr_ = (dynamic_cast<_ResultObjectType*>(_ptr.obj_ptr_));
    if (ptr.obj_ptr_ != nullptr) {
        ptr.ctrl_block_ptr_ = _ptr.ctrl_block_ptr_;
        _ptr.obj_ptr_ = nullptr;
        _ptr.ctrl_block_ptr_ = nullptr;
    }
    return ptr;
}

} // zengine