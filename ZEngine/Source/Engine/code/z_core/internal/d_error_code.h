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
#ifndef Z_CORE_INTERNAL_D_ERROR_CODE_H_
#define Z_CORE_INTERNAL_D_ERROR_CODE_H_

#include "d_lib.h"
#include "d_type.h"

namespace zengine {

inline constexpr ReturnType kOK = 0;

namespace error_code {

inline constexpr ReturnType kErrorCodeBasePCore = 0x00100000u;
inline constexpr ReturnType kErrorCodeBaseZFile = kErrorCodeBasePCore + 0x100u;
inline constexpr ReturnType kErrorCodeBaseZPrintManager = kErrorCodeBasePCore + 0x200u;
inline constexpr ReturnType kErrorCodeBaseMLog = kErrorCodeBasePCore + 0x300u;
inline constexpr ReturnType kErrorCodeBaseTUniqueLock = kErrorCodeBasePCore + 0x400u;
inline constexpr ReturnType kErrorCodeBaseZThreadPool = kErrorCodeBasePCore + 0x500u;
inline constexpr ReturnType kErrorCodeBaseFMemoryPool = kErrorCodeBasePCore + 0x600u;
inline constexpr ReturnType kErrorCodeBaseZTask = kErrorCodeBasePCore + 0x700u;
inline constexpr ReturnType kErrorCodeBaseFFileSystem = kErrorCodeBasePCore + 0x800u;
inline constexpr ReturnType kErrorCodeBaseZString = kErrorCodeBasePCore + 0x900u;
inline constexpr ReturnType kErrorCodeBaseFTest = kErrorCodeBasePCore + 0xA00u;

}//error_code

}//zengine

#endif // !Z_CORE_INTERNAL_D_ERROR_CODE_H_