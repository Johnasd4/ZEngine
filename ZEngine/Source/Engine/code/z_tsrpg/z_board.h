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
namespace error_code {
enum ZBoardErrorCode : ReturnType {
    kZBoardErrorCode_LinkError = kErrorCodeBase_ZBoard,
    kZBoardErrorCode_BoardTypeNotExist,
    kZBoardErrorCode_NullptrParams
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {

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
    NODISCARD FORCEINLINE ZTile*& At(IndexType _x, IndexType _y) noexcept {
        return tile_matrix_(_x, _y);
    }
    NODISCARD FORCEINLINE ZTile* const& At(IndexType _x, IndexType _y) const noexcept {
        return tile_matrix_(_x, _y);
    }
    NODISCARD FORCEINLINE ZTile*& At(LogicVector2D _index) noexcept {
        return tile_matrix_(_index.x_, _index.y_);
    }
    NODISCARD FORCEINLINE ZTile* const& At(LogicVector2D _index) const noexcept {
        return tile_matrix_(_index.x_, _index.y_);
    }

    NODISCARD FORCEINLINE IndexType XLength() const noexcept { return tile_matrix_.Row(); }
    NODISCARD FORCEINLINE IndexType YLength() const noexcept { return tile_matrix_.Column(); }
    NODISCARD FORCEINLINE LogicVector2D Size() const noexcept {
        return LogicVector2D(tile_matrix_.Row(), tile_matrix_.Column()); 
    }

    /*
        Returns false if the index is out of bound.
    */
    NODISCARD FORCEINLINE Bool IndexCheck(IndexType _x, IndexType _y) const noexcept {
        return _x >= 0 && _x < XLength() && _y >= 0 && _y < YLength();
    }
    /*
        Returns false if the index is out of bound.
    */
    NODISCARD FORCEINLINE Bool IndexCheck(LogicVector2D _index) const noexcept {
        return _index.x_ >= 0 && _index.x_ < XLength() && _index.y_ >= 0 && _index.y_ < YLength();
    }

    /*
        Get the board type.
    */
    NODISCARD virtual RPGObjectType Type() const noexcept = 0;

    /*
        Empty the board and remove all the relative links.
    */
    virtual Void Destroy() noexcept;

    /*
        Gets the tile's surrounded tile by the given inner and outer radius(distance to the center tile).
        Includes the inner circle and outer circle.
    */
    NODISCARD virtual ReturnType GetSurroundTile(
        TVector<ZTile*>* _tile_list_ptr,
        const LogicVector2D& _center_index,
        Int32 _inner_radius = 1,
        Int32 _outer_radius = 1
    ) noexcept;

protected:
    using SuperType_ = ZRPGObject;
    
    /*
        Initialize the board to the given size. The board is still empty.
    */
    NODISCARD ReturnType InitializeP(const LogicVector2D& _board_size) noexcept;

private:
    ZBoard(const ZBoard&) = delete;
    ZBoard(ZBoard&&) = delete;
    ZBoard& operator=(const ZBoard&) = delete;
    ZBoard& operator=(ZBoard&&) = delete;

public:
    TMatrix<ZTile*> tile_matrix_;
};

}//tsrpg
}//zengine