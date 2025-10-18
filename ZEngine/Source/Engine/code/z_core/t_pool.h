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

#include "t_list.h"
#include "t_lock_guard.h"
#include "z_object.h"

namespace zengine {

/*
    Pool container. When all objects applied, will auto extend the pool.
    Object's contructor will be called when the pool entends and destructor will be called when the pool destructs.
    Applying object and releasing object is independent, can be used in diffent threads.

    WARNING: Before destructor function called, all objects must be released. Unreleased object's destructor will not
    be called, might cause memory leak.
*/
template<typename _ObjectType, Bool kIfCallConstructorAndDestructor = kIsClass<_ObjectType>>
class TPool : public ZObject {
private:
    static constexpr IndexType kDefaultPoolSize = 10;
    static constexpr IndexType kMemoryPtrListSize = 10;
    static constexpr Float32 kAutoExtendMultFactor = 0.2f;

    using MemoryPtrList_ = TList<Void*>;

public:

    TPool() noexcept
        : SuperType_()
        , mem_ptr_list_(kMemoryPtrListSize)
        , head_node_ptr_(nullptr)
        , end_node_ptr_(nullptr)
        , model_obj_ptr_(nullptr)
        , total_num_(0)
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
        , total_num_(0)
    {
        if constexpr (kIfCallConstructorAndDestructor) {
            model_obj_ptr_ = new _ObjectType(std::forward<_ArgsType>(args)...);
        }
        else {
            model_obj_ptr_ = nullptr;
        }
    }

