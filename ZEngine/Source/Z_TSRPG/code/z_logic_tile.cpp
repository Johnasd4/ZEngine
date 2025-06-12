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
#define TSRPG_DLLFILE

#include "z_logic_tile.h"

#include "z_core/m_log.h"

namespace zengine {
namespace tsrpg {

ZLogicTile::ZLogicTile() noexcept
    : SuperType_()
    , pos_()
{}

ZLogicTile::~ZLogicTile() noexcept {}

NODISCARD UInt64 ZLogicTile::Type() const noexcept { return kTileType_LogicTile; }

Void ZLogicTile::Destroy() noexcept {
    SuperType_::Destroy();
}

ReturnType ZLogicTile::Initialize(ZBoard* _owner_board_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::Initialize(_owner_board_ptr);
    if (link_code != kOK) {
        ret_val = error_code::kZLogicTileErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogicTile::Initialize() link error!");
        return ret_val;
    }
    return ret_val;
}

}//tsrpg
}//zengine