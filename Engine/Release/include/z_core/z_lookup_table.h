#ifndef Z_CORE_Z_LOOKUP_TABLE_H_
#define Z_CORE_Z_LOOKUP_TABLE_H_

#include "internal/drive.h"

#include "z_core/z_fixed_array.h"

namespace zengine {

template<typename ObjectType, IndexType kTableSize>
class ZLookupTable : public ZFixedArray<ObjectType, kTableSize> {
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
        constexpr auto init_function = [](ZLookupTable<Float32, 10, true>* table_ptr) {
            for (IndexType index = 0; index < table_ptr->size(); ++index) {
                (*table_ptr)(index) = 1.0F;
            }
        };
        constexpr ZLookupTable<Int32, 10, true> test(init_function);
    */
    template<typename InitFunction, typename... ArgsType>
    FORCEINLINE constexpr ZLookupTable(InitFunction&& init_function, ArgsType&&... args);

    /*
        Find the object at the certain index.
    */
    FORCEINLINE constexpr const ObjectType At(const IndexType index) const;
    /*
        Find the object at the certain index.
        Will search the table over again if the index is bigger then the table size.
    */
    FORCEINLINE constexpr const ObjectType LoopAt(const IndexType index) const;

protected:
    using SuperType = ZFixedArray<ObjectType, kTableSize>;
};

template<typename ObjectType, IndexType kTableSize>
template<typename InitFunction, typename... ArgsType>
FORCEINLINE constexpr ZLookupTable<ObjectType, kTableSize>::ZLookupTable(InitFunction&& init_function, 
                                                                           ArgsType&&... args)
        : SuperType() {
    init_function(this, std::forward<ArgsType>(args)...);
}

template<typename ObjectType, IndexType kTableSize>
FORCEINLINE constexpr const ObjectType ZLookupTable<ObjectType, kTableSize>::At(
        const IndexType index) const {
    return (*this)(index);
}

template<typename ObjectType, IndexType kTableSize>
FORCEINLINE constexpr const ObjectType ZLookupTable<ObjectType, kTableSize>::LoopAt(
        const IndexType index) const {
    return (*this)(index % SuperType::size());
}

}//zengine


#endif //!Z_CORE_Z_LOOKUP_TABLE_H_