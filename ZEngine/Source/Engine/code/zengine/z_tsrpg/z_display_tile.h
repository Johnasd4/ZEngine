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

#include "drive.h"

#include "z_tile.h"

namespace zengine {
namespace error_code {
enum ZDisplayTileErrorCode : ReturnType {
    kZDisplayTileErrorCode_LinkError = kErrorCodeBase_ZDisplayTile,
    kZDisplayTileErrorCode_SystemError,
    kZDisplayTileErrorCode_NullptrParam,
    kZDisplayTileErrorCode_ParamOutOfRange,
    kZDisplayTileErrorCode_LogicTileNotExists
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {

/*
    The display tile base class, used for scene tiles.
*/
class TSRPG_DLLAPI ZDisplayTile : public ZTile {
public:
    ZDisplayTile() noexcept;

    virtual ~ZDisplayTile() noexcept;

    NODISCARD FORCEINLINE Float64 X() noexcept { return pos_.x_; }
    NODISCARD FORCEINLINE Float64 Y() noexcept { return pos_.y_; }
    NODISCARD FORCEINLINE Float64 Z() noexcept { return pos_.z_; }
    NODISCARD FORCEINLINE DisplayVector3D Pos() noexcept { return pos_; }

    /*
        Sets the pos by the display pos.
    */
    NODISCARD ReturnType SetPos(const DisplayVector3D& _pos) noexcept;

    /*
        Sets the pos by the logic pos.
    */
    NODISCARD ReturnType SetPos(const LogicVector3D& _pos) noexcept;

    /*
        Gets the tile's board index.
    */
    NODISCARD virtual LogicVector2D Index() const noexcept;

    /*
        Get the tile type.
    */
    NODISCARD virtual RPGObjectType Type() const noexcept = 0;

    /*
        Destroy the tile.
    */
    virtual Void Destroy() noexcept;

    /*
        Initialize the tile.
    */
    NODISCARD virtual ReturnType Initialize(
        ZBoard* _owner_board_ptr,
        ZLogicTile* _logic_tile_ptr,
        const LogicVector2D& _index
    ) noexcept;

    /*
        Called after the logic pos z changed.
    */
    virtual ReturnType OnLogicPosZChanged(Int32 _new_pos_z) noexcept = 0;

    /*
        Called after the class member changed, do the changes in this function.
    */
    virtual ReturnType OnPosChanged(const DisplayVector3D& _old_pos, const DisplayVector3D& _new_pos) noexcept = 0;

protected:
    using SuperType_ = ZTile;

private:
    ZDisplayTile(const ZDisplayTile&) = delete;
    ZDisplayTile(ZDisplayTile&&) = delete;
    ZDisplayTile& operator=(const ZDisplayTile&) = delete;
    ZDisplayTile& operator=(ZDisplayTile&&) = delete;

public:
    DisplayVector3D pos_;
    LogicVector2D index_;
    
    ZLogicTile* logic_tile_ptr_;
    //used to listen to logic tile when change happens
    ZDisplayTile* next_display_tile_ptr_;
    ZDisplayTile* pre_display_tile_ptr_;
};

}//tsrpg
}//zengine