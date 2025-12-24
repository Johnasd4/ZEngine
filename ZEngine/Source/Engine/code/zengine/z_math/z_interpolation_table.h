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

#include "../z_core/t_fixed_array.h"

namespace zengine {
namespace math {

template<typename NumberType, SizeType kTableSize> 
class ZInterpolationTable :public TFixedArray<NumberType, kTableSize> {
public:
    /*
        The work is done at compile time.
        Tne first parameter is added to the index when using the table.
        Tne second parameter is distance between the table values.
        The Constructor's third parameter is the initial funtion of the table. 
        The first parameter of the initial funtion must be ZFixedArray*.
        Constructor Template Parameters:
        - InitFunction: The function type that initial the array.
        - ArgsType...: The parameters type.
        Constructor Parameters:
        - init_function: The function to initial the array.
        - ArgsType...: The parameters of the function except for the fisrt.
        Example:
        inline constexpr auto init_function = [](ZInterpolationTable<Float32, 10>* table_ptr) {
            for (SizeType index = 0; index < table_ptr->size(); ++index) {
                (*table_ptr)(index) = 1.0F;
            }
        };
        inline constexpr ZInterpolationTable<Int32, 10> test(10.0F,10.0F,init_function);
    */
    template<typename InitFunction, typename... ArgsType>
    FORCEINLINE constexpr ZInterpolationTable(const NumberType _index_offset, const NumberType _step_distance,
                                                InitFunction&& _init_func, ArgsType&&... _args);

    NODISCARD FORCEINLINE constexpr NumberType index_offset() const { return index_offset_; }
    NODISCARD FORCEINLINE constexpr NumberType step_distance() const {
        return static_cast<NumberType>(1.0) / step_distance_reciprocal_; 
    }
    NODISCARD FORCEINLINE constexpr NumberType step_distance_reciprocal() const { return step_distance_reciprocal_; }
    /*
        Search the table.
    */
    NODISCARD FORCEINLINE constexpr const NumberType SearchTable(const NumberType _index) const;
    /*
        Search the table.Will search the table over again if the index is bigger then the table size.
    */
    NODISCARD FORCEINLINE constexpr const NumberType LoopSearchTable(const NumberType _index) const;
    /*
        Sereah the table with a linear compensation.
    */
    NODISCARD FORCEINLINE constexpr const NumberType LinearSearchTable(const NumberType _index) const;
    /*
        Sereah the table with a linear compensation.
        Will search the table over again if the index is bigger then the table size.
    */
    NODISCARD FORCEINLINE constexpr const NumberType LoopLinearSearchTable(const NumberType _index) const;

protected:
    using SuperType = TFixedArray<NumberType, kTableSize>;

private:
    //The offset of the index.
    NumberType index_offset_;
    //The distance between the two number. 
    NumberType step_distance_reciprocal_;
};

template<typename NumberType, SizeType kTableSize>
template<typename InitFunction, typename... ArgsType>
FORCEINLINE constexpr ZInterpolationTable<NumberType, kTableSize>::ZInterpolationTable(
    const NumberType _index_offset, const NumberType _step_distance,
    InitFunction&& _init_func, ArgsType&&... _args)
        : SuperType()
        , index_offset_(_index_offset)
        , step_distance_reciprocal_(static_cast<NumberType>(1.0) / _step_distance) {
    _init_func(this, std::forward<ArgsType>(_args)...);
}

template<typename NumberType, SizeType kTableSize>
NODISCARD FORCEINLINE constexpr const NumberType ZInterpolationTable<NumberType, kTableSize>::SearchTable(
        const NumberType _index) const {
    return (*this)(static_cast<SizeType>(_index * step_distance_reciprocal_ + 0.5));
}

template<typename NumberType, SizeType kTableSize>
NODISCARD FORCEINLINE constexpr const NumberType ZInterpolationTable<NumberType, kTableSize>::LoopSearchTable(
        const NumberType _index) const {
    return (*this)(static_cast<SizeType>(_index * step_distance_reciprocal_ + 0.5) % SuperType::size());
}

template<typename NumberType, SizeType kTableSize>
NODISCARD FORCEINLINE constexpr const NumberType ZInterpolationTable<NumberType, kTableSize>::LinearSearchTable(
        const NumberType _index) const {
    NumberType table_index = (_index - index_offset_) * step_distance_reciprocal_;
    SizeType array_index = static_cast<SizeType>(table_index);
    return (*this)(array_index) + 
        ((*this)(array_index + 1) - (*this)(array_index)) * 
        (table_index - static_cast<NumberType>(array_index));
}

template<typename NumberType, SizeType kTableSize>
NODISCARD FORCEINLINE constexpr const NumberType ZInterpolationTable<NumberType, kTableSize>::LoopLinearSearchTable(
    const NumberType _index) const {
    NumberType table_index = (_index - index_offset_) * step_distance_reciprocal_;
    SizeType int_table_index = static_cast<SizeType>(table_index);
    SizeType array_index = int_table_index % SuperType::GetCapacity();
    return (*this)[array_index] +
        ((*this)[array_index + 1] - (*this)[array_index]) *
        (table_index - static_cast<NumberType>(int_table_index));
}

}//math
}//zengine