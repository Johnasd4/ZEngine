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

#include "z_core/z_object.h"

namespace zengine {
namespace tsrpg {

namespace error_code {

enum ZRPGObjectErrorCode : ReturnType {
    kZRPGObjectErrorCode_LinkError = kErrorCodeBase_ZRPGObject,
    kZRPGObjectErrorCode_NotInitialized
};

}//error_code

/*
    The base class objects that are placed on the board.
*/
class TSRPG_DLLAPI ZRPGObject : public ZObject {
public:
    ZRPGObject() noexcept;

    virtual ~ZRPGObject() noexcept;

    NODISCARD FORCEINLINE Bool Initialized() const noexcept { return initialized_; }

    NODISCARD Bool IsType(UInt64 _type_enum) const noexcept;

    /*
        Get the rpg object type.
    */
    NODISCARD virtual UInt64 Type() const noexcept = 0;

    /*
        Destroy the rpg object.
    */
    virtual Void Destroy() noexcept;

    /*
        Initialize the rpg object.
    */
    NODISCARD Void Initialize() noexcept;

    Bool initialized_;
protected:
    using SuperType_ = ZObject;

private:
    ZRPGObject(const ZRPGObject&) = delete;
    ZRPGObject(ZRPGObject&&) = delete;
    ZRPGObject& operator=(const ZRPGObject&) = delete;
    ZRPGObject& operator=(ZRPGObject&&) = delete;
};

}//tsrpg
}//zengine