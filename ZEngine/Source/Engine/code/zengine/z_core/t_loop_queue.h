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

#include "m_log.h"
#include "t_lock_guard.h"
#include "z_mutex.h"
#include "z_object.h"

namespace zengine {

/*
    Loog queue container, front points at the first object, back points at the last object.
    The capacity is fixed when constructed, will not resize auto.
    Object's contructor will be called when the queue resize and destructor will be called when the pool destructs.
*/
template<typename _ObjectType, Bool kIfCallConstructorAndDestructor = kIsClassType<_ObjectType>>
class TLoopQueue : public ZObject {
private:
    static inline constexpr Float32 kAutoExtendMultFactor = 1.5f;

public:
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TLoopQueue(SizeType _capacity) noexcept 
        : SuperType_() 
        , data_ptr_(memory_pool::ApplyMemory(_capacity * sizeof(_ObjectType), &capacity_))
        , front_index_(0ULL)
        , back_index_(_capacity - 1ULL)
        , size_(0ULL) 
    {
        capacity_ = capacity_ / sizeof(_ObjectType);
    }
    FORCEINLINE TLoopQueue(const TLoopQueue& _queue) noexcept 
        : SuperType_(_queue) 
        , capacity_(0ULL)
    { 
        CopyP(_queue); 
    }
    FORCEINLINE TLoopQueue(TLoopQueue&& _queue) noexcept 
        : SuperType_(std::forward<TLoopQueue>(_queue)) 
    {
        MoveP(std::forward<TLoopQueue>(_queue));
    }

    FORCEINLINE ~TLoopQueue() noexcept {
        DestroyAllObjectsP();
        if (data_ptr_ != nullptr) {
            memory_pool::ReleaseMemory(data_ptr_);
        }
    }

    FORCEINLINE TLoopQueue& operator=(const TLoopQueue& _queue) noexcept {
        if (this == &_queue) {
            return *this;
        }
        SuperType_::operator=(_queue);
        CopyP(_queue);
        return *this;
    }
    FORCEINLINE TLoopQueue& operator=(TLoopQueue&& _queue) noexcept {
        if (this == &_queue) {
            return *this;
        }
        SuperType_::operator=(std::forward<TLoopQueue>(_queue));
        MoveP(std::forward<TLoopQueue>(_queue));
        return *this;
    }

    NODISCARD FORCEINLINE Bool operator==(const TLoopQueue& _queue) noexcept { 
        return data_ptr_ == _queue.data_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TLoopQueue& _queue) noexcept { 
        return data_ptr_ != _queue.data_ptr_;
    }
     
    NODISCARD FORCEINLINE _ObjectType& operator[](SizeType _index) noexcept { 
        return data_ptr_[(front_index_ + _index) % capacity_];
    }
    NODISCARD FORCEINLINE const _ObjectType& operator[](SizeType _index) const noexcept { 
        return data_ptr_[(front_index_ + _index) % capacity_];
    }

