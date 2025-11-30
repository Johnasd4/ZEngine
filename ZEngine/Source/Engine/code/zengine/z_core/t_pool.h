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

#include "t_list.h"
#include "t_lock_guard.h"
#include "t_smart_pointer.h"
#include "z_mutex.h"
#include "z_object.h"

namespace zengine {

/*
    Pool container. When all objects applied, will auto extend the pool.
    Object's contructor will be called when the pool entends and destructor will be called when the pool destructs.
    Applying object and releasing object is independent, can be used in diffent threads.

    WARNING: Before destructor function called, all objects must be released. Unreleased object's destructor will not
    be called, might cause memory leak.
*/
template<typename _ObjectType, Bool kIfCallConstructorAndDestructor = kIsClassType<_ObjectType>>
class TPool : public ZObject {
private:
    static inline constexpr SizeType kDefaultPoolSize = 10ULL;
    static inline constexpr SizeType kMemoryPtrListSize = 10ULL;
    static inline constexpr Float32 kAutoExtendMultFactor = 0.2f;

    using MemoryPtrList_ = TList<Void*>;

public:

    TPool() noexcept
        : SuperType_()
        , mem_ptr_list_(kMemoryPtrListSize)
        , head_node_ptr_(nullptr)
        , end_node_ptr_(nullptr)
        , model_obj_ptr_()
        , pool_size_(0ULL)
    {}
    TPool(TPool&& _pool) noexcept
        : SuperType_()
    {
        MoveP(std::forward<TPool>(_pool));
    }
    template<typename... _ArgsType>
    TPool(_ArgsType&&... args) noexcept
        : SuperType_()
        , mem_ptr_list_(kMemoryPtrListSize)
        , head_node_ptr_(nullptr)
        , end_node_ptr_(nullptr)
        , model_obj_ptr_()
        , pool_size_(0ULL)
    {
        if constexpr (kIfCallConstructorAndDestructor) {
            model_obj_ptr_ = MakeUnique<_ObjectType>(std::forward<_ArgsType>(args)...);
        }
    }

    ~TPool() noexcept {
        if constexpr (kIfCallConstructorAndDestructor) {
            if constexpr (kIsClassType<_ObjectType>) {
                Node_* node_ptr = head_node_ptr_;
                while (node_ptr != end_node_ptr_) {
                    node_ptr->object_.~_ObjectType();
                    node_ptr = node_ptr->next_node_ptr_;
                }
            }
        }
        for (auto mem_ptr = mem_ptr_list_.Begin(); mem_ptr != mem_ptr_list_.End(); ++mem_ptr) {
            memory_pool::ReleaseMemory(*mem_ptr);
        }
    }

    TPool& operator=(TPool&& _pool) noexcept {
        MoveP(std::forward<TPool>(_pool));
        return *this;
    }

    NODISCARD FORCEINLINE SizeType PoolSize() noexcept {
        return pool_size_;
    }

    template<typename... _ArgsType>
    NODISCARD Void SetModel(_ArgsType&&... args) noexcept {
        if constexpr (kIfCallConstructorAndDestructor) {
            model_obj_ptr_ = MakeUnique<_ObjectType>(std::forward<_ArgsType>(args)...);
        }
    }

    NODISCARD _ObjectType* Apply() noexcept {
        //when pool object num = 1
        if (head_node_ptr_ == end_node_ptr_) {
            ApplyNewObjectsP();
        }
        _ObjectType* object_ptr = &head_node_ptr_->object_;
        head_node_ptr_ = head_node_ptr_->next_node_ptr_;
        return object_ptr;
    }
    NODISCARD Void Release(_ObjectType* _object_ptr) noexcept {
        Node_* release_node_ptr = reinterpret_cast<Node_*>(_object_ptr);
        end_node_ptr_->next_node_ptr_ = release_node_ptr;
        end_node_ptr_ = release_node_ptr;
    }

