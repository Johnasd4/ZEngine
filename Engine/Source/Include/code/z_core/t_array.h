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

#include "internal/drive.h"

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
    Arrayr caintainer.
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

    FORCEINLINE constexpr TArray() noexcept : array_() {}
    FORCEINLINE constexpr TArray(const TArray& array) noexcept : array_(array.array_) {}
    FORCEINLINE constexpr TArray(TArray&& array) noexcept : array_(std::move(array.array_)) {}
    FORCEINLINE TArray(InitializerList init_list) noexcept : array_(init_list) {}

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
            for (IndexType index = 0; index < array_ptr->size(); ++index) {
                (*array_ptr)[index] = 1;
            }
        };
        constexpr TArray<Int32, 10> test(init_function);
    */
    template<typename InitFunction, typename... ArgsType>
    requires internal::kIsArrayInitFunction<TArray<ObjectType, kCapacity>, InitFunction, ArgsType...>
    FORCEINLINE constexpr TArray(InitFunction&& init_function, ArgsType&&... args) : SuperType() {
        init_function(this, std::forward<ArgsType>(args)...);
    }

    NODISCARD FORCEINLINE constexpr Bool operator==(const TArray& array) noexcept { return array_ == array.array_; }
    NODISCARD FORCEINLINE constexpr Bool operator!=(const TArray& array) noexcept { return array_ != array.array_; }

    NODISCARD FORCEINLINE constexpr ObjectType& operator[](IndexType index) { return array_[index]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& operator[](IndexType index) const { return array_[index]; }

    NODISCARD FORCEINLINE constexpr ObjectType& At(IndexType index) {
        return data_[index];
    }
    NODISCARD FORCEINLINE constexpr const ObjectType& At(IndexType index) const {
        return data_[index];
    }
    NODISCARD FORCEINLINE ObjectType* AtPtr(IndexType index) {
        return data_ + index;
    }
    NODISCARD FORCEINLINE const ObjectType* AtPtr(IndexType index) const {
        return data_ + index;
    }
    NODISCARD FORCEINLINE constexpr ObjectType& Front() { return data_[0]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& Front() const { return data_[0]; }
    NODISCARD FORCEINLINE ObjectType* FrontPtr() { return data_; }
    NODISCARD FORCEINLINE const ObjectType* FrontPtr() const { return data_; }
    NODISCARD FORCEINLINE constexpr ObjectType& Back() { return data_[kCapacity - 1]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& Back() const { return data_[kCapacity - 1]; }
    NODISCARD FORCEINLINE ObjectType* BackPtr() { return data_ + (kCapacity - 1); }
    NODISCARD FORCEINLINE const ObjectType* BackPtr() const { return data_ + (kCapacity - 1); }

    NODISCARD FORCEINLINE constexpr const ObjectType* data_ptr() const { return data_; }

    NODISCARD static constexpr IndexType size() { return kCapacity; }

    /*
        The iterator funcions.
    */
    NODISCARD FORCEINLINE Iterator Begin() { return Iterator(data_); }
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const { return ConstIterator(data_); }
    NODISCARD FORCEINLINE ReverseIterator ReverseBegin() { return ReverseIterator(data_ + kCapacity - 1); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseBegin() const {
        return ConstReverseIterator(data_ + kCapacity - 1);
    }
    NODISCARD FORCEINLINE Iterator End() { return Iterator(data_ + kCapacity); }
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const { return ConstIterator(data_ + kCapacity); }
    NODISCARD FORCEINLINE ReverseIterator ReverseEnd() { return ReverseIterator(data_ - 1); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseEnd() const {
        return ConstReverseIterator(data_ - 1);
    }



    template<typename... ArgsType>
    inline Void Fill(ArgsType&&... args) noexcept;

protected:
    using SuperType = ZObject;

private:
    STDArray array_;
};

}//zengine

#endif // !Z_CORE_T_ARRAY_H_