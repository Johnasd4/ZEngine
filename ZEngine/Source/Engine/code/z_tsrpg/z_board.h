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

#include "z_core/t_matrix.h"

#include "z_rpg_object.h"

namespace zengine {
namespace tsrpg {

namespace error_code {

enum ZBoardErrorCode : ReturnType {
    kZBoardErrorCode_LinkError = kErrorCodeBase_ZBoard,
};

}//error_code

/*
    The board base class.
*/
class TSRPG_DLLAPI ZBoard : public ZRPGObject {
public:
    ZBoard() noexcept;

    virtual ~ZBoard() noexcept;

    NODISCARD FORCEINLINE ZTile*& operator()(IndexType _x, IndexType _y) noexcept { 
        return tile_matrix_(_x, _y);
    }
    NODISCARD FORCEINLINE ZTile* const& operator()(IndexType _x, IndexType _y) const noexcept {
        return tile_matrix_(_x, _y);
    }
    NODISCARD FORCEINLINE ZTile*& operator()(LogicVector2D _index) noexcept {
        return tile_matrix_(_index.x_, _index.y_);
    }
    NODISCARD FORCEINLINE ZTile* const& operator()(LogicVector2D _index) const noexcept {
        return tile_matrix_(_index.x_, _index.y_);
    }

    NODISCARD FORCEINLINE IndexType XLength() const noexcept { return tile_matrix_.Row(); }
    NODISCARD FORCEINLINE IndexType YLength() const noexcept { return tile_matrix_.Column(); }
    NODISCARD FORCEINLINE LogicVector2D Size() const noexcept {
        return LogicVector2D(tile_matrix_.Row(), tile_matrix_.Column()); 
    }

    /*
        Get the board type.
    */
    NODISCARD virtual UInt64 Type() const noexcept = 0;

    /*
        Empty the board and remove all the relative links.
    */
    virtual Void Destroy() noexcept;

    /*
        Initialize the board to the given size. The board is still empty.
    */
    NODISCARD virtual ReturnType Initialize(const LogicVector2D& _size) noexcept;

    TMatrix<ZTile*> tile_matrix_;
protected:
    using SuperType_ = ZRPGObject;
    
private:
    ZBoard(const ZBoard&) = delete;
    ZBoard(ZBoard&&) = delete;
    ZBoard& operator=(const ZBoard&) = delete;
    ZBoard& operator=(ZBoard&&) = delete;
};

}//tsrpg
}//zengine