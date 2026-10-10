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
#include "d_type.h"

namespace zengine {
namespace gui {

/**
 * @brief Defines the keyboard key codes.
 *
 * This enumeration maps keyboard keys to specific integer values, covering printable characters
 * (ASCII standards) and function keys. It is used to handle input events within the GUI system.
 */
enum class KeyEnum : Int32 {
    /** @brief Space key. */
    kSpace = 32,
    /** @brief Apostrophe key ('). */
    kApostrophe = 39,
    /** @brief Comma key (,). */
    kComma = 44,
    /** @brief Minus key (-). */
    kMinus = 45,
    /** @brief Period key (.). */
    kPeriod = 46,
    /** @brief Slash key (/). */
    kSlash = 47,
    /** @brief Number key 0. */
    k0 = 48,
    /** @brief Number key 1. */
    k1 = 49,
    /** @brief Number key 2. */
    k2 = 50,
    /** @brief Number key 3. */
    k3 = 51,
    /** @brief Number key 4. */
    k4 = 52,
    /** @brief Number key 5. */
    k5 = 53,
    /** @brief Number key 6. */
    k6 = 54,
    /** @brief Number key 7. */
    k7 = 55,
    /** @brief Number key 8. */
    k8 = 56,
    /** @brief Number key 9. */
    k9 = 57,
    /** @brief Semicolon key (;). */
    kSemicolon = 59,
    /** @brief Equal key (=). */
    kEqual = 61,
    /** @brief Letter key A. */
    kA = 65,
    /** @brief Letter key B. */
    kB = 66,
    /** @brief Letter key C. */
    kC = 67,
    /** @brief Letter key D. */
    kD = 68,
    /** @brief Letter key E. */
    kE = 69,
    /** @brief Letter key F. */
    kF = 70,
    /** @brief Letter key G. */
    kG = 71,
    /** @brief Letter key H. */
    kH = 72,
    /** @brief Letter key I. */
    kI = 73,
    /** @brief Letter key J. */
    kJ = 74,
    /** @brief Letter key K. */
    kK = 75,
    /** @brief Letter key L. */
    kL = 76,
    /** @brief Letter key M. */
    kM = 77,
    /** @brief Letter key N. */
    kN = 78,
    /** @brief Letter key O. */
    kO = 79,
    /** @brief Letter key P. */
    kP = 80,
    /** @brief Letter key Q. */
    kQ = 81,
    /** @brief Letter key R. */
    kR = 82,
    /** @brief Letter key S. */
    kS = 83,
    /** @brief Letter key T. */
    kT = 84,
    /** @brief Letter key U. */
    kU = 85,
    /** @brief Letter key V. */
    kV = 86,
    /** @brief Letter key W. */
    kW = 87,
    /** @brief Letter key X. */
    kX = 88,
    /** @brief Letter key Y. */
    kY = 89,
    /** @brief Letter key Z. */
    kZ = 90,
    /** @brief Left bracket key ([). */
    kLeftBracket = 91,
    /** @brief Backslash key (\). */
    kBackSlash = 92,
    /** @brief Right bracket key (]). */
    kRightBracket = 93,
    /** @brief Grave accent key (`). */
    kGraveAccent = 96,
    /** @brief Escape key. */
    kEscape = 256,
    /** @brief Enter/Return key. */
    kEnter = 257,
    /** @brief Tab key. */
    kTab = 258,
    /** @brief Backspace key. */
    kBackspace = 259,
    /** @brief Insert key. */
    kInsert = 260,
    /** @brief Delete key. */
    kDelete = 261,
    /** @brief Right arrow key. */
    kRight = 262,
    /** @brief Left arrow key. */
    kLeft = 263,
    /** @brief Down arrow key. */
    kDown = 264,
    /** @brief Up arrow key. */
    kUp = 265,
    /** @brief Page Up key. */
    kPageUp = 266,
    /** @brief Page Down key. */
    kPageDown = 267,
    /** @brief Home key. */
    kHome = 268,
    /** @brief End key. */
    kEnd = 269,
    /** @brief Caps Lock key. */
    kCapsLock = 280,
    /** @brief Scroll Lock key. */
    kScrollLock = 281,
    /** @brief Num Lock key. */
    kNumLock = 282,
    /** @brief Print Screen key. */
    kPrintScreen = 283,
    /** @brief Pause key. */
    kPause = 284,
    /** @brief Function key F1. */
    kF1 = 290,
    /** @brief Function key F2. */
    kF2 = 291,
    /** @brief Function key F3. */
    kF3 = 292,
    /** @brief Function key F4. */
    kF4 = 293,
    /** @brief Function key F5. */
    kF5 = 294,
    /** @brief Function key F6. */
    kF6 = 295,
    /** @brief Function key F7. */
    kF7 = 296,
    /** @brief Function key F8. */
    kF8 = 297,
    /** @brief Function key F9. */
    kF9 = 298,
    /** @brief Function key F10. */
    kF10 = 299,
    /** @brief Function key F11. */
    kF11 = 300,
    /** @brief Function key F12. */
    kF12 = 301,
    /** @brief Function key F13. */
    kF13 = 302,
    /** @brief Function key F14. */
    kF14 = 303,
    /** @brief Function key F15. */
    kF15 = 304,
    /** @brief Function key F16. */
    kF16 = 305,
    /** @brief Function key F17. */
    kF17 = 306,
    /** @brief Function key F18. */
    kF18 = 307,
    /** @brief Function key F19. */
    kF19 = 308,
    /** @brief Function key F20. */
    kF20 = 309,
    /** @brief Function key F21. */
    kF21 = 310,
    /** @brief Function key F22. */
    kF22 = 311,
    /** @brief Function key F23. */
    kF23 = 312,
    /** @brief Function key F24. */
    kF24 = 313,
    /** @brief Function key F25. */
    kF25 = 314,
    /** @brief Keypad key 0. */
    kKP0 = 320,
    /** @brief Keypad key 1. */
    kKP1 = 321,
    /** @brief Keypad key 2. */
    kKP2 = 322,
    /** @brief Keypad key 3. */
    kKP3 = 323,
    /** @brief Keypad key 4. */
    kKP4 = 324,
    /** @brief Keypad key 5. */
    kKP5 = 325,
    /** @brief Keypad key 6. */
    kKP6 = 326,
    /** @brief Keypad key 7. */
    kKP7 = 327,
    /** @brief Keypad key 8. */
    kKP8 = 328,
    /** @brief Keypad key 9. */
    kKP9 = 329,
    /** @brief Keypad Decimal key. */
    kKPDecimal = 330,
    /** @brief Keypad Divide key. */
    kKPDivide = 331,
    /** @brief Keypad Multiply key. */
    kKPMultiply = 332,
    /** @brief Keypad Subtract key. */
    kKPSubtract = 333,
    /** @brief Keypad Add key. */
    kKPAdd = 334,
    /** @brief Keypad Enter key. */
    kKPEnter = 335,
    /** @brief Keypad Equal key. */
    kKPEqual = 336,
    /** @brief Left Shift key. */
    kLeftShift = 340,
    /** @brief Left Control key. */
    kLeftControl = 341,
    /** @brief Left Alt key. */
    kLeftAlt = 342,
    /** @brief Left Super key (Windows/Command key). */
    kLeftSuper = 343,
    /** @brief Right Shift key. */
    kRightShift = 344,
    /** @brief Right Control key. */
    kRightControl = 345,
    /** @brief Right Alt key. */
    kRightAlt = 346,
    /** @brief Right Super key (Windows/Command key). */
    kRightSuper = 347,
    /** @brief Menu key. */
    kMenu = 348
};

/**
 * @brief Defines the mouse button codes.
 *
 * This enumeration identifies the physical buttons on a mouse device, including the standard
 * left, right, and middle (scroll) buttons.
 */
enum class MouseButtonEnum : Int32 {
    /** @brief Left mouse button. */
    kLeft = 0,
    /** @brief Right mouse button. */
    kRight = 1,
    /** @brief Middle mouse button (often the scroll wheel click). */
    kScroll = 2
};

/** @brief The minimum valid value for an RGBA color component (0.0f). */
inline constexpr Float32 kRGBAMin = 0.0f;
/** @brief The maximum valid value for an RGBA color component (1.0f). */
inline constexpr Float32 kRGBAMax = 1.0f;

} // gui
} // zengine