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

#include "../z_core/t_function.h"

#include "z_widget_object.h"


namespace zengine {
namespace error_code {
enum ZCheckBoxErrorCode : ReturnType {
    kZCheckBoxErrorCode_LinkError = kErrorCodeBase_ZCheckBox,
    kZCheckBoxErrorCode_SystemError,
    kZCheckBoxErrorCode_NullptrParam,
    kZCheckBoxErrorCode_ParamOutOfRange,
};
}//error_code
}//zengine

namespace zengine {
namespace gui {

/*
    Check Box class.
*/
class GUI_DLLAPI ZCheckBox : public ZWidgetObject {
public:
    static constexpr GuiColour kDefaultTextColour = { 1.0f, 1.0f, 1.0f, 1.0f };
    static constexpr GuiColour kDefaultCheckMarkColour = { 1.0f, 1.0f, 1.0f, 1.0f };
    static constexpr GuiColour kDefaultCheckBoxColour = { 0.3f, 0.3f, 0.3f, 1.0f };
    static constexpr GuiColour kDefaultHoveredColour = { 0.5f, 0.5f, 0.5f, 1.0f };
    static constexpr GuiColour kDefaultActiveColour = { 0.4f, 0.4f, 0.4f, 1.0f };

    ZCheckBox() noexcept;
    ZCheckBox(ZCheckBox&& _check_box) noexcept;
    /*
        Add ## at the start of the name if you want an empty input text.
    */
    ZCheckBox(const Char* _name, GuiPos _pos = kBasePos) noexcept;

    ~ZCheckBox() noexcept;

    ZCheckBox& operator=(ZCheckBox&& _check_box) noexcept;

    FORCEINLINE Void BindClickFunction(const TFunction<Void(Bool)>& _click_func) noexcept {
        click_func_ = _click_func; 
    }
    FORCEINLINE Void SetIfChecked(Bool _if_checked) noexcept {
        if_checked_ = _if_checked;
    }

    FORCEINLINE NODISCARD Bool IfChecked() noexcept { return if_checked_; }

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

    virtual Void SetTextColour(GuiColour _colour) noexcept;
    virtual Void SetCheckMarkColour(GuiColour _colour) noexcept;
    virtual Void SetCheckBoxColour(GuiColour _colour) noexcept;
    virtual Void SetHoveredColour(GuiColour _colour) noexcept;
    virtual Void SetActiveColour(GuiColour _colour) noexcept;

    NODISCARD virtual WidgetTypeEnum_ WidgetType() const noexcept;

    NODISCARD virtual GuiColour TextColour() const noexcept;
    NODISCARD virtual GuiColour CheckMarkColour() const noexcept;
    NODISCARD virtual GuiColour CheckBoxColour() const noexcept;
    NODISCARD virtual GuiColour HoveredColour() const noexcept;
    NODISCARD virtual GuiColour ActiveColour() const noexcept;

    virtual Void OnCheckBoxClicked(Bool _if_checked) noexcept;

protected:
    using SuperType_ = ZWidgetObject;

private:
    ZCheckBox(const ZCheckBox&) = delete;
    ZCheckBox& operator=(const ZCheckBox&) = delete;

    Void MoveP(ZCheckBox&& _text) noexcept;

    GuiColour text_colour_;
    GuiColour check_mark_colour_;
    GuiColour check_box_colour_;
    GuiColour hovered_colour_;
    GuiColour active_colour_;
    TFunction<Void(Bool)> click_func_;
    Bool if_checked_;
    Bool pre_if_checked_;
};

}//gui
}//zengine