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
#ifndef Z_CORE_INTERNAL_D_TYPE_H_
#define Z_CORE_INTERNAL_D_TYPE_H_

#include "d_lib.h"

namespace zengine {

using CChar = char;
using TChar = wchar_t;

//range£∫-128~127£®3 digits£©
using Int8 = char;
//range£∫-3,2768~3,2767£®5 digits£©
using Int16 = short;
//range£∫-21,4748,3648~21,4748,3647£®10 digits£©
using Int32 = int;
//range£∫-922,3372,0368,5477,5808~922,3372,0368,5477,5807£®19 digits£©
using Int64 = long long;

//range£∫0~255£®3 digits£©
using UInt8 = unsigned char;
//range£∫0~6,5535£®5 digits£©
using UInt16 = unsigned short;
//range£∫0~42,9496,7295£®10 digits£©
using UInt32 = unsigned int;
//range£∫0~1844,6744,0737,0955,1616£®20 digits£©
using UInt64 = unsigned long long;

//range£∫-3.40E+38 ~ +3.40E+38
using Float32 = float;
//range£∫-1.79E+308 ~ +1.79E+308
using Float64 = double;

//The type of the max array length.
using SizeType = size_t;
//The type of the pointer when translated to number.
using PointerType = SizeType;
//The type of the memory size.
using MemoryType = UInt32;
//The type of the index.
using IndexType = Int32;
//The type of return value.
using ReturnType = UInt32;
//The type of time.
using TimeType = time_t;
//The type of time.
using ArgListType = va_list;

using Bool = bool;
using Byte = unsigned char;
using Void = void;
using Handle = void*;

union Size32Union {
    FORCEINLINE constexpr Size32Union(const Int32 int_32) : int_32(int_32) {}
    FORCEINLINE constexpr Size32Union(const UInt32 u_int_32) : u_int_32(u_int_32) {}
    FORCEINLINE constexpr Size32Union(const Float32 float_32) : float_32(float_32) {}

    Int32 int_32;
    UInt32 u_int_32;
    Float32 float_32;
};

union Size64Union {
    FORCEINLINE constexpr Size64Union(const Int64 int_64) : int_64(int_64) {}
    FORCEINLINE constexpr Size64Union(const UInt64 u_int_64) : u_int_64(u_int_64) {}
    FORCEINLINE constexpr Size64Union(const Float64 float_64) : float_64(float_64) {}

    Int64 int_64;
    UInt64 u_int_64;
    Float64 float_64;
};

enum StringEnum : IndexType{
    kStringMin = 0,
    kStringCStr = kStringMin,
    kStringTStr,
    kStringMax

};

}//zengine

#endif // !Z_CORE_INTERNAL_D_TYPE_H_