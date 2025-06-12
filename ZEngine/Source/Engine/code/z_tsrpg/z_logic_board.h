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

#include "z_core/t_list.h"
#include "z_core/t_set.h"

#include "z_board.h"

namespace zengine {
namespace tsrpg {

namespace error_code {

enum ZLogicBoardErrorCode : ReturnType {
    kZLogicBoardErrorCode_LinkError = kErrorCodeBase_ZLogicBoard,
    kZLogicBoardErrorCode_NullptrParams,
    kZLogicBoardErrorCode_OwnerBoardAlreadyExists,
};

}//error_code

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

    NODISCARD FORCEINLINE ZLogicBoard* OwnerBoardPtr() noexcept { return owner_board_ptr_; }
    NODISCARD FORCEINLINE const ZLogicBoard* OwnerBoardPtr() const noexcept { return owner_board_ptr_; }
    NODISCARD FORCEINLINE ZWorldBoard* WorldBoardPtr() noexcept { return world_board_ptr_; }
    NODISCARD FORCEINLINE const ZWorldBoard* WorldBoardPtr() const noexcept { return world_board_ptr_; }
    NODISCARD FORCEINLINE LogicVector3D PosOffset() const noexcept { return pos_offset_; }

    ReturnType SetPosOffset(const LogicVector3D& _offset) noexcept;

    /*
        Register a teleport info.
    */
    NODISCARD ReturnType AddTeleportInfo(
        ZLogicTile* _source_tile_ptr,
        ZLogicBoard* _target_board_ptr,
        ZLogicTile* _target_tile_ptr
    ) noexcept;

    /*
        Adds a sub board to the board.
    */
    NODISCARD ReturnType AddSubBoard(
        ZLogicBoard* _board_ptr,
        const LogicVector3D& _logic_pos_offset
    ) noexcept;

    /*
        Remove a teleport info.
    */
    NODISCARD Void RemoveTeleportInfo(
        ZLogicTile* _source_tile_ptr,
        ZLogicBoard* _target_board_ptr,
        ZLogicTile* _target_tile_ptr
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
        Fill the board with the tile template. Will clear the existing tiles.
    */
    NODISCARD virtual ReturnType Fill(const ZLogicTile& _tile_template) noexcept;

    TSet<ZLogicBoard*> sub_board_ptr_set_;
    ZLogicBoard* owner_board_ptr_;
    ZWorldBoard* world_board_ptr_;
    TSet<ZLogicBoard*> relevant_board_ptr_set_;
    TList<TeleportInfo_> teleport_info_list_;
    LogicVector3D pos_offset_;

protected:
    using SuperType_ = ZBoard;
    
private:
    ZLogicBoard(const ZLogicBoard&) = delete;
    ZLogicBoard(ZLogicBoard&&) = delete;
    ZLogicBoard& operator=(const ZLogicBoard&) = delete;
    ZLogicBoard& operator=(ZLogicBoard&&) = delete;

    NODISCARD Bool CalculateRelativeBoardLogicPosOffsetP(
        const ZLogicBoard* _target_board_ptr,
        LogicVector3D* _offset_ptr
    ) const noexcept;

    NODISCARD Void ClearReleventInfoP() noexcept;
};

}//tsrpg
}//zengine