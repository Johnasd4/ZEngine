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

enum ZWorldBoardObjectErrorCode : ReturnType {
    kZWorldBoardObjectErrorCode_LinkError = kErrorCodeBase_ZWorldBoardObject
};

}//error_code

/*
    The objects that are placed on the board. Used for logic calculation.
*/
class TSRPG_DLLAPI ZWorldBoardObject : public ZBoardObject {
public:
    ZWorldBoardObject() noexcept;

    virtual ~ZWorldBoardObject() noexcept;

    NODISCARD FORCEINLINE ZLogicBoardObject* LogicObjectPtr() noexcept { return logic_object_ptr_; }
    NODISCARD FORCEINLINE const ZLogicBoardObject* LogicObjectPtr() const noexcept { return logic_object_ptr_; }

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

    ZLogicBoardObject* logic_object_ptr_;
protected:
    using SuperType_ = ZBoardObject;

private:
    ZWorldBoardObject(const ZWorldBoardObject&) = delete;
    ZWorldBoardObject(ZWorldBoardObject&&) = delete;
    ZWorldBoardObject& operator=(const ZWorldBoardObject&) = delete;
    ZWorldBoardObject& operator=(ZWorldBoardObject&&) = delete;
};

}//tsrpg
}//zengine