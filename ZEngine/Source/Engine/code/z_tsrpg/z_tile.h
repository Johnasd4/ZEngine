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

#include "z_rpg_object.h"

namespace zengine {
namespace tsrpg {

namespace error_code {

enum ZTileErrorCode : ReturnType {
    kZTileErrorCode_LinkError = kErrorCodeBase_ZTile,
    kZTileErrorCode_NullptrParams
};

}//error_code

/*
    The tile base class.
*/
class TSRPG_DLLAPI ZTile : public ZRPGObject {
public:
    ZTile() noexcept;

    virtual ~ZTile() noexcept;

    NODISCARD FORCEINLINE ZBoard* OwnerBoardPtr() noexcept { return owner_board_ptr_; }
    NODISCARD FORCEINLINE const ZBoard* OwnerBoardPtr() const noexcept { return owner_board_ptr_; }
    NODISCARD FORCEINLINE ZBoardObject* BoardObjectPtr() noexcept { return board_object_ptr_; }
    NODISCARD FORCEINLINE const ZBoardObject* BoardObjectPtr() const noexcept { return board_object_ptr_; }

    /*
        Get the tile type.
    */
    NODISCARD virtual UInt64 Type() const noexcept = 0;

    /*
        Destroy the tile.
    */
    virtual Void Destroy() noexcept;

    /*
        Initialize the tile.
    */
    NODISCARD virtual ReturnType Initialize(ZBoard* _owner_board_ptr) noexcept;

    /*
        Returns a copy of the current tile. Must be overrided.
    */
    NODISCARD virtual ZTile* CreateCopy() const noexcept = 0;

    ZBoard* owner_board_ptr_;
    ZBoardObject* board_object_ptr_;
protected:
    using SuperType_ = ZRPGObject;

private:
    ZTile(const ZTile&) = delete;
    ZTile(ZTile&&) = delete;
    ZTile& operator=(const ZTile&) = delete;
    ZTile& operator=(ZTile&&) = delete;
};

}//tsrpg
}//zengine