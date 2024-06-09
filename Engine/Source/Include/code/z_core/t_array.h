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

template<typename ArrayType, typename Function, typename... ArgsType>
concept kIsArrayInitFunction = requires(ArrayType* array, Function function, ArgsType&&... args) {
    function(array, std::forward<ArgsType>(args)...);
};

}//internal

/*
    Array caintainer.
*/
template<typename ObjectType, IndexType kCapacity>
class TArray : public ZObject {
public:
    using STDArray = std::array<ObjectType, kCapacity>;
    using Iterator = STDArray::iterator;
    using ConstIterator = STDArray::const_iterator;
    using ReverseIterator = STDArray::reverse_iterator;
    using ConstReverseIterator = STDArray::const_reverse_iterator;
    using InitializerList = std::initializer_list<ObjectType>;

    FORCEINLINE constexpr TArray() noexcept : SuperType(), array_() {}
    FORCEINLINE constexpr TArray(const TArray& array) noexcept : SuperType(), array_(array.array_) {}
    FORCEINLINE constexpr TArray(TArray&& array) noexcept : SuperType(), array_(std::move(array.array_)) {}
    FORCEINLINE TArray(InitializerList init_list) noexcept : SuperType(), array_(init_list) {}

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
        constexpr auto init_function = [](TArray<Int32, 10>* array_ptr) {
            for (IndexType index = 0; index < array_ptr->Size(); ++index) {
                (*array_ptr)[index] = 1;
            }
        };
        constexpr TArray<Int32, 10> test(init_function);
    */
    template<typename InitFunction, typename... ArgsType>
    requires internal::kIsArrayInitFunction<TArray<ObjectType, kCapacity>, InitFunction, ArgsType...>
    FORCEINLINE constexpr TArray(InitFunction&& init_function, ArgsType&&... args) noexcept : SuperType() {
        init_function(this, std::forward<ArgsType>(args)...);
    }

    FORCEINLINE constexpr ~TArray() noexcept {}

    NODISCARD FORCEINLINE constexpr Bool operator==(const TArray& array) noexcept { return array_ == array.array_; }
    NODISCARD FORCEINLINE constexpr Bool operator!=(const TArray& array) noexcept { return array_ != array.array_; }
     
    NODISCARD FORCEINLINE constexpr ObjectType& operator[](IndexType index) noexcept { return array_[index]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& operator[](IndexType index) const noexcept { return array_[index]; }

    NODISCARD FORCEINLINE constexpr ObjectType& At(IndexType index) noexcept { return array_.at(index); }
    NODISCARD FORCEINLINE constexpr const ObjectType& At(IndexType index) const noexcept { return array_.at(index); }
    NODISCARD FORCEINLINE constexpr ObjectType& Front() noexcept { return array_.front(); }
    NODISCARD FORCEINLINE constexpr const ObjectType& Front() const noexcept { return array_.front(); }
    NODISCARD FORCEINLINE constexpr ObjectType& Back() noexcept { return array_.back(); }
    NODISCARD FORCEINLINE constexpr const ObjectType& Back() const noexcept { return array_.back(); }
    NODISCARD FORCEINLINE constexpr ObjectType* DataPtr() noexcept { return array_.data(); }
    NODISCARD FORCEINLINE constexpr const ObjectType* DataPtr() const noexcept { return array_.data(); }

    NODISCARD FORCEINLINE static constexpr IndexType Capacity() noexcept { return kCapacity; }

    NODISCARD FORCEINLINE Iterator Begin() noexcept { return array_.begin(); }
    NODISCARD FORCEINLINE ConstIterator Begin() const noexcept { return array_.begin(); }
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const noexcept { return array_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseBegin() noexcept { return array_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseBegin() const noexcept { return array_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseBegin() const noexcept { return array_.crbegin(); }
    NODISCARD FORCEINLINE Iterator End() noexcept { return array_.end(); }
    NODISCARD FORCEINLINE ConstIterator End() const noexcept { return array_.end(); }
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const noexcept { return array_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseEnd() noexcept { return array_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseEnd() const noexcept { return array_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseEnd() const noexcept { return array_.crend(); }

    FORCEINLINE constexpr Void Fill(const ObjectType& value) noexcept { array_.fill(value); }
    FORCEINLINE constexpr Void FillZero() noexcept { memset(array_.data(), 0, sizeof(ObjectType) * kCapacity); }
    FORCEINLINE constexpr Void Swap(TArray& array) noexcept { array_.swap(array); }

protected:
    using SuperType = ZObject;

private:
    STDArray array_;
};

}//zengine

#endif // !Z_CORE_T_ARRAY_H_