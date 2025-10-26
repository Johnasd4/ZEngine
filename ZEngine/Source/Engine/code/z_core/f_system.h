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
#pragma once

#include "internal/z_drive.h"

namespace zengine {
namespace error_code {
enum FSystemErrorCodeEnum : ReturnType {
    kFSystemErrorCode_LinkError = kErrorCodeBase_FSystem,
    kFSystemPoolErrorCode_SystemError,
    kFSystemPoolErrorCode_NullptrParam,
    kFSystemPoolErrorCode_ParamOutOfRange,
};
}//error_code
}//zengine

namespace zengine{
namespace system{

/*
    Returns if running as administrator.
*/
CORE_DLLAPI Bool IsAdmin() noexcept;

/*
    Restart program with administrator privileges.
*/
CORE_DLLAPI Void RunAsAdmin() noexcept;

}//system
}//zengine