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

#include "z_separator.h"

namespace zengine {
namespace gui {

ZSeparator::ZSeparator() noexcept 
    : SuperType_() 
    , colour_(kDefaultColour)
{}

ZSeparator::ZSeparator(ZSeparator&& _text) noexcept 
    : SuperType_(std::forward<ZSeparator>(_text))
{
    MoveP(std::forward<ZSeparator>(_text));
}

ZSeparator::~ZSeparator() noexcept {}

ZSeparator& ZSeparator::operator=(ZSeparator&& _text) noexcept {
    SuperType_::operator=(std::forward<ZSeparator>(_text));
    MoveP(std::forward<ZSeparator>(_text));
    return *this;
}

Void ZSeparator::Begin() noexcept {
    SuperType_::Begin();
}

Void ZSeparator::Tick(Float32 _delta_sec) noexcept {
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
            ImGuiCol_Separator,
            ImVec4(
                colour_.red_,
                colour_.green_,
                colour_.blue_,
                colour_.alpha_
            )
        );
    }
    else {
        ImGui::PushStyleColor(
            ImGuiCol_Separator,
            ImVec4(
                colour_.red_ * kDisableColourFactor,
                colour_.green_ * kDisableColourFactor,
                colour_.blue_ * kDisableColourFactor,
                colour_.alpha_
            )
        );
    }

    ImGui::SetWindowFontScale(FontScale());

    ImGui::Separator();

    ImVec2 temp_pos = ImGui::GetItemRectMin();
    ImVec2 temp_size = ImGui::GetItemRectSize();

    if (OwnerPtr() != nullptr) {
        ZGuiObject::SetPos(GuiPos(temp_pos.x, temp_pos.y) - OwnerPtr()->AbsPos());
    }
    ZGuiObject::SetSize(GuiSize(temp_size.x, temp_size.y));

    ImGui::PopStyleColor();
}

Void ZSeparator::Reset() noexcept {
    SuperType_::Reset();
}

Void ZSeparator::SetColour(GuiColour _colour) noexcept {
    colour_ = _colour;
}

NODISCARD ZSeparator::WidgetTypeEnum_ ZSeparator::WidgetType() const noexcept {
    return WidgetTypeEnum_::kWidgetType_Separator;
}

NODISCARD GuiColour ZSeparator::Colour() const noexcept {
    return colour_;
}

Void ZSeparator::MoveP(ZSeparator&& _obj) noexcept {
    colour_ = _obj.colour_;
    _obj.colour_ = kDefaultColour;
}

}//gui
}//zengine