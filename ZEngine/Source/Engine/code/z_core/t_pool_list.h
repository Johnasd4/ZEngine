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
    Pool list container, is both pool and list. Every object has an extra 16B size. 
    Object insert and remove's time complexity is O(1).
    All objects insert to the list must be applied from this pool. 
    When all objects applied, will auto extend the pool.
    Object's contructor will be called when the pool entends and destructor will be called when the pool destructs.
    Objects can be released directly from the list without removing.

    WARNING: Before destructor function called, all objects must be released or insert to the list. 
    Unreleased object's destructor will not be called, might cause memory leak.
*/
template<typename _ObjectType, Bool kIfCallConstructorAndDestructor = kIsClass<_ObjectType>>
class TPoolList : public ZObject {
private:
    static constexpr IndexType kDefaultPoolSize = 10;
    static constexpr IndexType kMemoryPtrListSize = 10;
    static constexpr Float32 kAutoExtendMultFactor = 0.2f;
    using MemoryPtrList_ = TList<Void*>;

public:
    class Iterator_ {
    public:
        FORCEINLINE Iterator_(TPoolList::Node_* _node_ptr) noexcept : node_ptr_(_node_ptr) {}
        FORCEINLINE Iterator_(const Iterator_& _iterator) noexcept : node_ptr_(_iterator.node_ptr_) {}
        FORCEINLINE Iterator_(Iterator_&& _iterator) noexcept : node_ptr_(_iterator.node_ptr_) {
            _iterator.node_ptr_ = nullptr;
        }

        NODISCARD FORCEINLINE Bool operator==(const TPoolList::Node_* _node_ptr) const noexcept {
            return node_ptr_ == _node_ptr;
        }
        NODISCARD FORCEINLINE Bool operator!=(const TPoolList::Node_* _node_ptr) const noexcept {
            return node_ptr_ != _node_ptr;
        }
        NODISCARD FORCEINLINE Bool operator==(const Iterator_& _iterator) const noexcept {
            return node_ptr_ == _iterator.node_ptr_;
        }
        NODISCARD FORCEINLINE Bool operator!=(const Iterator_& _iterator) const noexcept {
            return node_ptr_ != _iterator.node_ptr_;
        }
        FORCEINLINE Iterator_& operator=(const Iterator_& _iterator) noexcept {
            node_ptr_ = _iterator.node_ptr_;
            return *this;
        }
        FORCEINLINE Iterator_& operator=(Iterator_&& _iterator) noexcept {
            node_ptr_ = _iterator.node_ptr_;
            _iterator.node_ptr_ = nullptr;
            return *this;
        }
        FORCEINLINE Iterator_& operator++() noexcept {
            node_ptr_ = node_ptr_->next_node_ptr_;
            return *this;
        }
        NODISCARD FORCEINLINE _ObjectType& operator*() noexcept { return node_ptr_->object_; }
        NODISCARD FORCEINLINE const _ObjectType& operator*() const noexcept { return node_ptr_->object_; }
        NODISCARD FORCEINLINE _ObjectType* operator->() noexcept { return &node_ptr_->object_; }
        NODISCARD FORCEINLINE const _ObjectType* operator->() const noexcept { return &node_ptr_->object_; }
        NODISCARD FORCEINLINE _ObjectType* Ptr() noexcept { return &node_ptr_->object_; }
        NODISCARD FORCEINLINE const _ObjectType* Ptr() const noexcept { return &node_ptr_->object_; }

    private:
        TPoolList::Node_* node_ptr_;
    };

    TPoolList() noexcept
        : SuperType_()
        , mem_ptr_list_(kMemoryPtrListSize)
        , head_node_ptr_(nullptr)
        , end_node_ptr_(nullptr)
        , list_head_node_ptr_(nullptr)
        , model_obj_ptr_(nullptr)
        , total_num_(0)
    {}
    TPoolList(TPoolList&& _pool) noexcept
        : SuperType_()
    {
        MoveP(std::forward<TPool>(_pool));
    }
    template<typename... _ArgsType>
    TPoolList(_ArgsType&&... args) noexcept
        : SuperType_()
        , mem_ptr_list_(kMemoryPtrListSize)
        , head_node_ptr_(nullptr)
        , end_node_ptr_(nullptr)
        , list_head_node_ptr_(nullptr)
        , total_num_(0)
    {
        if constexpr (kIfCallConstructorAndDestructor) {
            model_obj_ptr_ = new _ObjectType(std::forward<_ArgsType>(args)...);
        }
        else {
            model_obj_ptr_ = nullptr;
        }
    }

