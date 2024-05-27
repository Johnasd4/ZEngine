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
#define CORE_DLLFILE

#include "m_log.h"

#include "f_console.h"

namespace zengine {
namespace internal {

/*
    Log error message and error location.
*/
CORE_DLLAPI extern Void LogError(const time_t time_raw,
                                 const CChar* error_file, 
                                 const CChar* error_funcion,
                                 Int32 error_line, 
                                 const CChar* format,
                                 ...) noexcept {

}

}//internal
}//zengine