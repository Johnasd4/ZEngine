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

#include "d_lib.h"
#include "d_type.h"

namespace zengine {
namespace error_code {

inline constexpr ReturnType kErrorCodeBase_PGui = 0x1U * kErrorCodeProjcetMask + kErrorCodeBase_SZEngine;

inline constexpr ReturnType kErrorCodeBase_ZApplication = 0x0U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZWindow = 0x1U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZFrame = 0x2U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZText = 0x3U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZInputText = 0x4U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZButton = 0x5U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZSeparator = 0x6U * kErrorCodeTypeMask + kErrorCodeBase_PGui;
inline constexpr ReturnType kErrorCodeBase_ZCheckBox = 0x7U * kErrorCodeTypeMask + kErrorCodeBase_PGui;

}//error_code
}//zengine