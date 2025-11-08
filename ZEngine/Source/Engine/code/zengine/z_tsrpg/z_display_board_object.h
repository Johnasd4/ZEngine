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
enum ZDisplayBoardObjectErrorCode : ReturnType {
    kZDisplayBoardObjectErrorCode_LinkError = kErrorCodeBase_ZDisplayBoardObject,
    kZDisplayBoardObjectErrorCode_SystemError,
    kZDisplayBoardObjectErrorCode_NullptrParam,
    kZDisplayBoardObjectErrorCode_ParamOutOfRange,
    kZDisplayBoardObjectErrorCode_LogicTileNotExists
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {

/*
    The objects that are placed on the board. Used for logic calculation.
*/
class TSRPG_DLLAPI ZDisplayBoardObject : public ZBoardObject {
public:
    ZDisplayBoardObject() noexcept;

    virtual ~ZDisplayBoardObject() noexcept;

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

    /*
        Register the display board object to the given logic board object. Call after initialized.
    */
    NODISCARD ReturnType RegisterLogicBoardObject(ZLogicBoardObject* _logic_object_ptr) noexcept;

protected:
    using SuperType_ = ZBoardObject;

private:
    ZDisplayBoardObject(const ZDisplayBoardObject&) = delete;
    ZDisplayBoardObject(ZDisplayBoardObject&&) = delete;
    ZDisplayBoardObject& operator=(const ZDisplayBoardObject&) = delete;
    ZDisplayBoardObject& operator=(ZDisplayBoardObject&&) = delete;

public:
    ZLogicBoardObject* logic_board_object_ptr_;
    //used to listen to logic board object when change happens
    ZDisplayBoardObject* next_display_board_object_ptr_;
    ZDisplayBoardObject* pre_display_board_object_ptr_;
};

}//tsrpg
}//zengine