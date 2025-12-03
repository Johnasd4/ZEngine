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

#include "../z_core/t_array.h"

#include "z_rpg_object.h"

namespace zengine {
namespace error_code {
enum ZLogicBoardViewerErrorCodeEnum : ReturnType {
    kZLogicBoardViewerErrorCode_LinkError = kErrorCodeBase_ZLogicBoardViewer,
    kZLogicBoardViewerErrorCode_SystemError,
    kZLogicBoardViewerErrorCode_NullptrParam,
    kZLogicBoardViewerErrorCode_ParamOutOfRange,
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {
class ZTile;
class ZLogicTile;
class ZLogicBoard;
class ZDisplayBoard;
}//tsrpg
}//zengine

namespace zengine {
namespace tsrpg {

/*
    Logic board viewer, the class between logic board and display board. 
    Used to get a part reference of the logic board to improve the performance.
    Changing the base index and size will regenerate the display board.
*/
class TSRPG_DLLAPI ZLogicBoardViewport : public ZRPGObject {
public:
    ZLogicBoardViewport() noexcept;

    virtual ~ZLogicBoardViewport() noexcept;

    NODISCARD ZLogicTile* operator()(Int32 _x, Int32 _y) noexcept;
    NODISCARD ZLogicTile* const operator()(Int32 _x, Int32 _y) const noexcept;
    NODISCARD ZLogicTile* operator()(LogicVector2D _index) noexcept;
    NODISCARD ZLogicTile* const operator()(LogicVector2D _index) const noexcept;
    NODISCARD ZLogicTile* At(Int32 _x, Int32 _y) noexcept;
    NODISCARD ZLogicTile* const At(Int32 _x, Int32 _y) const noexcept;
    NODISCARD ZLogicTile* At(LogicVector2D _index) noexcept;
    NODISCARD ZLogicTile* const At(LogicVector2D _index) const noexcept;

    NODISCARD FORCEINLINE Int32 XLength() noexcept { return view_size_.x_; }
    NODISCARD FORCEINLINE Int32 YLength() noexcept { return view_size_.y_; }

    /*
        Get the logic board type.
    */
    NODISCARD RPGObjectType Type() const noexcept;

    /*
        Destroy the board viewer.
    */
    Void Destroy() noexcept;

    /*
        Initialize the board viewer.
    */
    NODISCARD ReturnType Initialize(
        ZLogicBoard* _logic_board_ptr,
        const LogicVector2D& _base_index = LogicVector2D(0, 0),
        const LogicVector2D& _view_size = LogicVector2D(-1, -1)
    ) noexcept;

    /*
        Sets the base index of the viewer, will regenerate the display board.
    */
    NODISCARD ReturnType SetBaseIndex(const LogicVector2D& _base_index) noexcept;

    /*
        Sets the view size, will regenerate the display board.
    */
    NODISCARD ReturnType SetViewSize(const LogicVector2D& _view_size) noexcept;

    /*
        Gets the tile's surrounded tile by the given inner and outer radius(distance to the center tile).
        Includes the inner circle and outer circle.
    */
    NODISCARD ReturnType GetSurroundTile(
        TArray<ZTile*>* _tile_list_ptr,
        const LogicVector2D& _center_index,
        Int32 _inner_radius = 1,
        Int32 _outer_radius = 1
    ) noexcept;

    /*
        Calculate the global logic pos offset of the logic board viewport.
    */
    NODISCARD LogicVector3D CalculateLogicPosOffset() const noexcept;

    /*
        Called when the logic board pos offset changed.
    */
    NODISCARD ReturnType OnLogicBoardPosOffsetChanged(const LogicVector3D& _offset) noexcept;

protected:
    using SuperType_ = ZRPGObject;

private:
    ZLogicBoardViewport(const ZLogicBoardViewport&) = delete;
    ZLogicBoardViewport(ZLogicBoardViewport&&) = delete;
    ZLogicBoardViewport& operator=(const ZLogicBoardViewport&) = delete;
    ZLogicBoardViewport& operator=(ZLogicBoardViewport&&) = delete;

public:
    LogicVector2D base_index_;
    LogicVector2D view_size_;

    ZLogicBoard* logic_board_ptr_;
    ZDisplayBoard* display_board_ptr_;
    //used to listen to logic board when change happens
    ZLogicBoardViewport* next_logic_board_viewport_ptr_;
    ZLogicBoardViewport* pre_logic_board_viewport_ptr_;
};

}//tsrpg
}//zengine