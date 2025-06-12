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
namespace tsrpg {

namespace error_code {

    enum ZHexLogicTileErrorCode : ReturnType {
        kZHexLogicTileErrorCode_LinkError = kErrorCodeBase_ZHexLogicTile,
        kZHexLogicTileErrorCode_WorldBoardNotExist
};

}//error_code

/*
    The hex tile logic class, used for logic calculation.
*/
class TSRPG_DLLAPI ZHexLogicTile : public ZLogicTile {
public:
    ZHexLogicTile() noexcept;

    virtual ~ZHexLogicTile() noexcept;

    /*
        Get the tile type.
    */
    NODISCARD virtual UInt64 Type() const noexcept;

    /*
        Destroy the tile.
    */
    virtual Void Destroy() noexcept;

    /*
        Initialize the tile.
    */
    NODISCARD virtual ReturnType Initialize(ZBoard* _owner_board_ptr) noexcept;

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
    ZHexLogicTile(const ZHexLogicTile&) = delete;
    ZHexLogicTile(ZHexLogicTile&&) = delete;
    ZHexLogicTile& operator=(const ZHexLogicTile&) = delete;
    ZHexLogicTile& operator=(ZHexLogicTile&&) = delete;
};

}//tsrpg
}//zengine