    ~TPool() noexcept {
        if constexpr (kIfCallConstructorAndDestructor) {
            if constexpr (kIsClass<_ObjectType>) {
                Node_* node_ptr = head_node_ptr_;
                while (node_ptr != end_node_ptr_) {
                    node_ptr->object_.~_ObjectType();
                    node_ptr = node_ptr->next_node_ptr_;
                }
            }
            if (model_obj_ptr_ != nullptr) {
                delete model_obj_ptr_;
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

    template<typename... _ArgsType>
    NODISCARD Void SetModel(_ArgsType&&... args) noexcept {
        if constexpr (kIfCallConstructorAndDestructor) {
            if (model_obj_ptr_ != nullptr) {
                delete model_obj_ptr_;
            }
            model_obj_ptr_ = new _ObjectType(std::forward<_ArgsType>(args)...);
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
    Void ExtendPool(IndexType _size = 0) noexcept {
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
    Void ApplyNewObjectsP(Int32 _num = 0) noexcept {
        Int32 extend_num = _num == 0 ? Int32(total_num_ * kAutoExtendMultFactor) : (total_num_ - _num);
        if (extend_num < kDefaultPoolSize) {
            extend_num = kDefaultPoolSize;
        }
        MemoryType mem_size = sizeof(Node_) * extend_num;
        Void* mem_ptr = memory_pool::ApplyMemory(mem_size, &mem_size);
        extend_num = mem_size / sizeof(Node_);
        if (model_obj_ptr_ != nullptr) {
            for (IndexType index = 0; index < extend_num; ++index) {
                Node_* node_ptr = reinterpret_cast<Node_*>(mem_ptr) + index;
                if constexpr (kIfCallConstructorAndDestructor) {
                    if constexpr (kIsClass<_ObjectType>) {
                        new(reinterpret_cast<Void*>(node_ptr)) _ObjectType(*model_obj_ptr_);
                    }
                    else {
                        node_ptr->object_ = *model_obj_ptr_;
                    }
                }
                node_ptr->next_node_ptr_ = node_ptr + 1;
            }
        }
        else {
            for (IndexType index = 0; index < extend_num; ++index) {
                Node_* node_ptr = reinterpret_cast<Node_*>(mem_ptr) + index;
                if constexpr (kIfCallConstructorAndDestructor) {
                    if constexpr (kIsClass<_ObjectType>) {
                        new(reinterpret_cast<Void*>(node_ptr)) _ObjectType();
                    }
                }
                node_ptr->next_node_ptr_ = node_ptr + 1;
            }
        }
        if (total_num_ == 0) {
            end_node_ptr_ = reinterpret_cast<Node_*>(mem_ptr) + extend_num - 1;
        }
        else {
            (reinterpret_cast<Node_*>(mem_ptr) + extend_num - 1)->next_node_ptr_ = head_node_ptr_;
        }
        head_node_ptr_ = reinterpret_cast<Node_*>(mem_ptr);
        total_num_ = total_num_ + extend_num;
    }

    Void MoveP(TPool&& _pool) noexcept {
        mem_ptr_list_ = std::move(_pool.mem_ptr_list_);
        head_node_ptr_ = _pool.head_node_ptr_;
        end_node_ptr_ = _pool.end_node_ptr_;
        model_obj_ptr_ = _pool.model_obj_ptr_;
        total_num_ = _pool.total_num_;
        _pool.head_node_ptr_ = nullptr;
        _pool.end_node_ptr_ = nullptr;
        _pool.model_obj_ptr_ = nullptr;
        _pool.total_num_ = 0;
    }

    MemoryPtrList_ mem_ptr_list_;
    Node_* head_node_ptr_;
    Node_* end_node_ptr_;
    _ObjectType* model_obj_ptr_;
    IndexType total_num_;
};

/*
    Pool container. Thread safe. When all objects applied, will auto extend the pool.
    Object's contructor will be called when the pool entends and destructor will be called when the pool destructs.
    Applying object and releasing object is independent, can be used in diffent threads.

    WARNING: Before destructor function called, all objects must be released. Unreleased object's destructor will not
    be called, might cause memory leak.
*/
template<typename _ObjectType, Bool kIfCallConstructorAndDestructor = kIsClass<_ObjectType>>
class TPoolSafe : public ZObject {
private:
    static constexpr IndexType kDefaultPoolSize = 10;
    static constexpr IndexType kMemoryPtrListSize = 10;
    static constexpr Float32 kAutoExtendMultFactor = 0.2f;

    using MemoryPtrList_ = TList<Void*>;

public:

    TPoolSafe() noexcept
        : SuperType_()
        , mem_ptr_list_(kMemoryPtrListSize)
        , head_node_ptr_(nullptr)
        , end_node_ptr_(nullptr)
        , model_obj_ptr_(nullptr)
        , mutex_()
        , total_num_(0)
    {}
    TPoolSafe(TPoolSafe&& _pool) noexcept
        : SuperType_()
    {
        MoveP(std::forward<TPoolSafe>(_pool));
    }
    template<typename... _ArgsType>
    TPoolSafe(_ArgsType&&... args) noexcept
        : SuperType_()
        , mem_ptr_list_(kMemoryPtrListSize)
        , head_node_ptr_(nullptr)
        , end_node_ptr_(nullptr)
        , mutex_()
        , total_num_(0)
    {
        if constexpr (kIfCallConstructorAndDestructor) {
            model_obj_ptr_ = new _ObjectType(std::forward<_ArgsType>(args)...);
        }
        else {
            model_obj_ptr_ = nullptr;
        }
    }

    ~TPoolSafe() noexcept {
        if constexpr (kIfCallConstructorAndDestructor) {
            if constexpr (kIsClass<_ObjectType>) {
                Node_* node_ptr = head_node_ptr_;
                while (node_ptr != end_node_ptr_) {
                    node_ptr->object_.~_ObjectType();
                    node_ptr = node_ptr->next_node_ptr_;
                }
            }
            if (model_obj_ptr_ != nullptr) {
                delete model_obj_ptr_;
            }
        }
        for (auto mem_ptr = mem_ptr_list_.Begin(); mem_ptr != mem_ptr_list_.End(); ++mem_ptr) {
            memory_pool::ReleaseMemory(*mem_ptr);
        }
    }

    TPoolSafe& operator=(TPoolSafe&& _pool) noexcept {
        TLockGuard lock_guard(mutex_);
        MoveP(std::forward<TPoolSafe>(_pool));
        return *this;
    }

    template<typename... _ArgsType>
    NODISCARD Void SetModel(_ArgsType&&... args) noexcept {
        TLockGuard lock_guard(mutex_);
        if constexpr (kIfCallConstructorAndDestructor) {
            if (model_obj_ptr_ != nullptr) {
                delete model_obj_ptr_;
            }
            model_obj_ptr_ = new _ObjectType(std::forward<_ArgsType>(args)...);
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
    Void ExtendPool(IndexType _size = 0) noexcept {
        TLockGuard lock_guard(mutex_);
        ApplyNewObjectsP(_size);
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
    Void ApplyNewObjectsP(Int32 _num = 0) noexcept {
        Int32 extend_num = _num == 0 ? Int32(total_num_ * kAutoExtendMultFactor) : (total_num_ - _num);
        if (extend_num < kDefaultPoolSize) {
            extend_num = kDefaultPoolSize;
        }
        MemoryType mem_size = sizeof(Node_) * extend_num;
        Void* mem_ptr = memory_pool::ApplyMemory(mem_size, &mem_size);
        extend_num = mem_size / sizeof(Node_);
        if (model_obj_ptr_ != nullptr) {
            for (IndexType index = 0; index < extend_num; ++index) {
                Node_* node_ptr = reinterpret_cast<Node_*>(mem_ptr) + index;
                if constexpr (kIfCallConstructorAndDestructor) {
                    if constexpr (kIsClass<_ObjectType>) {
                        new(reinterpret_cast<Void*>(node_ptr)) _ObjectType(*model_obj_ptr_);
                    }
                    else {
                        node_ptr->object_ = *model_obj_ptr_;
                    }
                }
                node_ptr->next_node_ptr_ = node_ptr + 1;
            }
        }
        else {
            for (IndexType index = 0; index < extend_num; ++index) {
                Node_* node_ptr = reinterpret_cast<Node_*>(mem_ptr) + index;
                if constexpr (kIfCallConstructorAndDestructor) {
                    if constexpr (kIsClass<_ObjectType>) {
                        new(reinterpret_cast<Void*>(node_ptr)) _ObjectType();
                    }
                }
                node_ptr->next_node_ptr_ = node_ptr + 1;
            }
        }
        if (total_num_ == 0) {
            end_node_ptr_ = reinterpret_cast<Node_*>(mem_ptr) + extend_num - 1;
        }
        else {
            (reinterpret_cast<Node_*>(mem_ptr) + extend_num - 1)->next_node_ptr_ = head_node_ptr_;
        }
        head_node_ptr_ = reinterpret_cast<Node_*>(mem_ptr);
        total_num_ = total_num_ + extend_num;
    }

    Void MoveP(TPoolSafe&& _pool) noexcept {
        mem_ptr_list_ = std::move(_pool.mem_ptr_list_);
        mutex_ = std::move(_pool.mutex_);
        head_node_ptr_ = _pool.head_node_ptr_;
        end_node_ptr_ = _pool.end_node_ptr_;
        model_obj_ptr_ = _pool.model_obj_ptr_;
        total_num_ = _pool.total_num_;
        _pool.head_node_ptr_ = nullptr;
        _pool.end_node_ptr_ = nullptr;
        _pool.model_obj_ptr_ = nullptr;
        _pool.total_num_ = 0;
    }

    MemoryPtrList_ mem_ptr_list_;
    Node_* head_node_ptr_;
    Node_* end_node_ptr_;
    _ObjectType* model_obj_ptr_;
    ZMutex mutex_;
    IndexType total_num_;
};

}//zengine