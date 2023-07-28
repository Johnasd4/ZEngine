/*
    Copyright (c) YuLin Zhu (������)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (������)
    Contact: 1152325286@qq.com
*/
#ifndef Z_CORE_Z_LOOKUP_TABLE_H_
#define Z_CORE_Z_LOOKUP_TABLE_H_

#include "internal/drive.h"

#include "m_error_message.h"
#include "z_object.h"

namespace zengine {

namespace internal {

template<typename LookupTableType, typename Function, typename... ArgsType>
concept kIsLookupTableInitFunction = requires(LookupTableType * lookup_table, Function function, ArgsType&&... args) {
    function(lookup_table, std::forward<ArgsType>(args)...);
};

}//internal

template<typename ObjectType, IndexType kTableSize>
class ZLookupTable : public ZObject {
public:
    /*
        The work is done at compile time.
        Tne first parameter is added to the index when using the table.
        Tne second parameter is distance between the table values.
        The Constructor's third parameter is the initial funtion of the table.
        The first parameter of the initial funtion must be ZLookupTable*.
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
    requires internal::kIsLookupTableInitFunction<ZLookupTable<ObjectType, kTableSize>,
                                                  InitFunction, ArgsType...>
    FORCEINLINE constexpr ZLookupTable(InitFunction&& init_function, ArgsType&&... args) : SuperType() {
        init_function(this, std::forward<ArgsType>(args)...);
    }

    NODISCARD FORCEINLINE constexpr ObjectType& operator[](IndexType index) {
        DEBUG(index < 0 || index >= kTableSize, "Index out of bounds!");
        return this->data_[index];
    }
    NODISCARD FORCEINLINE constexpr const ObjectType& operator[](IndexType index) const {
        DEBUG(index < 0 || index >= kTableSize, "Index out of bounds!");
        return this->data_[index];
    }

    NODISCARD FORCEINLINE static constexpr IndexType size() { return kTableSize; }

    /*
        Find the object at the certain index.
    */
    NODISCARD FORCEINLINE constexpr const ObjectType& At(IndexType index) const {
        DEBUG(index < 0 || index >= kTableSize, "Index out of bounds!");
        return data_[index];
    }
    /*
        Find the object at the certain index.
        Will search the table over again if the index is bigger then the table size.
    */
    NODISCARD FORCEINLINE constexpr const ObjectType& LoopAt(IndexType index) const {
        DEBUG(index < 0, "Index out of bounds!");
        return data_[index % kTableSize];
    }

protected:
    using SuperType = ZObject;

private:
    ZLookupTable() = delete;
    ZLookupTable(const ZLookupTable&) = delete;
    ZLookupTable(ZLookupTable&&) = delete;

    ZLookupTable& operator=(const ZLookupTable&) = delete;
    ZLookupTable& operator=(ZLookupTable&&) = delete;

    ObjectType data_[kTableSize];
};

}//zengine


#endif //!Z_CORE_Z_LOOKUP_TABLE_H_