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

#include "z_board_object.h"

namespace zengine {
namespace tsrpg {

namespace error_code {

enum ZLogicBoardObjectErrorCode : ReturnType {
    kZLogicBoardObjectErrorCode_LinkError = kErrorCodeBase_ZLogicBoardObject
};

}//error_code

/*
    The objects that are placed on the board. Used for logic calculation.
*/
class TSRPG_DLLAPI ZLogicBoardObject : public ZBoardObject {
public:
    ZLogicBoardObject() noexcept;

    virtual ~ZLogicBoardObject() noexcept;

    NODISCARD FORCEINLINE ZWorldBoardObject* WorldObjectPtr() noexcept { return world_object_ptr_; }
    NODISCARD FORCEINLINE const ZWorldBoardObject* WorldObjectPtr() const noexcept { return world_object_ptr_; }
    NODISCARD FORCEINLINE Bool Visible() const noexcept { return visible_; }
    NODISCARD FORCEINLINE Bool Attackable() const noexcept { return attackable_; }
    NODISCARD FORCEINLINE Bool IfObstacle() const noexcept { return if_obstacle_; }

    /*
        Get the board object type.
    */
    NODISCARD virtual UInt64 Type() const noexcept;

    /*
        Destroy the board object.
    */
    virtual Void Destroy() noexcept;

    /*
        Initialize the board object.
    */
    NODISCARD virtual ReturnType Initialize(ZTile* _owner_tile_ptr) noexcept;

    ZWorldBoardObject* world_object_ptr_;
    Bool visible_;
    Bool attackable_;
    Bool if_obstacle_;
protected:
    using SuperType_ = ZBoardObject;

private:
    ZLogicBoardObject(const ZLogicBoardObject&) = delete;
    ZLogicBoardObject(ZLogicBoardObject&&) = delete;
    ZLogicBoardObject& operator=(const ZLogicBoardObject&) = delete;
    ZLogicBoardObject& operator=(ZLogicBoardObject&&) = delete;
};

}//tsrpg
}//zengine