    NODISCARD FORCEINLINE _ObjectType& At(SizeType _index) noexcept { 
        return data_ptr_[(front_index_ + _index) % capacity_];
    }
    NODISCARD FORCEINLINE const _ObjectType& At(SizeType _index) const noexcept { 
        return data_ptr_[(front_index_ + _index) % capacity_];
    }
    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return data_ptr_[front_index_]; }
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return data_ptr_[front_index_]; }
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { return data_ptr_[back_index_]; }
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return data_ptr_[back_index_]; }

    NODISCARD FORCEINLINE SizeType Capacity() noexcept { return capacity_; }
    NODISCARD FORCEINLINE SizeType Size() noexcept { return size_; }
    NODISCARD FORCEINLINE Bool Empty() noexcept { return size_ == 0ULL; }
    NODISCARD FORCEINLINE Bool Full() noexcept { return size_ == capacity_; }

    Void PopFront() noexcept { 
        if (size_ > 0ULL) {
            --size_;
            DestroyObjectP(front_index_);
            front_index_ = (front_index_ + 1ULL) % capacity_;
        }
    }  
    Void PushBack(const _ObjectType& _obj) noexcept {
        if (size_ == capacity_) {
            Reserve(static_cast<SizeType>(static_cast<Float32>(size_) * kAutoExtendMultFactor));
        }
        back_index_ = (back_index_ + 1ULL) % capacity_;
        CreateObjectP(back_index_, _obj);
        ++size_;
    }
    Void PushBack(_ObjectType&& _obj) noexcept {
        if (size_ == capacity_) {
            Reserve(static_cast<SizeType>(static_cast<Float32>(size_) * kAutoExtendMultFactor));
        }
        back_index_ = (back_index_ + 1ULL) % capacity_;
        CreateObjectP(back_index_, std::forward<_ObjectType>(_obj));
        ++size_;
    }
    template<typename... _ArgsType>
    Void EmplaceBack(_ArgsType&&... _args) noexcept {
        if (size_ == capacity_) {
            Reserve(static_cast<SizeType>(static_cast<Float32>(size_) * kAutoExtendMultFactor));
        }
        back_index_ = (back_index_ + 1ULL) % capacity_;
        if constexpr (kIfCallConstructorAndDestructor && kIsClassType<_ObjectType>) {
            new(data_ptr_ + back_index_) _ObjectType(std::forward<_ArgsType>(_args)...);
        }
        else {
            data_ptr_[back_index_] = _ObjectType(std::forward<_ArgsType>(_args)...);
        }


        ++size_;
    }

    Void Clear() noexcept { 
        DestroyAllObjectsP();
        size_ = 0ULL;
        front_index_ = 0ULL;
        back_index_ = capacity_ - 1ULL;
    }

    Void Reserve(SizeType _capacity) noexcept {
        if (_capacity <= capacity_) {
            return;
        }

        //apply memory
        SizeType new_memory_size = 0;
        _ObjectType* new_data_ptr = static_cast<_ObjectType*>(memory_pool::ApplyMemory(
            _capacity * sizeof(_ObjectType), &new_memory_size
        ));

        //calculate new capacity
        _capacity = new_memory_size / sizeof(_ObjectType);

        //move old data to new memory
        if (size_ > 0) {
            if constexpr (kIfCallConstructorAndDestructor && kIsClassType<_ObjectType>) {
                SizeType source_index = front_index_;
                for (SizeType index = 0; index < size_; ++index) {
                    new(new_data_ptr + index) _ObjectType(std::move(data_ptr_[source_index]));
                    DestroyObjectP(source_index);
                    source_index = (source_index + 1ULL) % capacity_;
                }
            }
            else {
                if (front_index_ <= back_index_) {
                    Copy(new_data_ptr, data_ptr_ + front_index_, size_ * sizeof(_ObjectType));
                }
                else {
                    SizeType first_copy_size = capacity_ - front_index_;
                    Copy(new_data_ptr, data_ptr_ + front_index_, first_copy_size * sizeof(_ObjectType));
                    Copy(new_data_ptr + first_copy_size, data_ptr_, (size_ - first_copy_size) * sizeof(_ObjectType));
                }
            }
        }

        //release old memory
        if (data_ptr_ != nullptr) {
            memory_pool::ReleaseMemory(data_ptr_);
        }

        //update members
        data_ptr_ = new_data_ptr;
        capacity_ = _capacity;
        front_index_ = 0ULL;
        back_index_ = (size_ == 0) ? (capacity_ - 1ULL) : (size_ - 1ULL);
    }

protected:
    using SuperType_ = ZObject;

