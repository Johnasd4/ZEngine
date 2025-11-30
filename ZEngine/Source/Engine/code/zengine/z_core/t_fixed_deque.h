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

#include "z_object.h"

namespace zengine {

namespace internal {

template<typename _FixedDequeType, typename _Function, typename... _ArgsType>
concept kIsFixedDequeInitFunction = requires(_FixedDequeType * _deque, _Function _func, _ArgsType&&... _args) {
    _func(_deque, std::forward<_ArgsType>(_args)...);
};

}//internal

/*
    Fixed deque container, front points at the first object, back points at the last object.
*/
template<typename _ObjectType, SizeType kCapacity>
class TFixedDeque : public ZObject {
public:
    using STDArray = std::array<_ObjectType, kCapacity>;
    using InitializerList = std::initializer_list<_ObjectType>;

    FORCEINLINE constexpr TFixedDeque() noexcept 
        : SuperType_()
        , deque_()
        , front_index_(0ULL)
        , back_index_(kCapacity - 1ULL)
        , size_(0ULL) {}
    FORCEINLINE constexpr TFixedDeque(const TFixedDeque& _deque) noexcept : SuperType_(_deque){ CopyP(_deque); }
    FORCEINLINE constexpr TFixedDeque(TFixedDeque&& _deque) noexcept 
        : SuperType_(std::forward<TFixedDeque>(_deque)) 
    {
        MoveP(std::forward<TFixedDeque>(_deque)); 
    }
    FORCEINLINE TFixedDeque(InitializerList _init_list) noexcept 
        : SuperType_()
        , deque_(_init_list)
        , front_index_(0ULL)
        , back_index_(kCapacity - 1ULL)
        , size_(kCapacity) {}

    /*
        Constexpr deque, the work is done at compile time.
        The Constructor's fisrt parameter is the initial funtion of the deque. The
        first parameter of the initial funtion must be TFixedDeque*.
        Constructor Template Parameters:
        - InitFunction: The function type that initial the deque.
        - ArgsType...: The parameters type.
        Constructor Parameters:
        - init_function: The function to initial the deque.
        - ArgsType...: The parameters of the function except for the fisrt.
        Example:
        inline constexpr auto init_func = [](TFixedDeque<SizeType, 10ULL>* _deque_ptr) {
            for (SizeType index = 0ULL; index < _deque_ptr->Size(); ++index) {
                (*_deque_ptr)[index] = 1ULL;
            }
        };
        inline constexpr TFixedDeque<SizeType, 10ULL> test(init_func);
    */
    template<typename _InitFunction, typename... _ArgsType>
    requires internal::kIsFixedDequeInitFunction<TFixedDeque<_ObjectType, kCapacity>, _InitFunction, _ArgsType...>
    FORCEINLINE constexpr TFixedDeque(_InitFunction&& _init_func, _ArgsType&&... _args) noexcept : SuperType_() {
        _init_func(this, std::forward<_ArgsType>(_args)...);
    }

    FORCEINLINE constexpr ~TFixedDeque() noexcept {}

    FORCEINLINE TFixedDeque& operator=(const TFixedDeque& _deque) noexcept {
        SuperType_::operator=(_deque);
        CopyP(_deque);
        return *this;
    }
    FORCEINLINE TFixedDeque& operator=(TFixedDeque&& _deque) noexcept {
        SuperType_::operator=(std::forward<TFixedDeque>(_deque));
        MoveP(std::forward<TFixedDeque>(_deque));
        return *this;
    }

    NODISCARD FORCEINLINE constexpr Bool operator==(const TFixedDeque& _deque) noexcept { 
        return deque_ == _deque.deque_; 
    }
    NODISCARD FORCEINLINE constexpr Bool operator!=(const TFixedDeque& _deque) noexcept { 
        return deque_ != _deque.deque_; 
    }
     
    NODISCARD FORCEINLINE constexpr _ObjectType& operator[](SizeType _index) noexcept { 
        return deque_[(front_index_ + _index) % kCapacity];
    }
    NODISCARD FORCEINLINE constexpr const _ObjectType& operator[](SizeType _index) const noexcept { 
        return deque_[(front_index_ + _index) % kCapacity];
    }

