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
#ifndef Z_GUI_INTERNAL_D_CONSTANT_H_
#define Z_GUI_INTERNAL_D_CONSTANT_H_

#include "d_lib.h"
#include "d_type.h"

namespace zengine {
namespace gui {

enum KeyEnum {
    /* Printable keys */
    kKey_Space = 32,
    kKey_Apostrophe = 39,    /* ' */
    kKey_Comma = 44,         /* , */
    kKey_Minus = 45,         /* - */
    kKey_Period = 46,        /* . */
    kKey_Slash = 47,         /* / */
    kKey_0 = 48,
    kKey_1 = 49,
    kKey_2 = 50,
    kKey_3 = 51,
    kKey_4 = 52,
    kKey_5 = 53,
    kKey_6 = 54,
    kKey_7 = 55,
    kKey_8 = 56,
    kKey_9 = 57,
    kKey_Semicolon = 59,     /* ; */
    kKey_Equal = 61,         /* = */
    kKey_A = 65,
    kKey_B = 66,
    kKey_C = 67,
    kKey_D = 68,
    kKey_E = 69,
    kKey_F = 70,
    kKey_G = 71,
    kKey_H = 72,
    kKey_I = 73,
    kKey_J = 74,
    kKey_K = 75,
    kKey_L = 76,
    kKey_M = 77,
    kKey_N = 78,
    kKey_O = 79,
    kKey_P = 80,
    kKey_Q = 81,
    kKey_R = 82,
    kKey_S = 83,
    kKey_T = 84,
    kKey_U = 85,
    kKey_V = 86,
    kKey_W = 87,
    kKey_X = 88,
    kKey_Y = 89,
    kKey_Z = 90,
    kKey_LeftBracket = 91,   /* [ */
    kKey_BackSlash = 92,     /* \ */
    kKey_RightBracket = 93,  /* ] */
    kKey_GraveAccent = 96,  /* ` */
    /* Function keys */
    kKey_Escape = 256,
    kKey_Enter = 257,
    kKey_Tab = 258,
    kKey_Backspace = 259,
    kKey_Insert = 260,
    kKey_Delete = 261,
    kKey_Right = 262,
    kKey_Left = 263,
    kKey_Down = 264,
    kKey_Up = 265,
    kKey_PageUp = 266,
    kKey_PageDown = 267,
    kKey_Home = 268,
    kKey_End = 269,
    kKey_CapsLock = 280,
    kKey_ScrollLock = 281,
    kKey_NumLock = 282,
    kKey_PrintScreen = 283,
    kKey_Pause = 284,
    kKey_F1 = 290,
    kKey_F2 = 291,
    kKey_F3 = 292,
    kKey_F4 = 293,
    kKey_F5 = 294,
    kKey_F6 = 295,
    kKey_F7 = 296,
    kKey_F8 = 297,
    kKey_F9 = 298,
    kKey_F10 = 299,
    kKey_F11 = 300,
    kKey_F12 = 301,
    kKey_F13 = 302,
    kKey_F14 = 303,
    kKey_F15 = 304,
    kKey_F16 = 305,
    kKey_F17 = 306,
    kKey_F18 = 307,
    kKey_F19 = 308,
    kKey_F20 = 309,
    kKey_F21 = 310,
    kKey_F22 = 311,
    kKey_F23 = 312,
    kKey_F24 = 313,
    kKey_F25 = 314,
    kKey_KP0 = 320,
    kKey_KP1 = 321,
    kKey_KP2 = 322,
    kKey_KP3 = 323,
    kKey_KP4 = 324,
    kKey_KP5 = 325,
    kKey_KP6 = 326,
    kKey_KP7 = 327,
    kKey_KP8 = 328,
    kKey_KP9 = 329,
    kKey_KPDecimal = 330,
    kKey_KPDivide = 331,
    kKey_KPMultiply = 332,
    kKey_KPSubtract = 333,
    kKey_KPAdd = 334,
    kKey_KPEnter = 335,
    kKey_KPEqual = 336,
    kKey_LeftShift = 340,
    kKey_LeftControl = 341,
    kKey_LeftAlt = 342,
    kKey_LeftSuper = 343,
    kKey_RightShift = 344,
    kKey_RightControl = 345,
    kKey_RightAlt = 346,
    kKey_RightSuper = 347,
    kKey_Menu = 348
};

enum MouseButtonEnum {
    kMouseButton_Left = 0,
    kMouseButton_Right = 1,
    kMouseButton_Scroll = 2
};

inline constexpr Float32 kRGBAMin = 0.0f;
inline constexpr Float32 kRGBAMax = 1.0f;

}//gui
}//zengine

#endif // !Z_GUI_INTERNAL_D_CONSTANT_H_