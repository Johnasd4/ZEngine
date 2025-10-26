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
namespace error_code {
enum ZBoardObjectErrorCode : ReturnType {
    kZBoardObjectErrorCode_LinkError = kErrorCodeBase_ZBoardObject,
    kZBoardObjectErrorCode_SystemError,
    kZBoardObjectErrorCode_NullptrParam,
    kZBoardObjectErrorCode_ParamOutOfRange,
    kZBoardObjectErrorCode_OnwerTileNotInitialized
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {

/*
    The base class objects that are placed on the board.
*/
class TSRPG_DLLAPI ZBoardObject : public ZRPGObject {
public:
    ZBoardObject() noexcept;

    virtual ~ZBoardObject() noexcept;

    /*
        Get the board object type.
    */
    NODISCARD virtual RPGObjectType Type() const noexcept = 0;

    /*
        Destroy the board object.
    */
    virtual Void Destroy() noexcept;

protected:
    using SuperType_ = ZRPGObject;

    /*
        Initialize the board object.
    */
    NODISCARD virtual ReturnType InitializeP(ZTile* _owner_tile_ptr) noexcept;

private:
    ZBoardObject(const ZBoardObject&) = delete;
    ZBoardObject(ZBoardObject&&) = delete;
    ZBoardObject& operator=(const ZBoardObject&) = delete;
    ZBoardObject& operator=(ZBoardObject&&) = delete;

public:
    ZTile* owner_tile_ptr_;
    ZBoardObject* pre_object_ptr_;
    ZBoardObject* next_object_ptr_;
};

}//tsrpg
}//zengine