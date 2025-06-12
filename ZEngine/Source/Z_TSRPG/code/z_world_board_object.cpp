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

#include "z_world_board_object.h"

#include "z_core/m_log.h"

namespace zengine {
namespace tsrpg {

ZWorldBoardObject::ZWorldBoardObject() noexcept
    : SuperType_()
    , logic_object_ptr_(nullptr)
{}

ZWorldBoardObject::~ZWorldBoardObject() noexcept {}

NODISCARD UInt64 ZWorldBoardObject::Type() const noexcept { return kBoardObjectType_WorldBoardObject; }

Void ZWorldBoardObject::Destroy() noexcept {
    SuperType_::Destroy();
}

NODISCARD ReturnType ZWorldBoardObject::Initialize(ZTile* _owner_tile_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = SuperType_::Initialize(_owner_tile_ptr);
    if (link_code != kOK) {
        ret_val = error_code::kZWorldBoardObjectErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZBoardObject::Initialize() link error!");
        return ret_val;
    }
    return ret_val;
}

}//tsrpg
}//zengine