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
#ifndef Z_CORE_F_SYSTEM_TIME_H_
#define Z_CORE_F_SYSTEM_TIME_H_

#pragma warning(disable : 26439)

#include "internal/drive.h"

namespace zengine {
namespace system_time {

/*
    The string length is 19, without '\0'. Format is "YYYY/MM/DD-HH:MM:SS".
*/
CORE_DLLAPI Void GetTimeString(CChar* str) noexcept;

}//system_time
}//zengine

#endif // !Z_CORE_F_SYSTEM_TIME_H_
