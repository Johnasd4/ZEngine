#ifndef Z_MATH_Z_INTERPOLATION_TABLE_H_
#define Z_MATH_Z_INTERPOLATION_TABLE_H_

#include "internal/drive.h"

#include "z_core/z_fixed_array.h"

namespace zengine {
namespace math {

template<typename NumberType, IndexType kTableSize, Bool kIsSimpleTable = false> 
class ZInterpolationTable :public ZFixedArray<NumberType, kTableSize> {
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
        constexpr auto init_function = [](ZInterpolationTable<Float32, 10>* table_ptr) {
            for (IndexType index = 0; index < table_ptr->size(); ++index) {
                (*table_ptr)(index) = 1.0F;
            }
        };
        constexpr ZInterpolationTable<Int32, 10> test(10.0F,10.0F,init_function);
    */
    template<typename InitFunction, typename... ArgsType>
    __forceinline constexpr ZInterpolationTable(const NumberType index_offset, const NumberType step_distance, 
                                                InitFunction&& init_function, ArgsType&&... args);    

    __forceinline constexpr NumberType index_offset() const { return index_offset_; }
    __forceinline constexpr NumberType step_distance() const {
        return static_cast<NumberType>(1.0) / step_distance_reciprocal_; 
    }
    __forceinline constexpr NumberType step_distance_reciprocal() const { return step_distance_reciprocal_; }
    /*
        Find the object at the certain index.
    */
    __forceinline constexpr const NumberType At(const IndexType index) const;
    /*
        Find the object at the certain index. 
        Will search the table over again if the index is bigger then the table size.
    */
    __forceinline constexpr const NumberType LoopAt(const IndexType index) const;
    /*
        Search the table.
    */
    __forceinline constexpr const NumberType SearchTable(const NumberType index) const;
    /*
        Search the table.Will search the table over again if the index is bigger then the table size.
    */
    __forceinline constexpr const NumberType LoopSearchTable(const NumberType index) const;
    /*
        Sereah the table with a linear compensation.
    */
    __forceinline constexpr const NumberType LinearSearchTable(const NumberType index) const;
    /*
        Sereah the table with a linear compensation.
        Will search the table over again if the index is bigger then the table size.
    */
    __forceinline constexpr const NumberType LoopLinearSearchTable(const NumberType index) const;

protected:
    using SuperType = ZFixedArray<NumberType, kTableSize>;

private:
    //The offset of the index.
    NumberType index_offset_;
    //The distance between the two number. 
    NumberType step_distance_reciprocal_;
};

template<typename NumberType, IndexType kTableSize>
class ZInterpolationTable<NumberType, kTableSize, true> : public ZFixedArray<NumberType, kTableSize> {
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
        constexpr auto init_function = [](ZInterpolationTable<Float32, 10, true>* table_ptr) {
            for (IndexType index = 0; index < table_ptr->size(); ++index) {
                (*table_ptr)(index) = 1.0F;
            }
        };
        constexpr ZInterpolationTable<Int32, 10, true> test(init_function);
    */
    template<typename InitFunction, typename... ArgsType>
    __forceinline constexpr ZInterpolationTable(InitFunction&& init_function, ArgsType&&... args);

    /*
        Find the object at the certain index.
    */
    __forceinline constexpr const NumberType At(const IndexType index) const;
    /*
        Find the object at the certain index.
        Will search the table over again if the index is bigger then the table size.
    */
    __forceinline constexpr const NumberType LoopAt(const IndexType index) const;

protected:
    using SuperType = ZFixedArray<NumberType, kTableSize>;
};

template<typename NumberType, IndexType kTableSize, Bool kIsSimpleTable> 
template<typename InitFunction, typename... ArgsType>
__forceinline constexpr ZInterpolationTable<NumberType, kTableSize, kIsSimpleTable>::ZInterpolationTable(
    const NumberType index_offset, const NumberType step_distance, 
    InitFunction&& init_function, ArgsType&&... args)
        : SuperType()
        , index_offset_(index_offset)
        , step_distance_reciprocal_(static_cast<NumberType>(1.0) / step_distance) {
    init_function(this, std::forward<ArgsType>(args)...);
}

template<typename NumberType, IndexType kTableSize>
template<typename InitFunction, typename... ArgsType>
__forceinline constexpr ZInterpolationTable<NumberType, kTableSize, true>::ZInterpolationTable(
        InitFunction&& init_function, ArgsType&&... args)
        : SuperType() {
    init_function(this, std::forward<ArgsType>(args)...);
}

template<typename NumberType, IndexType kTableSize>
__forceinline constexpr const NumberType ZInterpolationTable<NumberType, kTableSize, true>::At(
        const IndexType index) const {
    return (*this)(index);
}

template<typename NumberType, IndexType kTableSize>
__forceinline constexpr const NumberType ZInterpolationTable<NumberType, kTableSize, true>::LoopAt(
        const IndexType index) const {
    return (*this)(index % SuperType::size());
}

template<typename NumberType, IndexType kTableSize, Bool kIsSimpleTable> 
__forceinline constexpr const NumberType ZInterpolationTable<NumberType, kTableSize, kIsSimpleTable>::SearchTable(
        const NumberType index) const {
    return (*this)(static_cast<IndexType>(index * step_distance_reciprocal_));
}

template<typename NumberType, IndexType kTableSize, Bool kIsSimpleTable> 
__forceinline constexpr const NumberType ZInterpolationTable<NumberType, kTableSize, kIsSimpleTable>::LoopSearchTable(
        const NumberType index) const {
    return (*this)(static_cast<IndexType>(index * step_distance_reciprocal_) % SuperType::size());
}

template<typename NumberType, IndexType kTableSize, Bool kIsSimpleTable> 
__forceinline constexpr const NumberType ZInterpolationTable<NumberType, kTableSize, kIsSimpleTable>::LinearSearchTable(
        const NumberType index) const {
    NumberType table_index = (index - index_offset_) * step_distance_reciprocal_;
    IndexType array_index = static_cast<IndexType>(table_index);
    return (*this)(array_index) + 
        ((*this)(array_index + 1) - (*this)(array_index)) * 
        (table_index - static_cast<NumberType>(array_index));
}

template<typename NumberType, IndexType kTableSize, Bool kIsSimpleTable> 
__forceinline constexpr const NumberType ZInterpolationTable<NumberType, kTableSize, kIsSimpleTable>::LoopLinearSearchTable(
    const NumberType index) const {
    NumberType table_index = (index - index_offset_) * step_distance_reciprocal_;
    IndexType int_table_index = static_cast<IndexType>(table_index);
    IndexType array_index = int_table_index % SuperType::size();
    return (*this)(array_index) +
        ((*this)(array_index + 1) - (*this)(array_index)) *
        (table_index - static_cast<NumberType>(int_table_index));
}

}//math
}//zengine


#endif //!Z_MATH_Z_INTERPOLATION_TABLE_H_