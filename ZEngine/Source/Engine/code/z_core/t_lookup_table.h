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
#ifndef Z_CORE_Z_LOOKUP_TABLE_H_
#define Z_CORE_Z_LOOKUP_TABLE_H_

#include "internal/z_drive.h"

#include "m_log.h"
#include "z_object.h"

namespace zengine {

namespace internal {

template<typename _LookupTableType, typename _Function, typename... _ArgsType>
concept kIsLookupTableInitFunction = requires(_LookupTableType * _lookup_table, _Function _func, _ArgsType&&... _args) {
    _func(_lookup_table, std::forward<_ArgsType>(_args)...);
};

}//internal

template<typename _ObjectType, IndexType kTableSize>
class TLookupTable : public ZObject {
public:
    /*
        The work is done at compile time.
        Tne first parameter is added to the index when using the table.
        Tne second parameter is distance between the table values.
        The Constructor's third parameter is the initial funtion of the table.
        The first parameter of the initial funtion must be TLookupTable*.
        Constructor Template Parameters:
        - InitFunction: The function type that initial the array.
        - ArgsType...: The parameters type.
        Constructor Parameters:
        - init_function: The function to initial the array.
        - ArgsType...: The parameters of the function except for the fisrt.
        Example:
        constexpr auto init_function = [](TLookupTable<Float32, 10, true>* table_ptr) {
            for (IndexType index = 0; index < table_ptr->size(); ++index) {
                (*table_ptr)[index] = 1.0F;
            }
        };
        constexpr TLookupTable<Int32, 10, true> test(init_function);
    */
    template<typename _InitFunction, typename... _ArgsType>
    requires internal::kIsLookupTableInitFunction<TLookupTable<_ObjectType, kTableSize>, _InitFunction, _ArgsType...>
    FORCEINLINE constexpr TLookupTable(_InitFunction&& _init_func, _ArgsType&&... _args) : SuperType_() {
        _init_func(this, std::forward<_ArgsType>(_args)...);
    }

    NODISCARD FORCEINLINE constexpr _ObjectType& operator[](IndexType _index) { return data_[_index]; }
    NODISCARD FORCEINLINE constexpr const _ObjectType& operator[](IndexType _index) const { return data_[_index]; }

    NODISCARD FORCEINLINE static constexpr IndexType Size() { return kTableSize; }

    /*
        Find the object at the certain index.
    */
    NODISCARD FORCEINLINE constexpr _ObjectType& At(IndexType _index) { return data_[_index]; }
    NODISCARD FORCEINLINE constexpr const _ObjectType& At(IndexType _index) const { return data_[_index]; }
    /*
        Find the object at the certain index.
        Will search the table over again if the index is bigger then the table size.
    */
    NODISCARD FORCEINLINE constexpr const _ObjectType& LoopAt(IndexType _index) const { 
        return data_[_index % kTableSize]; 
    }

protected:
    using SuperType_ = ZObject;

private:
    TLookupTable() = delete;
    TLookupTable(const TLookupTable&) = delete;
    TLookupTable(TLookupTable&&) = delete;

    TLookupTable& operator=(const TLookupTable&) = delete;
    TLookupTable& operator=(TLookupTable&&) = delete;

    _ObjectType data_[kTableSize];
};

}//zengine


#endif //!Z_CORE_Z_LOOKUP_TABLE_H_