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

enum ZWorldTileErrorCode : ReturnType {
    kZWorldTileErrorCode_LinkError = kErrorCodeBase_ZWorldTile,
    kZWorldTileErrorCode_LogicTileNotExists
};

}//error_code

/*
    The world tile base class, used for scene tiles.
*/
class TSRPG_DLLAPI ZWorldTile : public ZTile {
public:
    ZWorldTile() noexcept;

    virtual ~ZWorldTile() noexcept;

    NODISCARD FORCEINLINE LogicVector2D Index() const noexcept { return index_; }
    NODISCARD FORCEINLINE WorldVector3D Pos() const noexcept { return pos_; }
    NODISCARD FORCEINLINE Float32 X() const noexcept { return pos_.x_; }
    NODISCARD FORCEINLINE Float32 Y() const noexcept { return pos_.y_; }
    NODISCARD FORCEINLINE Float32 Z() const noexcept { return pos_.z_; }

    NODISCARD ReturnType SetPos(const WorldVector3D& _pos) noexcept;
    NODISCARD ReturnType SetPos(const LogicVector3D& _pos) noexcept;

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
        Register the world tile to the given logic tile. Call after initialized.
    */
    NODISCARD virtual ReturnType RegisterLogicTile(
        ZLogicTile* _logic_tile_ptr
    ) noexcept;

    /*
        Called after the class member changed, do the changes in this function.
    */
    virtual Void OnPosChanged(
        const WorldVector3D& _old_pos,
        const WorldVector3D& _new_pos
    ) noexcept;

    WorldVector3D pos_;
    LogicVector2D index_;
protected:
    using SuperType_ = ZTile;

private:
    ZWorldTile(const ZWorldTile&) = delete;
    ZWorldTile(ZWorldTile&&) = delete;
    ZWorldTile& operator=(const ZWorldTile&) = delete;
    ZWorldTile& operator=(ZWorldTile&&) = delete;
};

}//tsrpg
}//zengine