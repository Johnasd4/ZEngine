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

static constexpr ReturnType kOK = 0;

static constexpr ReturnType kErrorCodeBasePCore = 0x00100000u;
static constexpr ReturnType kErrorCodeBaseZFile = kErrorCodeBasePCore + 0x100u;
static constexpr ReturnType kErrorCodeBaseZPrintManager = kErrorCodeBasePCore + 0x200u;
static constexpr ReturnType kErrorCodeBaseMLog = kErrorCodeBasePCore + 0x300u;
static constexpr ReturnType kErrorCodeBaseTUniqueLock = kErrorCodeBasePCore + 0x400u;

}//zengine

#endif // !Z_CORE_INTERNAL_D_ERROR_CODE_H_