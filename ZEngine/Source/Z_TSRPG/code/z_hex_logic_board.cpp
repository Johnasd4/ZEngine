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

#include "z_hex_logic_board.h"

#include "z_core/m_log.h"

namespace zengine {
namespace tsrpg {

ZHexLogicBoard::ZHexLogicBoard() noexcept : SuperType_() {}

ZHexLogicBoard::~ZHexLogicBoard() noexcept {}

NODISCARD UInt64 ZHexLogicBoard::Type() const noexcept {
    return kBoardType_LogicBoard | kBoardType_HexBoard; 
}

Void ZHexLogicBoard::Destroy() noexcept {
    SuperType_::Destroy();
}

NODISCARD ReturnType ZHexLogicBoard::Initialize(const LogicVector2D& _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::Initialize(_size);
    if (link_code != kOK) {
        ret_val = error_code::kZHexLogicBoardErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogicBoard::Initialize() link error!");
        return ret_val;
    }
    return ret_val;
}

NODISCARD ReturnType ZHexLogicBoard::Fill(const ZLogicTile& _tile_template) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::Fill(_tile_template);
    if (link_code != kOK) {
        ret_val = error_code::kZHexLogicBoardErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZLogicBoard::Fill() link error!");
        return ret_val;
    }
    return ret_val;
}

}//tsrpg
}//zengine