    /*
        Extends the pool to a size >= given size. If _size = 0 then auto calculates the extending size.
    */
    Void ExtendPool(SizeType _size = 0ULL) noexcept {
        ApplyNewObjectsP(_size);
    }

protected:
    using SuperType_ = ZObject;

private:
    struct Node_ {
        _ObjectType object_;
        Node_* next_node_ptr_;
    };

    TPool(const TPool&) = delete;
    TPool& operator=(const TPool&) = delete;

    /*
        Apply new ojects, add to the head node.
    */
    Void ApplyNewObjectsP(SizeType _num = 0ULL) noexcept {
        SizeType extend_num;
        if(_num == 0ULL || _num <= pool_size_) {
            extend_num = SizeType(pool_size_ * kAutoExtendMultFactor);
        }
        else {
            extend_num = _num - pool_size_;
        }
        if (extend_num < kDefaultPoolSize) {
            extend_num = kDefaultPoolSize;
        }
        SizeType mem_size = sizeof(Node_) * extend_num;
        Void* mem_ptr = memory_pool::ApplyMemory(mem_size, &mem_size);
        mem_ptr_list_.PushBack(mem_ptr);
        extend_num = mem_size / sizeof(Node_);
        if (model_obj_ptr_) {
            for (SizeType index = 0ULL; index < extend_num; ++index) {
                Node_* node_ptr = reinterpret_cast<Node_*>(mem_ptr) + index;
                if constexpr (requires{ new(reinterpret_cast<Void*>(node_ptr)) _ObjectType(*model_obj_ptr_); }) {
                    if constexpr (kIfCallConstructorAndDestructor) {
                        if constexpr (kIsClassType<_ObjectType>) {
                            new(reinterpret_cast<Void*>(node_ptr)) _ObjectType(*model_obj_ptr_);
                        }
                        else {
                            node_ptr->object_ = *model_obj_ptr_;
                        }
                    }
                }
                node_ptr->next_node_ptr_ = node_ptr + 1ULL;
            }
        }
        else {
            for (SizeType index = 0; index < extend_num; ++index) {
                Node_* node_ptr = reinterpret_cast<Node_*>(mem_ptr) + index;
                if constexpr (kIfCallConstructorAndDestructor) {
                    if constexpr (kIsClassType<_ObjectType>) {
                        new(reinterpret_cast<Void*>(node_ptr)) _ObjectType();
                    }
                }
                node_ptr->next_node_ptr_ = node_ptr + 1ULL;
            }
        }
        if (pool_size_ == 0ULL) {
            end_node_ptr_ = reinterpret_cast<Node_*>(mem_ptr) + extend_num - 1ULL;
        }
        else {
            (reinterpret_cast<Node_*>(mem_ptr) + extend_num - 1ULL)->next_node_ptr_ = head_node_ptr_;
        }
        head_node_ptr_ = reinterpret_cast<Node_*>(mem_ptr);
        pool_size_ = pool_size_ + extend_num;
    }

    Void MoveP(TPool&& _pool) noexcept {
        mem_ptr_list_ = std::move(_pool.mem_ptr_list_);
        model_obj_ptr_ = std::move(_pool.model_obj_ptr_);
        head_node_ptr_ = _pool.head_node_ptr_;
        end_node_ptr_ = _pool.end_node_ptr_;
        pool_size_ = _pool.pool_size_;
        _pool.head_node_ptr_ = nullptr;
        _pool.end_node_ptr_ = nullptr;
        _pool.pool_size_ = 0ULL;
    }

    MemoryPtrList_ mem_ptr_list_;
    Node_* head_node_ptr_;
    Node_* end_node_ptr_;
    TUniquePointer<_ObjectType> model_obj_ptr_;
    SizeType pool_size_;
};

/*
    Pool container. Thread safe. When all objects applied, will auto extend the pool.
    Object's contructor will be called when the pool entends and destructor will be called when the pool destructs.
    Applying object and releasing object is independent, can be used in diffent threads.

    WARNING: Before destructor function called, all objects must be released. Unreleased object's destructor will not
    be called, might cause memory leak.
*/
template<typename _ObjectType, Bool kIfCallConstructorAndDestructor = kIsClassType<_ObjectType>>
class TPoolSafe : public ZObject {
private:
    static constexpr inline SizeType kDefaultPoolSize = 10ULL;
    static constexpr inline SizeType kMemoryPtrListSize = 10ULL;
    static constexpr inline Float32 kAutoExtendMultFactor = 0.2f;

