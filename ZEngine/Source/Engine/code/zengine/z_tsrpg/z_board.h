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

#include "../z_core/t_matrix.h"

#include "z_rpg_object.h"

namespace zengine {
namespace error_code {
enum ZBoardErrorCodeEnum : ReturnType {
    kZBoardErrorCode_LinkError = kErrorCodeBase_ZBoard,
    kZBoardErrorCode_SystemOrLibraryError,
    kZBoardErrorCode_NullptrParam,
    kZBoardErrorCode_ParamOutOfRange,
    kZBoardErrorCode_BoardTypeNotExist
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {
class ZTile;
}//tsrpg
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

    NODISCARD FORCEINLINE ZTile*& operator()(Int32 _x, Int32 _y) noexcept {
        return tile_matrix_(_x, _y);
    }
    NODISCARD FORCEINLINE ZTile* const& operator()(Int32 _x, Int32 _y) const noexcept {
        return tile_matrix_(_x, _y);
    }
    NODISCARD FORCEINLINE ZTile*& operator()(LogicVector2D _index) noexcept {
        return tile_matrix_(_index.x_, _index.y_);
    }
    NODISCARD FORCEINLINE ZTile* const& operator()(LogicVector2D _index) const noexcept {
        return tile_matrix_(_index.x_, _index.y_);
    }
    NODISCARD FORCEINLINE ZTile*& At(Int32 _x, Int32 _y) noexcept {
        return tile_matrix_(_x, _y);
    }
    NODISCARD FORCEINLINE ZTile* const& At(Int32 _x, Int32 _y) const noexcept {
        return tile_matrix_(_x, _y);
    }
    NODISCARD FORCEINLINE ZTile*& At(LogicVector2D _index) noexcept {
        return tile_matrix_(_index.x_, _index.y_);
    }
    NODISCARD FORCEINLINE ZTile* const& At(LogicVector2D _index) const noexcept {
        return tile_matrix_(_index.x_, _index.y_);
    }

    NODISCARD FORCEINLINE Int32 XLength() const noexcept { return static_cast<Int32>(tile_matrix_.Row()); }
    NODISCARD FORCEINLINE Int32 YLength() const noexcept { return static_cast<Int32>(tile_matrix_.Column()); }
    NODISCARD FORCEINLINE LogicVector2D GetSize() const noexcept {
        return LogicVector2D(static_cast<Int32>(tile_matrix_.Row()), static_cast<Int32>(tile_matrix_.Column()));
    }

    /*
        Returns false if the index is out of bound.
    */
    NODISCARD FORCEINLINE Bool IndexCheck(LogicVector2D _index) const noexcept {
        return  _index.x_ < XLength() && _index.y_ < YLength();
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
        TArray<ZTile*>* _tile_list_ptr,
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