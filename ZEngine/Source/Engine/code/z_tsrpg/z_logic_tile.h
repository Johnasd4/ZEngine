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

#include "z_tile.h"

namespace zengine {
namespace tsrpg {

namespace error_code {

enum ZLogicTileErrorCode : ReturnType {
    kZLogicTileErrorCode_LinkError = kErrorCodeBase_ZLogicTile
};

}//error_code

/*
    The logic tile base class, used for logic calculation.
*/
class TSRPG_DLLAPI ZLogicTile : public ZTile {
public:
    static constexpr Int32 kDistanceNotConnet = -1;

    ZLogicTile() noexcept;

    virtual ~ZLogicTile() noexcept;

    NODISCARD FORCEINLINE LogicVector2D Index() const noexcept { return LogicVector2D(pos_.x_, pos_.y_); }
    NODISCARD FORCEINLINE LogicVector3D Pos() const noexcept { return pos_; }
    NODISCARD FORCEINLINE Int32 X() const noexcept { return pos_.x_; }
    NODISCARD FORCEINLINE Int32 Y() const noexcept { return pos_.y_; }
    NODISCARD FORCEINLINE Int32 Z() const noexcept { return pos_.z_; }

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
    NODISCARD virtual ZTile* CreateCopy() const noexcept = 0;

    /*
        Returns the logic distance with height, ignores all obstacles.
    */
    NODISCARD virtual Int32 CalculateDistanceWithHeight(const ZLogicTile& _tile) const noexcept = 0;

    /*
        Returns the logic distance without height, ignores all obstacles.
    */
    NODISCARD virtual Int32 CalculateDistanceWithoutHeight(const ZLogicTile& _tile) const noexcept = 0;

    LogicVector3D pos_;
protected:
    using SuperType_ = ZTile;

private:
    ZLogicTile(const ZLogicTile&) = delete;
    ZLogicTile(ZLogicTile&&) = delete;
    ZLogicTile& operator=(const ZLogicTile&) = delete;
    ZLogicTile& operator=(ZLogicTile&&) = delete;
};

}//tsrpg
}//zengine