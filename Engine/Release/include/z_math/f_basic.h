#ifndef Z_MATH_F_BASIC_H_
#define Z_MATH_F_BASIC_H_

#include "internal/drive.h"

#include "z_core/z_array.h"

namespace zengine {
namespace math {

//TODO(Johnasd4):The other funtions. 

#pragma region triganometric

/*
    Normal version of sin. Use SinA() for a more accurate answer. 
*/
MATH_DLLAPI const Float64 Sin(const Float64 raidian) noexcept;
/*
    Normal version of sin. Use SinA() for a more accurate answer.
*/
MATH_DLLAPI const Float32 Sin(const Float32 raidian) noexcept;
/*
    High accuracy version of sin.
*/
MATH_DLLAPI const Float64 SinA(const Float64 raidian) noexcept;
/*
    High accuracy version of sin.
*/
MATH_DLLAPI const Float32 SinA(const Float32 raidian) noexcept;

/*
    Normal version of cos. Use CosA() for a more accurate answer.
*/
MATH_DLLAPI const Float64 Cos(const Float64 raidian) noexcept;
/*
    Normal version of cos. Use CosA() for a more accurate answer.
*/
MATH_DLLAPI const Float32 Cos(const Float32 raidian) noexcept;
/*
   High accuracy version of cos.
*/
MATH_DLLAPI const Float64 CosA(const Float64 raidian) noexcept;
/*
   High accuracy version of cos.
*/
MATH_DLLAPI const Float32 CosA(const Float32 raidian) noexcept;

#pragma endregion //triganometric

}//math
}//zengine

#endif //!Z_MATH_F_BASIC_H_
