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

#include "z_logic_tile.h"

namespace zengine {
namespace error_code {
enum ZLogicHexTileErrorCode : ReturnType {
    kZLogicHexTileErrorCode_LinkError = kErrorCodeBase_ZLogicHexTile,
    kZLogicHexTileErrorCode_SystemError,
    kZLogicHexTileErrorCode_NullptrParam,
    kZLogicHexTileErrorCode_ParamOutOfRange,
    kZLogicHexTileErrorCode_DisplayBoardNotExist
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {

/*
    The hex tile logic class, used for logic calculation.
*/
class TSRPG_DLLAPI ZLogicHexTile : public ZLogicTile {
public:
    ZLogicHexTile() noexcept;

    virtual ~ZLogicHexTile() noexcept;

    /*
        Get the tile type.
    */
    NODISCARD virtual RPGObjectType Type() const noexcept;

    /*
        Destroy the tile.
    */
    virtual Void Destroy() noexcept;

    /*
        Initialize the tile.
    */
    NODISCARD virtual ReturnType Initialize(
        ZBoard* _owner_board_ptr,
        const LogicVector3D& _pos,
        const ZLogicTileTexture* _texture_ptr,
        Int32 _texture_length
    ) noexcept;

    /*
        Returns a copy of the current tile. Must be overrided.
    */
    NODISCARD virtual ZTile* CreateCopy() const noexcept;

    /*
        Returns the logic distance with height, ignores all obstacles.
    */
    NODISCARD virtual Int32 CalculateDistanceWithHeight(const ZLogicTile& _tile) const noexcept;

    /*
        Returns the logic distance without height, ignores all obstacles.
    */
    NODISCARD virtual Int32 CalculateDistanceWithoutHeight(const ZLogicTile& _tile) const noexcept;

protected:
    using SuperType_ = ZLogicTile;

private:
    ZLogicHexTile(const ZLogicHexTile&) = delete;
    ZLogicHexTile(ZLogicHexTile&&) = delete;
    ZLogicHexTile& operator=(const ZLogicHexTile&) = delete;
    ZLogicHexTile& operator=(ZLogicHexTile&&) = delete;
};

}//tsrpg
}//zengine