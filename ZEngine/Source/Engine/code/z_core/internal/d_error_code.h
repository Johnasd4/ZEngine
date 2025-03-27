/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#ifndef Z_CORE_INTERNAL_D_ERROR_CODE_H_
#define Z_CORE_INTERNAL_D_ERROR_CODE_H_

#include "d_lib.h"
#include "d_type.h"

namespace zengine {

inline constexpr ReturnType kOK = 0;

namespace error_code {

inline constexpr ReturnType kErrorCodeBase_PCore = 0x00100000u;
inline constexpr ReturnType kErrorCodeBase_ZFile = kErrorCodeBase_PCore + 0x100u;
inline constexpr ReturnType kErrorCodeBase_ZPrintManager = kErrorCodeBase_PCore + 0x200u;
inline constexpr ReturnType kErrorCodeBase_MLog = kErrorCodeBase_PCore + 0x300u;
inline constexpr ReturnType kErrorCodeBase_TUniqueLock = kErrorCodeBase_PCore + 0x400u;
inline constexpr ReturnType kErrorCodeBase_ZThreadPool = kErrorCodeBase_PCore + 0x500u;
inline constexpr ReturnType kErrorCodeBase_FMemoryPool = kErrorCodeBase_PCore + 0x600u;
inline constexpr ReturnType kErrorCodeBase_ZTask = kErrorCodeBase_PCore + 0x700u;
inline constexpr ReturnType kErrorCodeBase_FFileSystem = kErrorCodeBase_PCore + 0x800u;
inline constexpr ReturnType kErrorCodeBase_ZString = kErrorCodeBase_PCore + 0x900u;
inline constexpr ReturnType kErrorCodeBase_FDiagnose = kErrorCodeBase_PCore + 0xA00u;

}//error_code

}//zengine

#endif // !Z_CORE_INTERNAL_D_ERROR_CODE_H_