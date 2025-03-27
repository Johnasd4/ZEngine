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
#ifndef Z_GUI_INTERNAL_D_ERROR_CODE_H_
#define Z_GUI_INTERNAL_D_ERROR_CODE_H_

#include "d_lib.h"
#include "d_type.h"

namespace zengine {
namespace gui {
namespace error_code {

inline constexpr ReturnType kErrorCodeBase_PGui = 0x00200000u;
inline constexpr ReturnType kErrorCodeBase_ZApplication = kErrorCodeBase_PGui + 0x100u;
inline constexpr ReturnType kErrorCodeBase_ZWindow = kErrorCodeBase_PGui + 0x200u;
inline constexpr ReturnType kErrorCodeBase_ZFrame = kErrorCodeBase_PGui + 0x300u;
inline constexpr ReturnType kErrorCodeBase_ZText = kErrorCodeBase_PGui + 0x400u;
inline constexpr ReturnType kErrorCodeBase_ZInputText = kErrorCodeBase_PGui + 0x500u;
inline constexpr ReturnType kErrorCodeBase_ZButton = kErrorCodeBase_PGui + 0x600u;

}//error_code
}//gui
}//zengine

#endif // !Z_GUI_INTERNAL_D_ERROR_CODE_H_