    using MemoryPtrList_ = TList<Void*>;

public:
    TPoolSafe() noexcept
        : SuperType_()
        , mem_ptr_list_(kMemoryPtrListSize)
        , head_node_ptr_(nullptr)
        , end_node_ptr_(nullptr)
        , model_obj_ptr_()
        , mutex_()
        , pool_size_(0)
    {}
    TPoolSafe(TPoolSafe&& _pool) noexcept
        : SuperType_()
    {
        MoveP(std::forward<TPoolSafe>(_pool));
        mutex_ = std::move(_pool.mutex_);
    }
    template<typename... _ArgsType>
    TPoolSafe(_ArgsType&&... args) noexcept
        : SuperType_()
        , mem_ptr_list_(kMemoryPtrListSize)
        , head_node_ptr_(nullptr)
        , end_node_ptr_(nullptr)
        , model_obj_ptr_()
        , mutex_()
        , pool_size_(0ULL)
    {
        if constexpr (kIfCallConstructorAndDestructor) {
            model_obj_ptr_ = MakeUnique<_ObjectType>(std::forward<_ArgsType>(args)...);
        }
    }

    ~TPoolSafe() noexcept {
        if constexpr (kIfCallConstructorAndDestructor) {
            if constexpr (kIsClassType<_ObjectType>) {
                Node_* node_ptr = head_node_ptr_;
                while (node_ptr != end_node_ptr_) {
                    node_ptr->object_.~_ObjectType();
                    node_ptr = node_ptr->next_node_ptr_;
                }
            }
        }
        for (auto mem_ptr = mem_ptr_list_.Begin(); mem_ptr != mem_ptr_list_.End(); ++mem_ptr) {
            memory_pool::ReleaseMemory(*mem_ptr);
        }
    }

    /*
        Not thread safe.
    */
    TPoolSafe& operator=(TPoolSafe&& _pool) noexcept {
        TLockGuard lock_guard(mutex_);
        TLockGuard lock_guard(_pool.mutex_);
        MoveP(std::forward<TPoolSafe>(_pool));
        return *this;
    }

    NODISCARD FORCEINLINE SizeType PoolSize() noexcept {
        TLockGuard lock_guard(mutex_);
        return pool_size_;
    }

    template<typename... _ArgsType>
    NODISCARD Void SetModel(_ArgsType&&... args) noexcept {
        TLockGuard lock_guard(mutex_);
        if constexpr (kIfCallConstructorAndDestructor) {
            model_obj_ptr_ = MakeUnique<_ObjectType>(std::forward<_ArgsType>(args)...);
        }
    }

    NODISCARD _ObjectType* Apply() noexcept {
        TLockGuard lock_guard(mutex_);
        //when pool object num = 1
        if (head_node_ptr_ == end_node_ptr_) {
            ApplyNewObjectsP();
        }
        _ObjectType* object_ptr = &head_node_ptr_->object_;
        head_node_ptr_ = head_node_ptr_->next_node_ptr_;
        return object_ptr;
    }
    NODISCARD Void Release(_ObjectType* _object_ptr) noexcept {
        TLockGuard lock_guard(mutex_);
        Node_* release_node_ptr = reinterpret_cast<Node_*>(_object_ptr);
        end_node_ptr_->next_node_ptr_ = release_node_ptr;
        end_node_ptr_ = release_node_ptr;
    }

    /*
        Extends the pool to a size >= given size. If _size = 0 then auto calculates the extending size.
    */
    Void ExtendPool(SizeType _size = 0ULL) noexcept {
        TLockGuard lock_guard(mutex_);
        ApplyNewObjectsP(_size);
    }

