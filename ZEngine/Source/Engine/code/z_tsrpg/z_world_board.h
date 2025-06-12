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

#include "z_board.h"

namespace zengine {
namespace tsrpg {

namespace error_code {

enum ZWorldBoardErrorCode : ReturnType {
    kZWorldBoardErrorCode_LinkError = kErrorCodeBase_ZWorldBoard,
    kZWorldBoardErrorCode_LogicBoardNotExist,
    kZWorldBoardErrorCode_BoardNotInitialized,
};

}//error_code

/*
    The world board base class, used for scene boards.
*/
class TSRPG_DLLAPI ZWorldBoard : public ZBoard {
public:
    ZWorldBoard(const TileSizeType& _tile_size) noexcept;

    virtual ~ZWorldBoard() noexcept;

    NODISCARD FORCEINLINE ZLogicBoard* LogicBoardPtr() noexcept { return logic_board_ptr_; }
    NODISCARD FORCEINLINE const ZLogicBoard* LogicBoardPtr() const noexcept { return logic_board_ptr_; }
    NODISCARD FORCEINLINE TileSizeType TileSize() const noexcept { return tile_size_; }
    NODISCARD FORCEINLINE WorldVector3D PosOffset() const noexcept { return pos_offset_; }
    NODISCARD FORCEINLINE Bool Initialized() const noexcept { return initialized_; }

    NODISCARD ReturnType SetTileSize(const TileSizeType& _size) noexcept;
    NODISCARD ReturnType SetPosOffset(const WorldVector3D& _offset) noexcept;
    NODISCARD ReturnType SetPosOffset(const LogicVector3D& _offset) noexcept;

    /*
        Get the board type.
    */
    NODISCARD virtual UInt64 Type() const noexcept;

    /*
        Empty the board and remove all the relative links.
    */
    virtual Void Destroy() noexcept;

    /*
        Initialize the board to the given size.
    */
    NODISCARD virtual ReturnType Initialize(const LogicVector2D& _size) noexcept;

    /*
        Register the world tile to the given logic tile. Call after initialized.
    */
    NODISCARD virtual ReturnType RegisterLogicBoard(
        ZLogicBoard* _logic_board_ptr,
        const ZWorldTile& _tile_template
    ) noexcept;

    /*
        Calculates the world pos by the given logic pos and tile size.
    */
    NODISCARD virtual WorldVector3D CalculateWorldVectorByLogicVector(
        const LogicVector3D& _vec
    ) noexcept = 0;

    /*
        Called after the class member changed, do the changes in this function.
    */
    virtual Void OnTileSizeChanged(
        const TileSizeType& _old_tile_size,
        const TileSizeType& _new_tile_size
    ) noexcept;

    /*
        Called after the class member changed, do the changes in this function.
    */
    virtual Void OnPosOffsetChanged(
        const WorldVector3D& _old_offset,
        const WorldVector3D& _new_offset
    ) noexcept;

    ZLogicBoard* logic_board_ptr_;
    TileSizeType tile_size_;
    WorldVector3D pos_offset_;

protected:
    using SuperType_ = ZBoard;

private:
    ZWorldBoard(const ZWorldBoard&) = delete;
    ZWorldBoard(ZWorldBoard&&) = delete;
    ZWorldBoard& operator=(const ZWorldBoard&) = delete;
    ZWorldBoard& operator=(ZWorldBoard&&) = delete;
};

}//tsrpg
}//zengine