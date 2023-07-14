#ifndef Z_CORE_Z_LOOKUP_TABLE_H_
#define Z_CORE_Z_LOOKUP_TABLE_H_

#include "internal/drive.h"

namespace zengine {

template<typename ObjectType, IndexType kTableSize>
class ZLookupTable {
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
                (*table_ptr)[index] = 1.0F;
            }
        };
        constexpr ZLookupTable<Int32, 10, true> test(init_function);
    */
    template<typename InitFunction, typename... ArgsType>
    FORCEINLINE constexpr ZLookupTable(InitFunction&& init_function, ArgsType&&... args) {
        init_function(this, std::forward<ArgsType>(args)...);
    }

    NODISCARD FORCEINLINE constexpr ObjectType& operator[](const IndexType index) {
        DEBUG(index < 0 || index >= kTableSize, "Index out of bounds!");
        return this->data_[index];
    }
    NODISCARD FORCEINLINE constexpr const ObjectType& operator[](const IndexType index) const {
        DEBUG(index < 0 || index >= kTableSize, "Index out of bounds!");
        return this->data_[index];
    }

    NODISCARD FORCEINLINE static constexpr const IndexType size() { return kTableSize; }

    /*
        Find the object at the certain index.
    */
    NODISCARD FORCEINLINE constexpr const ObjectType& At(const IndexType index) const {
        DEBUG(index < 0 || index >= kTableSize, "Index out of bounds!");
        return data_[index];
    }
    /*
        Find the object at the certain index.
        Will search the table over again if the index is bigger then the table size.
    */
    NODISCARD FORCEINLINE constexpr const ObjectType& LoopAt(const IndexType index) const {
        return data_[index % kTableSize];
    }

private:
    ObjectType data_[kTableSize];
};

}//zengine


#endif //!Z_CORE_Z_LOOKUP_TABLE_H_