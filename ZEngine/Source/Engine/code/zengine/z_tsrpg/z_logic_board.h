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

#include "../z_core/t_list.h"
#include "../z_core/t_set.h"

#include "z_board.h"

namespace zengine {
namespace error_code {
enum ZLogicBoardErrorCodeEnum : ReturnType {
    kZLogicBoardErrorCode_LinkError = kErrorCodeBase_ZLogicBoard,
    kZLogicBoardErrorCode_SystemError,
    kZLogicBoardErrorCode_NullptrParam,
    kZLogicBoardErrorCode_ParamOutOfRange,
    kZLogicBoardErrorCode_OwnerBoardAlreadyExists,
    kZLogicBoardErrorCode_TileTextureNotExist
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {
class ZLogicTile;
class ZLogicTileTexture;
class ZLogicBoard;
class ZLogicBoardViewport;
}//tsrpg
}//zengine

namespace zengine {
namespace tsrpg {

/*
    The logic board base class, used for logic calculation.
*/
class TSRPG_DLLAPI ZLogicBoard : public ZBoard {
public:
    /*
        Used for algorithms to calculate movement between boards.
    */
    struct TeleportInfo_ {
        ZLogicTile* source_tile_ptr_;
        ZLogicBoard* target_board_ptr_;
        ZLogicTile* target_tile_ptr_;

        NODISCARD FORCEINLINE Bool operator==(const TeleportInfo_& _info) const noexcept {
            return (source_tile_ptr_ == _info.source_tile_ptr_) 
                && (target_board_ptr_ == _info.target_board_ptr_) 
                && (target_tile_ptr_ == _info.target_tile_ptr_);
        }
    };

    ZLogicBoard() noexcept;

    virtual ~ZLogicBoard() noexcept;

    ReturnType SetPosOffset(const LogicVector3D& _offset) noexcept;

    /*
        Set the base layer texture of the logic board. 
    */
    ReturnType SetBaseLayerTexture(const WChar* _texture_name) noexcept;

    /*
        Adds a sub board to the board.
    */
    NODISCARD ReturnType AddSubBoard(
        ZLogicBoard* _board_ptr,
        const LogicVector3D& _logic_pos_offset
    ) noexcept;

    /*
        Remove a sub board.
    */
    NODISCARD Void RemoveSubBoard(
        ZLogicBoard* _board_ptr
    ) noexcept;

    /*
        Gets the given board logic pos offset based on this board. Returns false if the given board is not related to
        the current board.
    */
    NODISCARD Bool CalculateRelativeBoardLogicPosOffset(
        const ZLogicBoard* _target_board_ptr,
        LogicVector3D* _offset_ptr
    ) const noexcept;

    /*
        Get the board type.
    */
    NODISCARD virtual RPGObjectType Type() const noexcept;

    /*
        Empty the board and remove all the relative links.
    */
    virtual Void Destroy() noexcept;

    /*
        Initialize the board to the given size.
    */
    NODISCARD virtual ReturnType Initialize(const LogicVector2D& _board_size) noexcept;

    /*
        Fill the board. Will clear the existing tiles.
        _horizontal: Tile's pos z.
        _texture_name: Tile's texture type. (nullptr = no texture)
        _texture_depth: Tile's texture length.
    */
    NODISCARD virtual ReturnType Fill(
        Int32 _horizontal, 
        const WChar* _texture_name, 
        Int32 _texture_length
    ) noexcept;

protected:
    using SuperType_ = ZBoard;
    
    /*
        News a logic tile, returns the tile ptr.
    */
    NODISCARD virtual ZLogicTile* CreateLogicTileP() const noexcept = 0;

private:
    ZLogicBoard(const ZLogicBoard&) = delete;
    ZLogicBoard(ZLogicBoard&&) = delete;
    ZLogicBoard& operator=(const ZLogicBoard&) = delete;
    ZLogicBoard& operator=(ZLogicBoard&&) = delete;

    NODISCARD Bool CalculateRelativeBoardLogicPosOffsetP(
        const ZLogicBoard* _target_board_ptr,
        LogicVector3D* _offset_ptr
    ) const noexcept;

public:
    LogicVector3D pos_offset_;

    const ZLogicTileTexture* base_layer_texture_ptr_;

    ZLogicBoardViewport* logic_board_viewport_head_ptr_;
    ZLogicBoard* owner_board_ptr_;
    ZLogicBoard* sub_board_head_ptr_;
    //used for sub board
    ZLogicBoard* next_sub_board_ptr_;
    ZLogicBoard* pre_sub_board_ptr_;
};

}//tsrpg
}//zengine