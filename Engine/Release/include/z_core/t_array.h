/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#ifndef Z_CORE_T_ARRAY_H_
#define Z_CORE_T_ARRAY_H_

#include "internal/z_drive.h"

#include <array>

#include "z_object.h"

namespace zengine {

namespace internal {

template<typename _ArrayType, typename _Function, typename... _ArgsType>
concept kIsArrayInitFunction = requires(_ArrayType* _array, _Function _func, _ArgsType&&... _args) {
    _func(_array, std::forward<_ArgsType>(_args)...);
};

}//internal

/*
    Array caintainer.
*/
template<typename _ObjectType, IndexType kCapacity>
class TArray : public ZObject {
public:
    using STDArray_ = std::array<_ObjectType, kCapacity>;
    using Iterator_ = STDArray_::iterator;
    using ConstIterator_ = STDArray_::const_iterator;
    using ReverseIterator_ = STDArray_::reverse_iterator;
    using ConstReverseIterator_ = STDArray_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE constexpr TArray() noexcept : SuperType_(), array_() {}
    FORCEINLINE constexpr TArray(const TArray& _array) noexcept : SuperType_(), array_(_array.array_) {}
    FORCEINLINE constexpr TArray(TArray&& _array) noexcept : SuperType_(), array_(std::move(_array.array_)) {}
    FORCEINLINE TArray(InitializerList_ _init_list) noexcept : SuperType_(), array_(_init_list) {}

    /*
        Constexpr array, the work is done at compile time.
        The Constructor's fisrt parameter is the initial funtion of the array. The
        first parameter of the initial funtion must be TArray*.
        Constructor Template Parameters:
        - InitFunction: The function type that initial the array.
        - ArgsType...: The parameters type.
        Constructor Parameters:
        - init_function: The function to initial the array.
        - ArgsType...: The parameters of the function except for the fisrt.
        Example:
        constexpr auto init_func = [](TArray<Int32, 10>* _array_ptr) {
            for (IndexType index = 0; index < _array_ptr->Size(); ++index) {
                (*_array_ptr)[index] = 1;
            }
        };
        constexpr TArray<Int32, 10> test(init_func);
    */
    template<typename _InitFunction, typename... _ArgsType>
    requires internal::kIsArrayInitFunction<TArray<_ObjectType, kCapacity>, _InitFunction, _ArgsType...>
    FORCEINLINE constexpr TArray(_InitFunction&& _init_func, _ArgsType&&... _args) noexcept : SuperType_() {
        _init_func(this, std::forward<_ArgsType>(_args)...);
    }

    FORCEINLINE constexpr ~TArray() noexcept {}

    NODISCARD FORCEINLINE constexpr Bool operator==(const TArray& _array) noexcept { return array_ == _array.array_; }
    NODISCARD FORCEINLINE constexpr Bool operator!=(const TArray& _array) noexcept { return array_ != _array.array_; }
     
    NODISCARD FORCEINLINE constexpr _ObjectType& operator[](IndexType _index) noexcept { return array_[_index]; }
    NODISCARD FORCEINLINE constexpr const _ObjectType& operator[](IndexType _index) const noexcept { 
        return array_[_index]; 
    }

    NODISCARD FORCEINLINE constexpr _ObjectType& At(IndexType _index) noexcept { return array_.at(_index); }
    NODISCARD FORCEINLINE constexpr const _ObjectType& At(IndexType _index) const noexcept { return array_.at(_index); }
    NODISCARD FORCEINLINE constexpr _ObjectType& Front() noexcept { return array_.front(); }
    NODISCARD FORCEINLINE constexpr const _ObjectType& Front() const noexcept { return array_.front(); }
    NODISCARD FORCEINLINE constexpr _ObjectType& Back() noexcept { return array_.back(); }
    NODISCARD FORCEINLINE constexpr const _ObjectType& Back() const noexcept { return array_.back(); }
    NODISCARD FORCEINLINE constexpr _ObjectType* DataPtr() noexcept { return array_.data(); }
    NODISCARD FORCEINLINE constexpr const _ObjectType* DataPtr() const noexcept { return array_.data(); }

    NODISCARD FORCEINLINE static constexpr IndexType Capacity() noexcept { return kCapacity; }

    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return array_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return array_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return array_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return array_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return array_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return array_.crbegin(); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return array_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return array_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return array_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return array_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return array_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return array_.crend(); }

    FORCEINLINE constexpr Void Fill(const _ObjectType& _val) noexcept { array_.fill(_val); }
    FORCEINLINE constexpr Void FillZero() noexcept { memset(array_.data(), 0, sizeof(_ObjectType) * kCapacity); }
    FORCEINLINE constexpr Void Swap(TArray& _array) noexcept { array_.swap(_array); }

protected:
    using SuperType_ = ZObject;

private:
    STDArray_ array_;
};

}//zengine

#endif // !Z_CORE_T_ARRAY_H_