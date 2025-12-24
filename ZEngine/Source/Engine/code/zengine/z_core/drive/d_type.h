/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "d_lib.h"

namespace zengine {

/** @brief Standard character type. */
using Char = char;
/** @brief Wide character type. */
using WChar = wchar_t;

/** @brief 8-bit signed integer type. Range: -128 to 127. */
using Int8 = int8_t;
/** @brief 16-bit signed integer type. Range: -32,768 to 32,767. */
using Int16 = int16_t;
/** @brief 32-bit signed integer type. Range: -2,147,483,648 to 2,147,483,647. */
using Int32 = int32_t;
/** @brief 64-bit signed integer type. Range: -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807. */
using Int64 = int64_t;

/** @brief 8-bit unsigned integer type. Range: 0 to 255. */
using UInt8 = uint8_t;
/** @brief 16-bit unsigned integer type. Range: 0 to 65,535. */
using UInt16 = uint16_t;
/** @brief 32-bit unsigned integer type. Range: 0 to 4,294,967,295. */
using UInt32 = uint32_t;
/** @brief 64-bit unsigned integer type. Range: 0 to 18,446,744,073,709,551,615. */
using UInt64 = uint64_t;

/** @brief 32-bit floating point type. Range: +/- 3.40E+38. */
using Float32 = float;
/** @brief 64-bit floating point type. Range: +/- 1.79E+308. */
using Float64 = double;

/** @brief Unsigned integer type for representing sizes. Same as size_t. */
using SizeType = size_t;
/** @brief Unsigned integer type capable of holding a pointer value. */
using PointerType = SizeType;
/** 
 * @brief Return type code. Returns kOK if successful, otherwise returns an error code. 
 * Format: 0xSPPTTTEE (S: Solution, P: Project, T: Type, E: Error). 
 */
using ReturnType = UInt32;
/** @brief Integer type used for representing time values. */
using TimeType = Int64;
/** @brief Type for handling variable argument lists. */
using ArgListType = va_list;
/** @brief Type of the null pointer literal. */
using NullptrType = std::nullptr_t;

/** @brief Boolean type. */
using Bool = bool;
/** @brief Unsigned byte type. */
using Byte = unsigned char;
/** @brief Void type. */
using Void = void;
/** @brief Generic handle type (void pointer). */
using Handle = void*;


#pragma warning(disable : 26495)
/** @brief Structure representing a color with Red, Green, Blue, and Alpha components. */
struct Colour {
public:
    /** @brief Default constructor. Initializes to Opaque Black (R=0, G=0, B=0, A=255). */
    FORCEINLINE Colour() noexcept
        : red_()
        , green_()
        , blue_()
        , alpha_(255)
    {}

    /** 
     * @brief Constructor initializing from a packed 32-bit integer. 
     * @param _colour The packed 32-bit integer color value. 
     */
    FORCEINLINE Colour(
        Int32 _colour
    ) noexcept {
        *reinterpret_cast<Int32*>(this) = _colour;
    }

    /** 
     * @brief Constructor initializing from RGB values. Alpha is set to 255. 
     * @param _red The red component (0-255). 
     * @param _green The green component (0-255). 
     * @param _blue The blue component (0-255). 
     */
    FORCEINLINE constexpr Colour(
        UInt8 _red,
        UInt8 _green,
        UInt8 _blue
    ) noexcept
        : red_(_red)
        , green_(_green)
        , blue_(_blue)
        , alpha_(255)
    {}

    /** 
     * @brief Constructor initializing from RGBA values. 
     * @param _red The red component (0-255). 
     * @param _green The green component (0-255). 
     * @param _blue The blue component (0-255). 
     * @param _alpha The alpha component (0-255). 
     */
    FORCEINLINE constexpr Colour(
        UInt8 _red,
        UInt8 _green,
        UInt8 _blue,
        UInt8 _alpha
    ) noexcept
        : red_(_red)
        , green_(_green)
        , blue_(_blue)
        , alpha_(_alpha)
    {}

    /** 
     * @brief Assignment operator. Copies the packed 32-bit color value. 
     * @param _buffer The source Colour object. 
     * @return Reference to this object. 
     */
    FORCEINLINE Colour& operator=(const Colour& _buffer) noexcept {
        *reinterpret_cast<Int32*>(this) = *reinterpret_cast<const Int32*>(&_buffer);
        return *this;
    }

    /** 
     * @brief Equality operator. Compares the packed 32-bit color values. 
     * @param _buffer The Colour object to compare with. 
     * @return True if colors are identical, false otherwise. 
     */
    FORCEINLINE Bool operator==(const Colour& _buffer) noexcept {        
        return *reinterpret_cast<Int32*>(this) == *reinterpret_cast<const Int32*>(&_buffer);
    }

private:
    /** @brief Deleted operator new to prevent heap allocation. */
    static Void* operator new(SizeType) = delete;
    /** @brief Deleted operator delete. */
    static Void operator delete(Void*) = delete;

public:
    /** @brief The red component of the color. */
    UInt8 red_;
    /** @brief The green component of the color. */
    UInt8 green_;
    /** @brief The blue component of the color. */
    UInt8 blue_;
    /** @brief The alpha (transparency) component of the color. */
    UInt8 alpha_;
};
#pragma warning(default : 26495)

/** @brief Union that can store a 32-bit integer, unsigned integer, or floating-point value. */
union Size32Union {
    /** @brief Constructor initializing with a 32-bit signed integer. */
    FORCEINLINE constexpr Size32Union(const Int32 _int_32) : int_32_(_int_32) {}
    /** @brief Constructor initializing with a 32-bit unsigned integer. */
    FORCEINLINE constexpr Size32Union(const UInt32 _u_int_32) : u_int_32_(_u_int_32) {}
    /** @brief Constructor initializing with a 32-bit float. */
    FORCEINLINE constexpr Size32Union(const Float32 _float_32) : float_32_(_float_32) {}

    /** @brief The value as a 32-bit signed integer. */
    Int32 int_32_;
    /** @brief The value as a 32-bit unsigned integer. */
    UInt32 u_int_32_;
    /** @brief The value as a 32-bit float. */
    Float32 float_32_;
};

/** @brief Union that can store a 64-bit integer, unsigned integer, or floating-point value. */
union Size64Union {
    /** @brief Constructor initializing with a 64-bit signed integer. */
    FORCEINLINE constexpr Size64Union(const Int64 _int_64) : int_64_(_int_64) {}
    /** @brief Constructor initializing with a 64-bit unsigned integer. */
    FORCEINLINE constexpr Size64Union(const UInt64 _u_int_64) : u_int_64_(_u_int_64) {}
    /** @brief Constructor initializing with a 64-bit float. */
    FORCEINLINE constexpr Size64Union(const Float64 _float_64) : float_64_(_float_64) {}

    /** @brief The value as a 64-bit signed integer. */
    Int64 int_64_;
    /** @brief The value as a 64-bit unsigned integer. */
    UInt64 u_int_64_;
    /** @brief The value as a 64-bit float. */
    Float64 float_64_;
};

/** @brief Enumeration to distinguish between narrow strings and wide strings. */
enum class StringEnum : Int32 {
    /** @brief Indicates a standard narrow string (char). */
    kString,
    /** @brief Indicates a wide string (wchar_t). */
    kWString
};

}//zengine