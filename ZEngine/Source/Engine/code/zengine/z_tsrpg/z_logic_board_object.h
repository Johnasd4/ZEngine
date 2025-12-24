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

#include "z_board_object.h"

namespace zengine {
namespace error_code {
enum ZLogicBoardObjectErrorCodeEnum : ReturnType {
    kZLogicBoardObjectErrorCode_LinkError = kErrorCodeBase_ZLogicBoardObject,
    kZLogicBoardObjectErrorCode_SystemOrLibraryError,
    kZLogicBoardObjectErrorCode_NullptrParam,
    kZLogicBoardObjectErrorCode_ParamOutOfRange,
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {
class ZDisplayBoardObject;
}//tsrpg
}//zengine

namespace zengine {
namespace tsrpg {

/*
    The objects that are placed on the board. Used for logic calculation.
*/
class TSRPG_DLLAPI ZLogicBoardObject : public ZBoardObject {
public:
    ZLogicBoardObject() noexcept;

    virtual ~ZLogicBoardObject() noexcept;

    /*
        Get the board object type.
    */
    NODISCARD virtual RPGObjectType Type() const noexcept;

    /*
        Destroy the board object.
    */
    virtual Void Destroy() noexcept;

    /*
        Initialize the board object.
    */
    NODISCARD virtual ReturnType Initialize(ZTile* _owner_tile_ptr) noexcept;

protected:
    using SuperType_ = ZBoardObject;

private:
    ZLogicBoardObject(const ZLogicBoardObject&) = delete;
    ZLogicBoardObject(ZLogicBoardObject&&) = delete;
    ZLogicBoardObject& operator=(const ZLogicBoardObject&) = delete;
    ZLogicBoardObject& operator=(ZLogicBoardObject&&) = delete;

public:
    Bool visible_;
    Bool attackable_;
    Bool if_obstacle_;

    ZDisplayBoardObject* display_board_object_head_ptr_;
};

}//tsrpg
}//zengine