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

#include "z_rpg_object.h"

namespace zengine {
namespace error_code {
enum ZTileErrorCode : ReturnType {
    kZTileErrorCode_LinkError = kErrorCodeBase_ZTile,
    kZTileErrorCode_NullptrParams
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {

/*
    The tile base class.
*/
class TSRPG_DLLAPI ZTile : public ZRPGObject {
public:
    ZTile() noexcept;

    virtual ~ZTile() noexcept;

    /*
        Gets the tile's board index.
    */
    NODISCARD virtual LogicVector2D Index() const noexcept = 0;

    /*
        Get the tile type.
    */
    NODISCARD virtual RPGObjectType Type() const noexcept = 0;

    /*
        Destroy the tile.
    */
    virtual Void Destroy() noexcept;

    /*
        Returns a copy of the current tile. Must be overrided.
    */
    NODISCARD virtual ZTile* CreateCopy() const noexcept = 0;

protected:
    using SuperType_ = ZRPGObject;

    /*
        Initialize the tile.
    */
    NODISCARD virtual ReturnType InitializeP(ZBoard* _owner_board_ptr) noexcept;

private:
    ZTile(const ZTile&) = delete;
    ZTile(ZTile&&) = delete;
    ZTile& operator=(const ZTile&) = delete;
    ZTile& operator=(ZTile&&) = delete;

public:
    ZBoard* owner_board_ptr_;
    ZBoardObject* board_object_head_ptr_;
};

}//tsrpg
}//zengine