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
namespace tsrpg {

namespace error_code {

enum ZHexLogicBoardErrorCode : ReturnType {
    kZHexLogicBoardErrorCode_LinkError = kErrorCodeBase_ZHexLogicBoard
};

}//error_code

/*
    The hex logic board class, used for logic calculation.
*/
class TSRPG_DLLAPI ZHexLogicBoard : public ZLogicBoard {
public:
    ZHexLogicBoard() noexcept;

    virtual ~ZHexLogicBoard() noexcept;

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

protected:
    using SuperType_ = ZLogicBoard;
    
private:
    ZHexLogicBoard(const ZHexLogicBoard&) = delete;
    ZHexLogicBoard(ZHexLogicBoard&&) = delete;
    ZHexLogicBoard& operator=(const ZHexLogicBoard&) = delete;
    ZHexLogicBoard& operator=(ZHexLogicBoard&&) = delete;
};

}//tsrpg
}//zengine