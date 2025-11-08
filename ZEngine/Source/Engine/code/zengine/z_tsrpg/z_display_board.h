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

#include "../z_core/z_string.h"
#include "z_board.h"

namespace zengine {
namespace error_code {
enum ZDisplayBoardErrorCode : ReturnType {
    kZDisplayBoardErrorCode_LinkError = kErrorCodeBase_ZDisplayBoard,
    kZDisplayBoardErrorCode_SystemError,
    kZDisplayBoardErrorCode_NullptrParam,
    kZDisplayBoardErrorCode_ParamOutOfRange,
    kZDisplayBoardErrorCode_LogicBoardNotExist,
    kZDisplayBoardErrorCode_BoardNotInitialized,
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {

/*
    The display board base class, used for scene boards.
*/
class TSRPG_DLLAPI ZDisplayBoard : public ZBoard {
public:
    ZDisplayBoard() noexcept;

    virtual ~ZDisplayBoard() noexcept;

    /*
        Sets the tile size. Will respawn the tile actor.
    */
    NODISCARD ReturnType SetTileSize(const TileSizeType& _size) noexcept;

    /*
        Sets the pos offset by the display offset.
    */
    NODISCARD ReturnType SetPosOffset(const DisplayVector3D& _offset) noexcept;

    /*
        Sets the pos offset by the logic offset.
    */
    NODISCARD ReturnType SetPosOffset(const LogicVector3D& _offset) noexcept;

    /*
        Get the board type.
    */
    NODISCARD virtual RPGObjectType Type() const noexcept = 0;

    /*
        Empty the board and remove all the relative links.
    */
    virtual Void Destroy() noexcept;

    /*
        Initialize the board.
    */
    NODISCARD virtual ReturnType Initialize(
        ZLogicBoardViewport* _logic_board_viewport_ptr,
        const TileSizeType& _tile_size,
        const WChar* _tile_mesh_name
    ) noexcept;

    /*
        Calculates the display pos by the given logic pos and tile size.
    */
    NODISCARD virtual DisplayVector3D CalculateDisplayVectorByLogicVector(
        const LogicVector3D& _vec
    ) noexcept = 0;

    /*
        Called after the tile size changed, do the changes in this function.
    */
    virtual ReturnType OnTileSizeChanged(
        const TileSizeType& _old_tile_size,
        const TileSizeType& _new_tile_size
    ) noexcept = 0;

    /*
        Called after the pos offset changed, do the changes in this function.
    */
    virtual ReturnType OnPosOffsetChanged(
        const DisplayVector3D& _old_offset,
        const DisplayVector3D& _new_offset
    ) noexcept = 0;

    /*
        Called after the logic board viewport base index changed, do the changes in this function.
    */
    virtual ReturnType OnLogicBoardViewportBaseIndexChanged(
        const LogicVector2D& _old_base_index,
        const LogicVector2D& _new_base_index
    ) noexcept = 0;

    /*
        Called after the logic board viewport view size changed, do the changes in this function.
    */
    virtual ReturnType OnLogicBoardViewportViewSizeChanged(
        const LogicVector2D& _old_view_size,
        const LogicVector2D& _new_view_size
    ) noexcept = 0;

protected:
    using SuperType_ = ZBoard;

private:
    ZDisplayBoard(const ZDisplayBoard&) = delete;
    ZDisplayBoard(ZDisplayBoard&&) = delete;
    ZDisplayBoard& operator=(const ZDisplayBoard&) = delete;
    ZDisplayBoard& operator=(ZDisplayBoard&&) = delete;

public:
    TileSizeType tile_size_;
    DisplayVector3D pos_offset_;
    ZWString tile_mesh_name_;

    ZLogicBoardViewport* logic_board_viewport_ptr_;
};

}//tsrpg
}//zengine