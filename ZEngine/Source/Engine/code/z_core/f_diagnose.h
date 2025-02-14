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
#ifndef Z_CORE_F_DIAGNOSE_H_
#define Z_CORE_F_DIAGNOSE_H_

#include "internal/z_drive.h"

namespace zengine {

namespace error_code {

enum FDiagnoseErrorCode : ReturnType {
    kFDiagnoseErrorCode_LinkError = kErrorCodeBase_FDiagnose
};

}//error_code

/*
    The namespace contains the diagnose functions.
*/
namespace diagnose {

/*
    Applys memory by the size given and returns the pointer of the memory.
*/
//CORE_DLLAPI NODISCARD Void* ApplyMemory(MemoryType _size) noexcept;

}//test
}//zengine

#endif // !Z_CORE_F_DIAGNOSE_H_