    ~TPoolList() noexcept {
        if constexpr (kIfCallConstructorAndDestructor) {
            if constexpr (kIsClass<_ObjectType>) {
                Node_* node_ptr = head_node_ptr_;
                while (node_ptr != end_node_ptr_) {
                    node_ptr->object_.~_ObjectType();
                    node_ptr = node_ptr->next_node_ptr_;
                }
                node_ptr = list_head_node_ptr_;
                while (node_ptr != nullptr) {
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

    TPoolList& operator=(TPoolList&& _pool) noexcept {
        MoveP(std::forward<TPool>(_pool));
        return *this;
    }

    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return Iterator_(list_head_node_ptr_); }
    NODISCARD FORCEINLINE const Iterator_ Begin() const noexcept { return Iterator_(list_head_node_ptr_); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return Iterator_(nullptr); }
    NODISCARD FORCEINLINE const Iterator_ End() const noexcept { return Iterator_(nullptr); }

    Void Push(_ObjectType* _object_ptr) noexcept {
        Node_* node_ptr = reinterpret_cast<Node_*>(_object_ptr);
        if (head_node_ptr_ == end_node_ptr_) {
            ApplyNewObjectsP();
        }
        node_ptr->next_node_ptr_ = list_head_node_ptr_;
        node_ptr->previous_node_ptr_ = nullptr;
        if (list_head_node_ptr_ != nullptr) {
            list_head_node_ptr_->previous_node_ptr_ = node_ptr;
        }
        list_head_node_ptr_ = node_ptr;
    }

    Void Clear() noexcept { 
        end_node_ptr_->next_node_ptr_ = list_head_node_ptr_;
        while (end_node_ptr_->next_node_ptr_ != nullptr) {
            end_node_ptr_ = list_end_node_ptr_;
        }
    }

    template<typename... _ArgsType>
    Void SetModel(_ArgsType&&... args) noexcept {
        if constexpr (kIfCallConstructorAndDestructor) {
            if (model_obj_ptr_ != nullptr) {
                delete model_obj_ptr_;
            }
            model_obj_ptr_ = new _ObjectType(std::forward<_ArgsType>(args)...);
        }
    }

    NODISCARD _ObjectType* Apply() noexcept {
        if (head_node_ptr_ == end_node_ptr_) {
            ApplyNewObjectsP();
        }
        Node_* apply_node_ptr = head_node_ptr_;
        head_node_ptr_ = head_node_ptr_->next_node_ptr_;
        apply_node_ptr->next_node_ptr_ = nullptr;
        apply_node_ptr->previous_node_ptr_ = nullptr;
        return &apply_node_ptr->object_;
    }

    NODISCARD Iterator_ Release(Iterator_ _iterator) noexcept {
        Node_* release_node_ptr = reinterpret_cast<Node_*>(_iterator.Ptr());
        Iterator_  ret_iterator = ++_iterator;
        if (list_head_node_ptr_ == release_node_ptr) {
            list_head_node_ptr_ = list_head_node_ptr_->next_node_ptr_;
        }
        if (release_node_ptr->previous_node_ptr_ != nullptr) {
            release_node_ptr->previous_node_ptr_->next_node_ptr_ = release_node_ptr->next_node_ptr_;
        }
        if (release_node_ptr->next_node_ptr_ != nullptr) {
            release_node_ptr->next_node_ptr_->previous_node_ptr_ = release_node_ptr->previous_node_ptr_;
        }
        end_node_ptr_->next_node_ptr_ = release_node_ptr;
        end_node_ptr_ = release_node_ptr;
        return ret_iterator;
    }

    Void Release(_ObjectType* _object_ptr) noexcept {
        Node_* release_node_ptr = reinterpret_cast<Node_*>(_object_ptr);
        if (list_head_node_ptr_ == release_node_ptr) {
            list_head_node_ptr_ = list_head_node_ptr_->next_node_ptr_;
        }
        if (release_node_ptr->previous_node_ptr_ != nullptr) {
            release_node_ptr->previous_node_ptr_->next_node_ptr_ = release_node_ptr->next_node_ptr_;
        }
        if (release_node_ptr->next_node_ptr_ != nullptr) {
            release_node_ptr->next_node_ptr_->previous_node_ptr_ = release_node_ptr->previous_node_ptr_;
        }
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
    friend class TPoolList<_ObjectType>::Iterator_;

private:
    struct Node_ {
        _ObjectType object_;
        Node_* next_node_ptr_;
        Node_* previous_node_ptr_;
    };

    TPoolList(const TPoolList&) = delete;
    TPoolList& operator=(const TPoolList&) = delete;

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

    Void MoveP(TPoolList&& _pool) noexcept {
        mem_ptr_list_ = std::move(_pool.mem_ptr_list_);
        head_node_ptr_ = _pool.head_node_ptr_;
        end_node_ptr_ = _pool.end_node_ptr_;
        list_head_node_ptr_ = _pool.list_head_node_ptr_;
        model_obj_ptr_ = _pool.model_obj_ptr_;
        total_num_ = _pool.total_num_;
        _pool.head_node_ptr_ = nullptr;
        _pool.end_node_ptr_ = nullptr;
        _pool.list_head_node_ptr_ = nullptr;
        _pool.model_obj_ptr_ = nullptr;
        _pool.total_num_ = 0;
    }

    MemoryPtrList_ mem_ptr_list_;
    Node_* head_node_ptr_;
    Node_* end_node_ptr_;
    Node_* list_head_node_ptr_;
    Node_* list_end_node_ptr_;
    _ObjectType* model_obj_ptr_;
    IndexType total_num_;
};

/*
    Pool list container, is both pool and list, thread safe. Every object has an extra 16B size. 
    Object insert and remove's time complexity is O(1).
    All objects insert to the list must be applied from this pool. 
    When all objects applied, will auto extend the pool.
    Object's contructor will be called when the pool entends and destructor will be called when the pool destructs.
    Applying object and releasing object is independent, can be used in diffent threads.
    Objects can be released directly from the list without removing.

    WARNING: Before destructor function called, all objects must be released or insert to the list. 
    Unreleased object's destructor will not be called, might cause memory leak.
*/
template<typename _ObjectType, Bool kIfCallConstructorAndDestructor = kIsClass<_ObjectType>>
class TPoolListSafe : public ZObject {
private:
    static constexpr IndexType kDefaultPoolSize = 10;
    static constexpr IndexType kMemoryPtrListSize = 10;
    static constexpr Float32 kAutoExtendMultFactor = 0.2f;
    using MemoryPtrList_ = TList<Void*>;

public:
    class Iterator_ {
    public:
        FORCEINLINE Iterator_(TPoolListSafe::Node_* _node_ptr) noexcept : node_ptr_(_node_ptr) {}
        FORCEINLINE Iterator_(const Iterator_& _iterator) noexcept : node_ptr_(_iterator.node_ptr_) {}
        FORCEINLINE Iterator_(Iterator_&& _iterator) noexcept : node_ptr_(_iterator.node_ptr_) {
            _iterator.node_ptr_ = nullptr;
        }

        NODISCARD FORCEINLINE Bool operator==(const TPoolListSafe::Node_* _node_ptr) const noexcept {
            return node_ptr_ == _node_ptr;
        }
        NODISCARD FORCEINLINE Bool operator!=(const TPoolListSafe::Node_* _node_ptr) const noexcept {
            return node_ptr_ != _node_ptr;
        }
        NODISCARD FORCEINLINE Bool operator==(const Iterator_& _iterator) const noexcept {
            return node_ptr_ == _iterator.node_ptr_;
        }
        NODISCARD FORCEINLINE Bool operator!=(const Iterator_& _iterator) const noexcept {
            return node_ptr_ != _iterator.node_ptr_;
        }
        FORCEINLINE Iterator_& operator=(const Iterator_& _iterator) noexcept {
            node_ptr_ = _iterator.node_ptr_;
            return *this;
        }
        FORCEINLINE Iterator_& operator=(Iterator_&& _iterator) noexcept {
            node_ptr_ = _iterator.node_ptr_;
            _iterator.node_ptr_ = nullptr;
            return *this;
        }
        FORCEINLINE Iterator_& operator++() noexcept {
            node_ptr_ = node_ptr_->next_node_ptr_;
            return *this;
        }
        NODISCARD FORCEINLINE _ObjectType& operator*() noexcept { return node_ptr_->object_; }
        NODISCARD FORCEINLINE const _ObjectType& operator*() const noexcept { return node_ptr_->object_; }
        NODISCARD FORCEINLINE _ObjectType* operator->() noexcept { return &node_ptr_->object_; }
        NODISCARD FORCEINLINE const _ObjectType* operator->() const noexcept { return &node_ptr_->object_; }
        NODISCARD FORCEINLINE _ObjectType* Ptr() noexcept { return &node_ptr_->object_; }
        NODISCARD FORCEINLINE const _ObjectType* Ptr() const noexcept { return &node_ptr_->object_; }

    private:
        TPoolListSafe::Node_* node_ptr_;
    };

    TPoolListSafe() noexcept
        : SuperType_()
        , mem_ptr_list_(kMemoryPtrListSize)
        , head_node_ptr_(nullptr)
        , end_node_ptr_(nullptr)
        , list_head_node_ptr_(nullptr)
        , model_obj_ptr_(nullptr)
        , mutex_()
        , total_num_(0)
    {}
    TPoolListSafe(TPoolListSafe&& _pool) noexcept
        : SuperType_()
    {
        MoveP(std::forward<TPool>(_pool));
    }
    template<typename... _ArgsType>
    TPoolListSafe(_ArgsType&&... args) noexcept
        : SuperType_()
        , mem_ptr_list_(kMemoryPtrListSize)
        , head_node_ptr_(nullptr)
        , end_node_ptr_(nullptr)
        , list_head_node_ptr_(nullptr)
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

    ~TPoolListSafe() noexcept {
        if constexpr (kIfCallConstructorAndDestructor) {
            if constexpr (kIsClass<_ObjectType>) {
                Node_* node_ptr = head_node_ptr_;
                while (node_ptr != end_node_ptr_) {
                    node_ptr->object_.~_ObjectType();
                    node_ptr = node_ptr->next_node_ptr_;
                }
                node_ptr = list_head_node_ptr_;
                while (node_ptr != nullptr) {
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

    TPoolListSafe& operator=(TPoolListSafe&& _pool) noexcept {
        TLockGuard lock_guard(mutex_);
        MoveP(std::forward<TPool>(_pool));
        return *this;
    }

    /*
        Call Lock to prevent aysnc errors. 
    */
    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return Iterator_(list_head_node_ptr_); }
    /*
        Call Lock to prevent aysnc errors.
    */
    NODISCARD FORCEINLINE const Iterator_ Begin() const noexcept { return Iterator_(list_head_node_ptr_); }
    /*
        Call Lock to prevent aysnc errors.
    */
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return Iterator_(nullptr); }
    /*
        Call Lock to prevent aysnc errors.
    */
    NODISCARD FORCEINLINE const Iterator_ End() const noexcept { return Iterator_(nullptr); }

    Void Push(_ObjectType* _object_ptr) noexcept {
        TLockGuard lock_guard(mutex_);
        Node_* node_ptr = reinterpret_cast<Node_*>(_object_ptr);
        if (head_node_ptr_ == end_node_ptr_) {
            ApplyNewObjectsP();
        }
        node_ptr->next_node_ptr_ = list_head_node_ptr_;
        node_ptr->previous_node_ptr_ = nullptr;
        if (list_head_node_ptr_ != nullptr) {
            list_head_node_ptr_->previous_node_ptr_ = node_ptr;
        }
        list_head_node_ptr_ = node_ptr;
    }

    Void Clear() noexcept { 
        TLockGuard lock_guard(mutex_);
        end_node_ptr_->next_node_ptr_ = list_head_node_ptr_;
        while (end_node_ptr_->next_node_ptr_ != nullptr) {
            end_node_ptr_ = list_end_node_ptr_;
        }
    }

    template<typename... _ArgsType>
    Void SetModel(_ArgsType&&... args) noexcept {
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
        if (head_node_ptr_ == end_node_ptr_) {
            ApplyNewObjectsP();
        }
        Node_* apply_node_ptr = head_node_ptr_;
        head_node_ptr_ = head_node_ptr_->next_node_ptr_;
        apply_node_ptr->next_node_ptr_ = nullptr;
        apply_node_ptr->previous_node_ptr_ = nullptr;
        return &apply_node_ptr->object_;
    }

    NODISCARD Iterator_ Release(Iterator_ _iterator) noexcept {
        TLockGuard lock_guard(mutex_);
        Node_* release_node_ptr = reinterpret_cast<Node_*>(_iterator.Ptr());
        Iterator_  ret_iterator = ++_iterator;
        if (list_head_node_ptr_ == release_node_ptr) {
            list_head_node_ptr_ = list_head_node_ptr_->next_node_ptr_;
        }
        if (release_node_ptr->previous_node_ptr_ != nullptr) {
            release_node_ptr->previous_node_ptr_->next_node_ptr_ = release_node_ptr->next_node_ptr_;
        }
        if (release_node_ptr->next_node_ptr_ != nullptr) {
            release_node_ptr->next_node_ptr_->previous_node_ptr_ = release_node_ptr->previous_node_ptr_;
        }
        end_node_ptr_->next_node_ptr_ = release_node_ptr;
        end_node_ptr_ = release_node_ptr;
        return ret_iterator;
    }

    Void Release(_ObjectType* _object_ptr) noexcept {
        TLockGuard lock_guard(mutex_);
        Node_* release_node_ptr = reinterpret_cast<Node_*>(_object_ptr);
        if (list_head_node_ptr_ == release_node_ptr) {
            list_head_node_ptr_ = list_head_node_ptr_->next_node_ptr_;
        }
        if (release_node_ptr->previous_node_ptr_ != nullptr) {
            release_node_ptr->previous_node_ptr_->next_node_ptr_ = release_node_ptr->next_node_ptr_;
        }
        if (release_node_ptr->next_node_ptr_ != nullptr) {
            release_node_ptr->next_node_ptr_->previous_node_ptr_ = release_node_ptr->previous_node_ptr_;
        }
        end_node_ptr_->next_node_ptr_ = release_node_ptr;
        end_node_ptr_ = release_node_ptr;
    }

    /*
        Extends the pool to a size >= given size. If _size = 0 then auto calculates the extending size.
    */
    FORCEINLINE Void ExtendPool(IndexType _size = 0) noexcept {
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
    friend class TPoolListSafe<_ObjectType>::Iterator_;

private:
    struct Node_ {
        _ObjectType object_;
        Node_* next_node_ptr_;
        Node_* previous_node_ptr_;
    };

    TPoolListSafe(const TPoolListSafe&) = delete;
    TPoolListSafe& operator=(const TPoolListSafe&) = delete;

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

    Void MoveP(TPoolListSafe&& _pool) noexcept {
        mem_ptr_list_ = std::move(_pool.mem_ptr_list_);
        mutex_ = std::move(_pool.mutex_);
        head_node_ptr_ = _pool.head_node_ptr_;
        end_node_ptr_ = _pool.end_node_ptr_;
        list_head_node_ptr_ = _pool.list_head_node_ptr_;
        model_obj_ptr_ = _pool.model_obj_ptr_;
        total_num_ = _pool.total_num_;
        _pool.head_node_ptr_ = nullptr;
        _pool.end_node_ptr_ = nullptr;
        _pool.list_head_node_ptr_ = nullptr;
        _pool.model_obj_ptr_ = nullptr;
        _pool.total_num_ = 0;
    }

    MemoryPtrList_ mem_ptr_list_;
    Node_* head_node_ptr_;
    Node_* end_node_ptr_;
    Node_* list_head_node_ptr_;
    Node_* list_end_node_ptr_;
    _ObjectType* model_obj_ptr_;
    ZMutex mutex_;
    IndexType total_num_;
};


}//zengine