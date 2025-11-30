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
#define GUI_DLLFILE
#include "drive/d_pch.h"

#include "z_text.h"

namespace zengine {
namespace gui {

ZText::ZText() noexcept 
    : SuperType_() 
    , text_colour_(kDefaultTextColour)
    , if_wrap_(true) {}

ZText::ZText(ZText&& _text) noexcept 
    : SuperType_(std::forward<ZText>(_text))
{
    MoveP(std::forward<ZText>(_text));
}

ZText::ZText(ZStringView _text, GuiPos _pos) noexcept
    : SuperType_(_text, kBaseSize, _pos)
    , text_colour_(kDefaultTextColour)
    , if_wrap_(true) {}

ZText::~ZText() noexcept {}

ZText& ZText::operator=(ZText&& _text) noexcept {
    SuperType_::operator=(std::forward<ZText>(_text));
    MoveP(std::forward<ZText>(_text));
    return *this;
}

Void ZText::Begin() noexcept {
    SuperType_::Begin();
}

Void ZText::Tick(Float32 _delta_sec) noexcept {
    if (!Visiable()) {
        return;
    }

    if (PosSet() && PosChanged()) {
        GuiPos pos = Pos();
        ImGui::SetCursorPos(ImVec2(pos.x_, pos.y_));
    }

    SuperType_::Tick(_delta_sec);

    if (Enabled()) {
        ImGui::PushStyleColor(
            ImGuiCol_Text,
            ImVec4(
                text_colour_.red_,
                text_colour_.green_,
                text_colour_.blue_,
                text_colour_.alpha_
            )
        );
    }
    else {
        ImGui::PushStyleColor(
            ImGuiCol_Text,
            ImVec4(
                text_colour_.red_ * kDisableColourFactor,
                text_colour_.green_ * kDisableColourFactor,
                text_colour_.blue_ * kDisableColourFactor,
                text_colour_.alpha_
            )
        );
    }

    ImGui::SetWindowFontScale(FontScale());
    if (if_wrap_) {
        ImGui::TextWrapped(Name().String());
    }
    else {
        ImGui::Text(Name().String());
    }

    ImVec2 temp_pos = ImGui::GetItemRectMin();
    ImVec2 temp_size = ImGui::GetItemRectSize();

    if (OwnerPtr() != nullptr) {
        ZGuiObject::SetPos(GuiPos(temp_pos.x, temp_pos.y) - OwnerPtr()->AbsPos());
    }
    ZGuiObject::SetSize(GuiSize(temp_size.x, temp_size.y));

    ImGui::PopStyleColor();
}

Void ZText::Reset() noexcept {
    SuperType_::Reset();
}

/*
    Text does't need a name, so puts the text string info the name obj.
*/
Void ZText::SetText(ZStringView _text) noexcept {
    SuperType_::SetName(_text);
}

Void ZText::SetTextColour(GuiColour _colour) noexcept {
    text_colour_ = _colour;
}

NODISCARD ZText::WidgetTypeEnum_ ZText::WidgetType() const noexcept {
    return WidgetTypeEnum_::kText;
}

/*
    Text does't need a name, so puts the text string info the name obj.
*/
NODISCARD const ZString& ZText::Text() const noexcept {
    return SuperType_::Name();
}

NODISCARD GuiColour ZText::TextColour() const noexcept {
    return text_colour_;
}

Void ZText::MoveP(ZText&& _obj) noexcept {
    text_colour_ = _obj.text_colour_;
    if_wrap_ = _obj.if_wrap_;
    _obj.text_colour_ = kDefaultTextColour;
    _obj.if_wrap_ = false;
}

}//gui
}//zengine