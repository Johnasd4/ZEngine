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

#include "z_widget_object.h"

namespace zengine {
namespace error_code {
enum ZSeparatorErrorCode : ReturnType {
    kZSeparatorErrorCode_LinkError = kErrorCodeBase_ZSeparator,
    kZSeparatorErrorCode_SystemError,
    kZSeparatorErrorCode_NullptrParam,
    kZSeparatorErrorCode_ParamOutOfRange,
};
}//error_code
}//zengine

namespace zengine {
namespace gui {

/*
    Text class.
*/
class GUI_DLLAPI ZSeparator : public ZWidgetObject {
public:
    static constexpr GuiColour kDefaultColour = { 1.0f, 1.0f, 1.0f, 1.0f };

    ZSeparator() noexcept;
    ZSeparator(ZSeparator&& _seeparator) noexcept;

    ~ZSeparator() noexcept;

    ZSeparator& operator=(ZSeparator&& _text) noexcept;

    /*
        Called before objects first tick.
    */
    virtual Void Begin() noexcept;
    /*
        Ticks every frame, used for rendering.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;
    /*
        Resets the object.
    */
    virtual Void Reset() noexcept;

    virtual Void SetColour(GuiColour _colour) noexcept;

    NODISCARD virtual WidgetTypeEnum_ WidgetType() const noexcept;

    NODISCARD virtual GuiColour Colour() const noexcept;

protected:
    using SuperType_ = ZWidgetObject;

private:
    ZSeparator(const ZSeparator&) = delete;
    ZSeparator& operator=(const ZSeparator&) = delete;

    Void MoveP(ZSeparator&& _text) noexcept;

    GuiColour colour_;
};

}//gui
}//zengine