private:
    FORCEINLINE Void CreateObjectP(SizeType _index) noexcept {
        if constexpr (kIfCallConstructorAndDestructor && kIsClassType<_ObjectType>) {
            new(data_ptr_ + _index) _ObjectType();
        }
    }

    FORCEINLINE Void CreateObjectP(SizeType _index, const _ObjectType& _obj) noexcept {
        if constexpr (kIfCallConstructorAndDestructor && kIsClassType<_ObjectType>) {
            new(data_ptr_ + _index) _ObjectType(_obj);
        }
        else {
            data_ptr_[_index] = _obj;
        }
    }

    FORCEINLINE Void CreateObjectP(SizeType _index, _ObjectType&& _obj) noexcept {
        if constexpr (kIfCallConstructorAndDestructor && kIsClassType<_ObjectType>) {
            new(data_ptr_ + _index) _ObjectType(std::forward<_ObjectType>(_obj));
        }
        else {
            data_ptr_[_index] = std::forward<_ObjectType>(_obj);
        }
    }

    FORCEINLINE Void DestroyObjectP(SizeType _index) noexcept {
        if constexpr (kIfCallConstructorAndDestructor) {
            if constexpr (kIsClassType<_ObjectType>) {
                data_ptr_[_index].~_ObjectType();
            }
        }
    }

    FORCEINLINE Void DestroyAllObjectsP() noexcept {
        if constexpr (kIfCallConstructorAndDestructor) {
            if constexpr (kIsClassType<_ObjectType>) {
                if(size_ == 0ULL) {
                    return;
                }
                SizeType index = front_index_;
                while (index != back_index_) {
                    data_ptr_[index].~_ObjectType();
                    index = (index + 1ULL) % capacity_;
                }
                data_ptr_[back_index_].~_ObjectType();
            }
        }
    }

    FORCEINLINE Void CopyP(const TLoopQueue& _queue) noexcept {
        static_assert(
            std::is_trivially_copyable_v<_ObjectType>,
            "TLoopQueue::CopyP(): Type must be trivially copyable!"
        );
        //resize memory if needed
        if(capacity_ < _queue.size_) {
            if (data_ptr_ != nullptr) {
                DestroyAllObjectsP();
                memory_pool::ReleaseMemory(data_ptr_);
            }
            //allocate new memory
            data_ptr_ = static_cast<_ObjectType*>(memory_pool::ApplyMemory(
                _queue.size_ * sizeof(_ObjectType), &capacity_
            ));
            capacity_ = capacity_ / sizeof(_ObjectType);
        }

        //copy objects
        if (_queue.size_ != 0ULL) {
            if(_queue.front_index_ <= _queue.back_index_) {
                Copy(data_ptr_, _queue.data_ptr_ + _queue.front_index_, _queue.size_ * sizeof(_ObjectType));
            }
            else {
                SizeType first_copy_size = _queue.capacity_ - _queue.front_index_;
                Copy(data_ptr_, _queue.data_ptr_ + _queue.front_index_, first_copy_size * sizeof(_ObjectType));
                Copy(
                    data_ptr_ + first_copy_size,
                    _queue.data_ptr_, 
                    (_queue.size_ - first_copy_size) * sizeof(_ObjectType)
                );
            }
            back_index_ = size_ - 1ULL;
        }
        else {
            back_index_ = capacity_ - 1ULL;
        }
        front_index_ = 0ULL;
        size_ = _queue.size_;
    }

    FORCEINLINE Void MoveP(TLoopQueue&& _queue) noexcept {
        if (data_ptr_ != nullptr) {
            DestroyAllObjectsP();
            memory_pool::ReleaseMemory(data_ptr_);
        }
        data_ptr_ = _queue.data_ptr_;
        front_index_ = _queue.front_index_;
        back_index_ = _queue.back_index_;
        size_ = _queue.size_;
        capacity_ = _queue.capacity_;
        _queue.data_ptr_ = nullptr;
        _queue.front_index_ = 0ULL;
        _queue.back_index_ = 0ULL;
        _queue.size_ = 0ULL;
        _queue.capacity_ = 0ULL;
    }

    _ObjectType* data_ptr_;
    SizeType front_index_;
    SizeType back_index_;
    SizeType size_;
    SizeType capacity_;
};

}//zengine