    /*
        Lock the container mutex. Used when operating iterators.
    */
    FORCEINLINE Void Lock() noexcept {
        mutex_.Lock();
    }

    /*
        Unlock the container mutex. Used when operating iterators.
    */
    FORCEINLINE Void Unlock() noexcept {
        mutex_.Unlock();
    }

protected:
    using SuperType_ = ZObject;

private:
    struct Node_ {
        _ObjectType object_;
        Node_* next_node_ptr_;
    };

    TPoolSafe(const TPoolSafe&) = delete;
    TPoolSafe& operator=(const TPoolSafe&) = delete;

    /*
        Apply new ojects, add to the head node.
    */
    Void ApplyNewObjectsP(SizeType _num = 0ULL) noexcept {
        SizeType extend_num;
        if (_num == 0ULL || _num <= pool_size_) {
            extend_num = SizeType(pool_size_ * kAutoExtendMultFactor);
        }
        else {
            extend_num = _num - pool_size_;
        }
        SizeType mem_size = sizeof(Node_) * extend_num;
        Void* mem_ptr = memory_pool::ApplyMemory(mem_size, &mem_size);
        mem_ptr_list_.PushBack(mem_ptr);
        extend_num = mem_size / sizeof(Node_);
        if (model_obj_ptr_) {
            for (SizeType index = 0; index < extend_num; ++index) {
                Node_* node_ptr = reinterpret_cast<Node_*>(mem_ptr) + index;
                if constexpr (requires{ new(reinterpret_cast<Void*>(node_ptr)) _ObjectType(*model_obj_ptr_); }) {
                    if constexpr (kIfCallConstructorAndDestructor) {
                        if constexpr (kIsClassType<_ObjectType>) {
                            new(reinterpret_cast<Void*>(node_ptr)) _ObjectType(*model_obj_ptr_);
                        }
                        else {
                            node_ptr->object_ = *model_obj_ptr_;
                        }
                    }
                    node_ptr->next_node_ptr_ = node_ptr + 1ULL;
                }
            }
        }
        else {
            for (SizeType index = 0; index < extend_num; ++index) {
                Node_* node_ptr = reinterpret_cast<Node_*>(mem_ptr) + index;
                if constexpr (kIfCallConstructorAndDestructor) {
                    if constexpr (kIsClassType<_ObjectType>) {
                        new(reinterpret_cast<Void*>(node_ptr)) _ObjectType();
                    }
                }
                node_ptr->next_node_ptr_ = node_ptr + 1ULL;
            }
        }
        if (pool_size_ == 0ULL) {
            end_node_ptr_ = reinterpret_cast<Node_*>(mem_ptr) + extend_num - 1ULL;
        }
        else {
            (reinterpret_cast<Node_*>(mem_ptr) + extend_num - 1ULL)->next_node_ptr_ = head_node_ptr_;
        }
        head_node_ptr_ = reinterpret_cast<Node_*>(mem_ptr);
        pool_size_ = pool_size_ + extend_num;
    }

    Void MoveP(TPoolSafe&& _pool) noexcept {
        mem_ptr_list_ = std::move(_pool.mem_ptr_list_);
        model_obj_ptr_ = std::move(_pool.model_obj_ptr_);
        head_node_ptr_ = _pool.head_node_ptr_;
        end_node_ptr_ = _pool.end_node_ptr_;
        pool_size_ = _pool.pool_size_;
        _pool.head_node_ptr_ = nullptr;
        _pool.end_node_ptr_ = nullptr;
        _pool.pool_size_ = 0ULL;
    }

    MemoryPtrList_ mem_ptr_list_;
    Node_* head_node_ptr_;
    Node_* end_node_ptr_;
    TUniquePointer<_ObjectType> model_obj_ptr_;
    ZMutex mutex_;
    SizeType pool_size_;
};

}//zengine