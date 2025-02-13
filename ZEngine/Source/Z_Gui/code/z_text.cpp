/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#define GUI_DLLFILE

#include "z_text.h"

#include "../z_core/m_log.h"

namespace zengine {
namespace gui {

ZText::ZText() noexcept 
    : SuperType_() 
    , text_colour_(kDefaultTextColour)
    , font_scale_(kDefaultFontScale) {}

ZText::ZText(ZText&& _text) noexcept 
    : SuperType_(std::forward<ZText>(_text))
    , text_colour_(kDefaultTextColour)
    , font_scale_(kDefaultFontScale)
{
    MoveP(std::forward<ZText>(_text));
}

ZText::ZText(const Char* _name, GuiPos _pos) noexcept
    : SuperType_(_name, { 0.0f, 0.0f }, _pos)
    , text_colour_(kDefaultTextColour)
    , font_scale_(kDefaultFontScale) {}

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

    if (Enabled()) {
        GuiPos pos = Pos();
        ImGui::SetCursorPos(ImVec2(pos.x_, pos.y_));

        SuperType_::Tick(_delta_sec);

        ImGui::PushStyleColor(
            ImGuiCol_Text, ImVec4(text_colour_.red_, text_colour_.green_, text_colour_.blue_, text_colour_.alpha_)
        );
        ImGui::SetWindowFontScale(font_scale_ * kFontScaleMultFactor);
        ImGui::Text(Name());
        ImGui::PopStyleColor();
    }
}

Void ZText::Reset() noexcept {
    SuperType_::Reset();
}

Void ZText::SetTextColour(GuiColour _colour) noexcept {
    text_colour_ = _colour;
}

Void ZText::SetFontScale(Float32 _scale) noexcept {
    font_scale_ = _scale;
}

NODISCARD ZText::WidgetTypeEnum ZText::WidgetType() const noexcept {
    return WidgetTypeEnum::kWidgetTypeText;
}

NODISCARD GuiColour ZText::TextColour() const noexcept {
    return text_colour_;
}

NODISCARD Float32 ZText::FontScale() const noexcept {
    return font_scale_;
}

Void ZText::MoveP(ZText&& _obj) noexcept {
    text_colour_ = _obj.text_colour_;
    font_scale_ = _obj.font_scale_;
    _obj.text_colour_ = kDefaultTextColour;
    _obj.font_scale_ = kDefaultFontScale;
}

}//gui
}//zengine