    NODISCARD FORCEINLINE constexpr _ObjectType& At(SizeType _index) noexcept { 
        return deque_.at((SizeType)((front_index_ + _index) % kCapacity));
    }
    NODISCARD FORCEINLINE constexpr const _ObjectType& At(SizeType _index) const noexcept { 
        return deque_.at((SizeType)((front_index_ + _index) % kCapacity));
    }
    NODISCARD FORCEINLINE constexpr _ObjectType& Front() noexcept { return deque_[front_index_]; }
    NODISCARD FORCEINLINE constexpr const _ObjectType& Front() const noexcept { return deque_[front_index_]; }
    NODISCARD FORCEINLINE constexpr _ObjectType& Back() noexcept { return deque_[back_index_]; }
    NODISCARD FORCEINLINE constexpr const _ObjectType& Back() const noexcept { return deque_[back_index_]; }
    NODISCARD FORCEINLINE constexpr _ObjectType* DataPtr() noexcept { return deque_.data(); }
    NODISCARD FORCEINLINE constexpr const _ObjectType* DataPtr() const noexcept { return deque_.data(); }

    NODISCARD FORCEINLINE static constexpr SizeType Capacity() noexcept { return kCapacity; }
    NODISCARD FORCEINLINE constexpr SizeType Size() noexcept { return size_; }
    NODISCARD FORCEINLINE constexpr Bool Empty() noexcept { return size_ == 0ULL; }

    constexpr Void PopFront() noexcept { 
        --size_;
        front_index_ = (front_index_ + 1ULL) % kCapacity;
    }  
    constexpr Void PopBack() noexcept {
        --size_;
        back_index_ = (back_index_ + (kCapacity - 1ULL)) % kCapacity;
    }
    constexpr Void PushFront(const _ObjectType& _obj) noexcept {
        front_index_ = (front_index_ + (kCapacity - 1ULL)) % kCapacity;
        deque_[front_index_] = _obj;
        ++size_;
    }
    constexpr Void PushFront(_ObjectType&& _obj) noexcept {
        front_index_ = (front_index_ + (kCapacity - 1ULL)) % kCapacity;
        deque_[front_index_] = std::forward<_ObjectType>(_obj);
        ++size_;
    }
    template<typename... _ArgsType>
    constexpr _ObjectType& EmplaceFront(_ArgsType&&... _args) noexcept {
        front_index_ = (front_index_ + 1ULL) % kCapacity;
        new(&deque_[front_index_]) _ObjectType(std::forward<_ArgsType>(_args)...);
        ++size_;
        return deque_[front_index_];
    }
    constexpr Void PushBack(const _ObjectType& _obj) noexcept {
        back_index_ = (back_index_ + 1ULL) % kCapacity;
        deque_[back_index_] = _obj;
        ++size_;
    }
    constexpr Void PushBack(_ObjectType&& _obj) noexcept {
        back_index_ = (back_index_ + 1ULL) % kCapacity;
        deque_[back_index_] = std::forward<_ObjectType>(_obj);
        ++size_;
    }
    template<typename... _ArgsType>
    constexpr _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        back_index_ = (back_index_ + 1ULL) % kCapacity;
        new(&deque_[back_index_]) _ObjectType(std::forward<_ArgsType>(_args)...);
        ++size_;
        return deque_[back_index_];
    }

    constexpr Void SetSize(SizeType _size) noexcept {
        size_ = _size; 
        back_index_ = (front_index_ + size_ - 1ULL) % kCapacity;
    }
    constexpr Void Clear() noexcept { 
        size_ = 0ULL;
        front_index_ = 0ULL;
        back_index_ = kCapacity - 1ULL;
    }
    constexpr Void Fill(const _ObjectType& _value) noexcept {
        deque_.fill(_value); 
        front_index_ = 0ULL;
        back_index_ = kCapacity - 1ULL;
    }
    FORCEINLINE constexpr Void Swap(TFixedDeque& _deque) noexcept { deque_.swap(_deque); }

protected:
    using SuperType_ = ZObject;

private:
    FORCEINLINE Void CopyP(const TFixedDeque& _deque) noexcept {
        deque_ = _deque.deque_;
        front_index_ = _deque.front_index_;
        back_index_ = _deque.back_index_;
        size_ = _deque.size_;
    }

    FORCEINLINE Void MoveP(TFixedDeque&& _deque) noexcept {
        deque_ = std::move(_deque.deque_);
        front_index_ = _deque.front_index_;
        back_index_ = _deque.back_index_;
        size_ = _deque.size_;
        _deque.front_index_ = 0ULL;
        _deque.back_index_ = kCapacity - 1ULL;
        _deque.size_ = 0ULL;
    }

    STDArray deque_;
    SizeType front_index_;
    SizeType back_index_;
    SizeType size_;
};

}//zengine