#define MATH_DLLFILE

#include "f_basic.h"

#include "basic/f_triganometric.h"

namespace zengine {
namespace math {

//TODD(Johnasd4):The rest of the functions.

#pragma region numeral

/*
    Return the mantissa of the float number.
*/
MATH_DLLAPI const Int32 FloatMantissa(const Float64 number) noexcept { return FloatMantissa<Float64>(number); }
/*
    Return the mantissa of the float number. 
*/
MATH_DLLAPI const Int32 FloatMantissa(const Float32 number) noexcept { return FloatMantissa<Float32>(number); }

/*
    Return the exponent of the float number.
*/
MATH_DLLAPI const Float64 FloatExponent(const Float64 number) noexcept { return FloatExponent<Float64>(number); }
/*
    Return the exponent of the float number.
*/
MATH_DLLAPI const Float32 FloatExponent(const Float32 number) noexcept { return FloatExponent<Float32>(number); }

/*
    Return a NaN value.
*/
MATH_DLLAPI const Float64 NaN64() noexcept { return NaN<Float64>(); }
/*
    Return a NaN value.
*/
MATH_DLLAPI const Float32 NaN32() noexcept { return NaN<Float32>(); }

/*
    Returns inf.
*/
MATH_DLLAPI const Float64 InfP64() noexcept { return InfP<Float64>(); }
/*
    Returns inf.
*/
MATH_DLLAPI const Float32 InfP32() noexcept { return InfP<Float32>(); }

/*
    Returns -inf.
*/
MATH_DLLAPI const Float64 InfN64() noexcept { return InfN<Float64>(); }
/*
    Returns -inf.
*/
MATH_DLLAPI const Float32 InfN32() noexcept { return InfN<Float32>(); }

/*
    If number is NaN or inf or -inf.
*/
MATH_DLLAPI const Float64 IsInvalid(const Float64 number) noexcept { return IsInvalid<Float64>(number); }
/*
    If number is NaN or inf or -inf.
*/
MATH_DLLAPI const Float32 IsInvalid(const Float32 number) noexcept { return IsInvalid<Float32>(number); }

/*
    If number is NaN.
*/
MATH_DLLAPI const Float64 IsNaN(const Float64 number) noexcept { return IsNaN<Float64>(number); }
/*
    If number is NaN.
*/
MATH_DLLAPI const Float32 IsNaN(const Float32 number) noexcept { return IsNaN<Float32>(number); }

/*
    If number is inf or -inf.
*/
MATH_DLLAPI const Float64 IsInf(const Float64 number) noexcept { return IsInf<Float64>(number); }
/*
    If number is inf or -inf.
*/
MATH_DLLAPI const Float32 IsInf(const Float32 number) noexcept { return IsInf<Float32>(number); }

/*
    If number is inf.
*/
MATH_DLLAPI const Float64 IsInfP(const Float64 number) noexcept { return IsInfP<Float64>(number); }
/*
    If number is inf.
*/
MATH_DLLAPI const Float32 IsInfP(const Float32 number) noexcept { return IsInfP<Float32>(number); }

/*
    If number is -inf.
*/
MATH_DLLAPI const Float64 IsInfN(const Float64 number) noexcept { return IsInfN<Float64>(number); }
/*
    If number is -inf.
*/
MATH_DLLAPI const Float32 IsInfN(const Float32 number) noexcept { return IsInfN<Float32>(number); }

/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const Float64 Sgn2State(const Float64 number) noexcept { return Sgn2State<Float64>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const Float32 Sgn2State(const Float32 number) noexcept { return Sgn2State<Float32>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const Int64 Sgn2State(const Int64 number) noexcept { return Sgn2State<Int64>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const Int32 Sgn2State(const Int32 number) noexcept { return Sgn2State<Int32>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const Int16 Sgn2State(const Int16 number) noexcept { return Sgn2State<Int16>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const Int8 Sgn2State(const Int8 number) noexcept { return Sgn2State<Int8>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const UInt64 Sgn2State(const UInt64 number) noexcept { return Sgn2State<UInt64>(number); }
/*
    Returns  1 when number >= 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const UInt32 Sgn2State(const UInt32 number) noexcept { return Sgn2State<UInt32>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const UInt16 Sgn2State(const UInt16 number) noexcept { return Sgn2State<UInt16>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const UInt8 Sgn2State(const UInt8 number) noexcept { return Sgn2State<UInt8>(number); }

/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const Float64 Sgn3State(const Float64 number) noexcept { return Sgn3State<Float64>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const Float32 Sgn3State(const Float32 number) noexcept { return Sgn3State<Float32>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const Int64 Sgn3State(const Int64 number) noexcept { return Sgn3State<Int64>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const Int32 Sgn3State(const Int32 number) noexcept { return Sgn3State<Int32>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const Int16 Sgn3State(const Int16 number) noexcept { return Sgn3State<Int16>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const Int8 Sgn3State(const Int8 number) noexcept { return Sgn3State<Int8>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const UInt64 Sgn3State(const UInt64 number) noexcept { return Sgn3State<UInt64>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const UInt32 Sgn3State(const UInt32 number) noexcept { return Sgn3State<UInt32>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const UInt16 Sgn3State(const UInt16 number) noexcept { return Sgn3State<UInt16>(number); }
/*
    Returns  1 when number > 0.
    Returns  0 when number = 0.
    Returns -1 when number < 0.
*/
MATH_DLLAPI const UInt8 Sgn3State(const UInt8 number) noexcept { return Sgn3State<UInt8>(number); }

/*
    Returns the absolute value of the number
*/
MATH_DLLAPI const Float64 Abs(const Float64 number) noexcept { return Abs<Float64>(number); }
/*
    Returns the absolute value of the number
*/
MATH_DLLAPI const Float32 Abs(const Float32 number) noexcept { return Abs<Float32>(number); }
/*
    Returns the absolute value of the number
*/
MATH_DLLAPI const Int64 Abs(const Int64 number) noexcept { return Abs<Int64>(number); }
/*
    Returns the absolute value of the number
*/
MATH_DLLAPI const Int32 Abs(const Int32 number) noexcept { return Abs<Int32>(number); }
/*
    Returns the absolute value of the number
*/
MATH_DLLAPI const Int16 Abs(const Int16 number) noexcept { return Abs<Int16>(number); }
/*
    Returns the absolute value of the number
*/
MATH_DLLAPI const Int8 Abs(const Int8 number) noexcept { return Abs<Int8>(number); }
/*
    Returns the absolute value of the number
*/
MATH_DLLAPI const UInt64 Abs(const UInt64 number) noexcept { return Abs<UInt64>(number); }
/*
    Returns the absolute value of the number
*/
MATH_DLLAPI const UInt32 Abs(const UInt32 number) noexcept { return Abs<UInt32>(number); }
/*
    Returns the absolute value of the number
*/
MATH_DLLAPI const UInt16 Abs(const UInt16 number) noexcept { return Abs<UInt16>(number); }
/*
    Returns the absolute value of the number
*/
MATH_DLLAPI const UInt8 Abs(const UInt8 number) noexcept { return Abs<UInt8>(number); }

/*
    Returns the absolute value of the number
*/
MATH_DLLAPI const Float64 Max(const Float64 number) noexcept { return Abs<Float64>(number); }


#pragma endregion //numeral

#pragma region triganometric

/*
    Normal version of sin. Use SinA() for a more accurate answer. 
*/
MATH_DLLAPI const Float64 Sin(const Float64 raidian) noexcept { return Sin<Float64>(raidian); }
/*
    Normal version of sin. Use SinA() for a more accurate answer.
*/
MATH_DLLAPI const Float32 Sin(const Float32 raidian) noexcept { return Sin<Float32>(raidian); }
/*
   High accuracy version of sin.
*/
MATH_DLLAPI const Float64 SinA(const Float64 raidian) noexcept { return SinA<Float64>(raidian); }
/*
   High accuracy version of sin.
*/
MATH_DLLAPI const Float32 SinA(const Float32 raidian) noexcept { return SinA<Float32>(raidian); }

/*
    Normal version of cos. Use CosA() for a more accurate answer.
*/
MATH_DLLAPI const Float64 Cos(const Float64 raidian) noexcept { return Cos<Float64>(raidian); }
/*
    Normal version of cos. Use CosA() for a more accurate answer.
*/
MATH_DLLAPI const Float32 Cos(const Float32 raidian) noexcept { return Cos<Float32>(raidian); }
/*
   High accuracy version of cos.
*/
MATH_DLLAPI const Float64 CosA(const Float64 raidian) noexcept { return CosA<Float64>(raidian); }
/*
   High accuracy version of cos.
*/
MATH_DLLAPI const Float32 CosA(const Float32 raidian) noexcept { return CosA<Float32>(raidian); }

#pragma endregion //triganometric

}//math
}//zengine
