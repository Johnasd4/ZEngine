/*
  Copyright 2023 YuLin Zhu
  Email:1152325286@qq.com

  Licensed under the Creative Commons Attribution-NonCommercial 4.0 International License (CC BY-NC 4.0).
  You may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      https://creativecommons.org/licenses/by-nc/4.0/

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#ifndef Z_CORE_INTERNAL_DRIVE_H_
#define Z_CORE_INTERNAL_DRIVE_H_

//Ignores the safety io function warning.
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

//Removes the part that conflicts with Winsock.h and Winsock2.h.
#define WIN32_LEAN_AND_MEAN

#include<iostream>
#include<windows.h>

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif
#ifndef DLLIMPORT
#define DLLIMPORT __declspec(dllimport)
#endif

#ifndef NODISCARD
#define NODISCARD [[nodiscard]]
#endif

//Define if at the start of the z_math cpp file.
#ifdef CORE_DLLFILE
#define CORE_DLLAPI DLLEXPORT
#else
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"ZCore_x64_Debug.lib")
#else
#pragma comment(lib,"ZCore_x64_Release.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib,"ZCore_Win32_Debug.lib")
#else
#pragma comment(lib,"ZCore_Win32_Release.lib")
#endif
#endif
#define CORE_DLLAPI DLLIMPORT
#endif

namespace zengine {

#pragma region basic type

using CChar = char;
using TChar = wchar_t;

//range£º-128~127£¨3 digits£©
using Int8 = char;
//range£º-3,2768~3,2767£¨5 digits£©
using Int16 = short;
//range£º-21,4748,3648~21,4748,3647£¨10 digits£©
using Int32 = int;
//range£º-922,3372,0368,5477,5808~922,3372,0368,5477,5807£¨19 digits£©
using Int64 = long long;

//range£º0~255£¨3 digits£©
using UInt8 = unsigned char;
//range£º0~6,5535£¨5 digits£©
using UInt16 = unsigned short;
//range£º0~42,9496,7295£¨10 digits£©
using UInt32 = unsigned int;
//range£º0~1844,6744,0737,0955,1616£¨20 digits£©
using UInt64 = unsigned long long;

//range£º-3.40E+38 ~ +3.40E+38
using Float32 = float;
//range£º-1.79E+308 ~ +1.79E+308
using Float64 = double;

//The type of the max array length.
using SizeType = size_t;
//The type of the address when translated to number.
using AddressType = SizeType;
//The type of the memory size.
using MemoryType = UInt32;
//The type of the index.
using IndexType = Int32;

using Bool = bool;
using Byte = unsigned char;
using Void = void;
using Address = void*;
using Handle = void*;

#pragma endregion basic type

#pragma region basic union

union Size32Type {
    FORCEINLINE constexpr Size32Type(const Int32 int_32) : int_32(int_32) {}
    FORCEINLINE constexpr Size32Type(const UInt32 u_int_32) : u_int_32(u_int_32) {}
    FORCEINLINE constexpr Size32Type(const Float32 float_32) : float_32(float_32) {}

    Int32 int_32;
    UInt32 u_int_32;
    Float32 float_32;
};

union Size64Type {
    FORCEINLINE constexpr Size64Type(const Int64 int_64) : int_64(int_64) {}
    FORCEINLINE constexpr Size64Type(const UInt64 u_int_64) : u_int_64(u_int_64) {}
    FORCEINLINE constexpr Size64Type(const Float64 float_64) : float_64(float_64) {}

    Int64 int_64;
    UInt64 u_int_64;
    Float64 float_64;
};

#pragma endregion basic union

#pragma region basic function

template<typename ObjectType>
FORCEINLINE constexpr Void Swap(ObjectType* object_1, ObjectType* object_2) {
    ObjectType temp_object = std::move(*object_1);
    *object_1 = std::move(*object_2);
    *object_2 = std::move(temp_object);
}

#pragma endregion basic function

#pragma region basic concept

template<typename ObjectType1,typename ObjectType2>
concept kSameType = std::is_same_v<ObjectType1, ObjectType2>;

template<typename ObjectType>
concept kIsClass = std::is_class_v<ObjectType>;

template<typename CharType>
concept kIsChar = kSameType<CharType, CChar> || kSameType<CharType, TChar>;


template<typename Function, typename ObjectType>
concept kIsCompareFunction = requires(Function function, ObjectType object) {
    { function(object, object) } -> kSameType<Bool>;
};

#pragma endregion basic concept

#pragma region basic constant

constexpr Float32 kFloat32Max = 3.402823466e+38F;
constexpr Float32 kFloat32Min = -3.402823466e+38F;

constexpr MemoryType kB = 1;
constexpr MemoryType kKB = 1024;
constexpr MemoryType kMB = 1048576;
constexpr MemoryType kGB = 1073741824;

//The unit size when applying memory.
constexpr MemoryType kHeapMemoryUnitSize = 4 * kKB;

#pragma endregion basic constant

}//zengine

#endif // !Z_CORE_INTERNAL_DRIVE_H_