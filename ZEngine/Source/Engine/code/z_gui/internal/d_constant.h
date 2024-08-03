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
    kKeySpace = 32,
    kKeyApostrophe = 39,    /* ' */
    kKeyComma = 44,         /* , */
    kKeyMinus = 45,         /* - */
    kKeyPeriod = 46,        /* . */
    kKeySlash = 47,         /* / */
    kKey0 = 48,
    kKey1 = 49,
    kKey2 = 50,
    kKey3 = 51,
    kKey4 = 52,
    kKey5 = 53,
    kKey6 = 54,
    kKey7 = 55,
    kKey8 = 56,
    kKey9 = 57,
    kKeySemicolon = 59,     /* ; */
    kKeyEqual = 61,         /* = */
    kKeyA = 65,
    kKeyB = 66,
    kKeyC = 67,
    kKeyD = 68,
    kKeyE = 69,
    kKeyF = 70,
    kKeyG = 71,
    kKeyH = 72,
    kKeyI = 73,
    kKeyJ = 74,
    kKeyK = 75,
    kKeyL = 76,
    kKeyM = 77,
    kKeyN = 78,
    kKeyO = 79,
    kKeyP = 80,
    kKeyQ = 81,
    kKeyR = 82,
    kKeyS = 83,
    kKeyT = 84,
    kKeyU = 85,
    kKeyV = 86,
    kKeyW = 87,
    kKeyX = 88,
    kKeyY = 89,
    kKeyZ = 90,
    kKeyLeftBracket = 91,   /* [ */
    kKeyBackSlash = 92,     /* \ */
    kKeyRightBracket = 93,  /* ] */
    kKeyGraveAccent = 96,  /* ` */
    /* Function keys */
    kKeyEscape = 256,
    kKeyEnter = 257,
    kKeyTab = 258,
    kKeyBackspace = 259,
    kKeyInsert = 260,
    kKeyDelete = 261,
    kKeyRight = 262,
    kKeyLeft = 263,
    kKeyDown = 264,
    kKeyUp = 265,
    kKeyPageUp = 266,
    kKeyPageDown = 267,
    kKeyHome = 268,
    kKeyEnd = 269,
    kKeyCapsLock = 280,
    kKeyScrollLock = 281,
    kKeyNumLock = 282,
    kKeyPrintScreen = 283,
    kKeyPause = 284,
    kKeyF1 = 290,
    kKeyF2 = 291,
    kKeyF3 = 292,
    kKeyF4 = 293,
    kKeyF5 = 294,
    kKeyF6 = 295,
    kKeyF7 = 296,
    kKeyF8 = 297,
    kKeyF9 = 298,
    kKeyF10 = 299,
    kKeyF11 = 300,
    kKeyF12 = 301,
    kKeyF13 = 302,
    kKeyF14 = 303,
    kKeyF15 = 304,
    kKeyF16 = 305,
    kKeyF17 = 306,
    kKeyF18 = 307,
    kKeyF19 = 308,
    kKeyF20 = 309,
    kKeyF21 = 310,
    kKeyF22 = 311,
    kKeyF23 = 312,
    kKeyF24 = 313,
    kKeyF25 = 314,
    kKeyKP0 = 320,
    kKeyKP1 = 321,
    kKeyKP2 = 322,
    kKeyKP3 = 323,
    kKeyKP4 = 324,
    kKeyKP5 = 325,
    kKeyKP6 = 326,
    kKeyKP7 = 327,
    kKeyKP8 = 328,
    kKeyKP9 = 329,
    kKeyKPDecimal = 330,
    kKeyKPDivide = 331,
    kKeyKPMultiply = 332,
    kKeyKPSubtract = 333,
    kKeyKPAdd = 334,
    kKeyKPEnter = 335,
    kKeyKPEqual = 336,
    kKeyLeftShift = 340,
    kKeyLeftControl = 341,
    kKeyLeftAlt = 342,
    kKeyLeftSuper = 343,
    kKeyRightShift = 344,
    kKeyRightControl = 345,
    kKeyRightAlt = 346,
    kKeyRightSuper = 347,
    kKeyMenu = 348
};

enum MouseButtonEnum {
    MouseButtonLeft = 0,
    MouseButtonRight = 1,
    MouseButtonScroll = 2
};

inline constexpr Float32 kRGBAMin = 0.0f;
inline constexpr Float32 kRGBAMax = 1.0f;

}//gui
}//zengine

#endif // !Z_GUI_INTERNAL_D_CONSTANT_H_