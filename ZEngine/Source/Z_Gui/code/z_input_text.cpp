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

#include "z_input_text.h"

#include "../z_core/m_log.h"

namespace zengine {
namespace gui {

namespace internal {
    
static thread_local ZInputText* CallbackInputTextPtr = nullptr;

}//internal

ZInputText::ZInputText() noexcept 
    : SuperType_() 
    , text_colour_(kDefaultTextColour)
    , background_colour_(kDefaultBackgroundColour)
    , input_text_(kDefaultInputTextSize)
    , font_scale_(kDefaultFontScale)
    , input_text_flag_(kDefaultInputTextFlag)
    , if_pos_set_(false) 
{
    input_text_.Clear();
}

ZInputText::ZInputText(ZInputText&& _input_text) noexcept
    : SuperType_(std::forward<ZInputText>(_input_text))
{
    MoveP(std::forward<ZInputText>(_input_text));
}

ZInputText::ZInputText(const Char* _name) noexcept
    : SuperType_(_name, { 0.0f, 0.0f }, { 0.0f, 0.0f })
    , text_colour_(kDefaultTextColour)
    , background_colour_(kDefaultBackgroundColour)
    , input_text_(kDefaultInputTextSize)
    , font_scale_(kDefaultFontScale)
    , input_text_flag_(kDefaultInputTextFlag)
    , if_pos_set_(false) {
    if (*_name == '\0') {
        SuperType_::SetName("##");
    }
    input_text_.Clear();
}

ZInputText::ZInputText(const Char* _name, GuiPos _pos) noexcept
    : SuperType_(_name, { 0.0f, 0.0f }, _pos)
    , text_colour_(kDefaultTextColour)
    , background_colour_(kDefaultBackgroundColour)
    , input_text_(kDefaultInputTextSize)
    , font_scale_(kDefaultFontScale)
    , input_text_flag_(kDefaultInputTextFlag)
    , if_pos_set_(true) 
{
    if (*_name == '\0') {
        SuperType_::SetName("##");
    }
    input_text_.Clear();
}

ZInputText::~ZInputText() noexcept {}

ZInputText& ZInputText::operator=(ZInputText&& _input_text) noexcept {
    SuperType_::operator=(std::forward<ZInputText>(_input_text));
    MoveP(std::forward<ZInputText>(_input_text));
    return *this;
}

Void ZInputText::Begin() noexcept {
    SuperType_::Begin();
}

Void ZInputText::Tick(Float32 _delta_sec) noexcept {
    if (!Visiable()) {
        return;
    }

    if (Enabled()) {
        if (if_pos_set_) {
            GuiPos pos = Pos();
            ImGui::SetCursorPos(ImVec2(pos.x_, pos.y_));
        }

        SuperType_::Tick(_delta_sec);

        ImGui::PushStyleColor(
            ImGuiCol_Text, ImVec4(text_colour_.red_, text_colour_.green_, text_colour_.blue_, text_colour_.alpha_)
        );
        ImGui::PushStyleColor(
            ImGuiCol_FrameBg, 
            ImVec4(
                background_colour_.red_, 
                background_colour_.green_, 
                background_colour_.blue_, 
                background_colour_.alpha_)
        );
        ImGui::SetWindowFontScale(font_scale_ * kFontScaleMultFactor);
        internal::CallbackInputTextPtr = this;
        ImGui::InputText(
            Name(), (Char*)input_text_.DataPtr(), input_text_.Size(), input_text_flag_, 
            [](ImGuiInputTextCallbackData* _data) {
                internal::CallbackInputTextPtr->OnInputTextChanged();
                return 0;
            }
        );
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
}

Void ZInputText::Reset() noexcept {
    SuperType_::Reset();
}

Void ZInputText::SetTextColour(GuiColour _colour) noexcept {
    text_colour_ = _colour;
}

Void ZInputText::SetBackgroundColour(GuiColour _colour) noexcept {
    background_colour_ = _colour;
}

Void ZInputText::SetFontScale(Float32 _scale) noexcept {
    font_scale_ = _scale;
}

Void ZInputText::SetInputTextBufferSize(UInt32 _buffer_size) noexcept {
    input_text_.Resize(_buffer_size);
}

NODISCARD ZInputText::WidgetTypeEnum ZInputText::WidgetType() const noexcept {
    return WidgetTypeEnum::kWidgetType_InputText;
}

NODISCARD const Char* ZInputText::InputText() const noexcept {
    return (Char*)(input_text_.DataPtr());
}

NODISCARD GuiColour ZInputText::TextColour() const noexcept {
    return text_colour_;
}

NODISCARD GuiColour ZInputText::BackgroundColour() const noexcept {
    return background_colour_;
}

NODISCARD Float32 ZInputText::FontScale() const noexcept {
    return font_scale_;
} 

NODISCARD UInt32 ZInputText::InputTextBufferSize() const noexcept {
    return input_text_.Size();
}

Void ZInputText::OnInputTextChanged() noexcept {}

Void ZInputText::MoveP(ZInputText&& _input_text) noexcept {
    text_colour_ = _input_text.text_colour_;
    background_colour_ = _input_text.background_colour_;
    font_scale_ = _input_text.font_scale_;
    input_text_flag_ = _input_text.input_text_flag_;
    if_pos_set_ = _input_text.if_pos_set_;
    input_text_ = std::move(_input_text.input_text_);
    _input_text.text_colour_ = kDefaultTextColour;
    _input_text.background_colour_ = kDefaultBackgroundColour;
    _input_text.font_scale_ = kDefaultFontScale;
    _input_text.input_text_flag_ = kDefaultInputTextFlag;
    _input_text.if_pos_set_ = false;
}

}//gui
}//zengine