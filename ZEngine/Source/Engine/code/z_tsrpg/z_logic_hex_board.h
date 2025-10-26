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

#include "z_logic_board.h"

namespace zengine {
namespace error_code {
enum ZLogicHexBoardErrorCode : ReturnType {
    kZLogicHexBoardErrorCode_LinkError = kErrorCodeBase_ZLogicHexBoard,
    kZLogicHexBoardErrorCode_SystemError,
    kZLogicHexBoardErrorCode_NullptrParam,
    kZLogicHexBoardErrorCode_ParamOutOfRange,
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {

/*
    The hex logic board class, used for logic calculation.
*/
class TSRPG_DLLAPI ZLogicHexBoard : public ZLogicBoard {
public:
    ZLogicHexBoard() noexcept;

    virtual ~ZLogicHexBoard() noexcept;

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

protected:
    using SuperType_ = ZLogicBoard;
    
    /*
        News a logic tile, returns the tile ptr.
    */
    NODISCARD virtual ZLogicTile* CreateLogicTileP() const noexcept;

private:
    ZLogicHexBoard(const ZLogicHexBoard&) = delete;
    ZLogicHexBoard(ZLogicHexBoard&&) = delete;
    ZLogicHexBoard& operator=(const ZLogicHexBoard&) = delete;
    ZLogicHexBoard& operator=(ZLogicHexBoard&&) = delete;
};

}//tsrpg
}//zengine