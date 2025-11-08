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

#include "z_input_text.h"

namespace zengine {
namespace gui {

ZInputText::ZInputText() noexcept 
    : SuperType_() 
    , text_colour_(kDefaultTextColour)
    , background_colour_(kDefaultBackgroundColour)
    , input_text_(kDefaultInputTextSize)
    , input_text_flag_(kDefaultInputTextFlag)
    , if_input_text_changed_(false)
    , if_multiline_(false)
{
    input_text_.Clear();
}

ZInputText::ZInputText(ZInputText&& _input_text) noexcept
    : SuperType_(std::forward<ZInputText>(_input_text))
{
    MoveP(std::forward<ZInputText>(_input_text));
}

ZInputText::ZInputText(const Char* _name, GuiPos _pos) noexcept
    : SuperType_(_name, kBaseSize, _pos)
    , text_colour_(kDefaultTextColour)
    , background_colour_(kDefaultBackgroundColour)
    , input_text_(kDefaultInputTextSize)
    , input_text_flag_(kDefaultInputTextFlag)
    , if_input_text_changed_(false)
    , if_multiline_(false)
{
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

    if (PosSet() && PosChanged()) {
        GuiPos pos = Pos();
        ImGui::SetCursorPos(ImVec2(pos.x_, pos.y_));
    }

    SuperType_::Tick(_delta_sec);

    if (Enabled()) {
        ImGui::PushStyleColor(
            ImGuiCol_Text, ImVec4(text_colour_.red_, text_colour_.green_, text_colour_.blue_, text_colour_.alpha_)
        );
        ImGui::PushStyleColor(
            ImGuiCol_FrameBg,
            ImVec4(
                background_colour_.red_,
                background_colour_.green_,
                background_colour_.blue_,
                background_colour_.alpha_
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
        ImGui::PushStyleColor(
            ImGuiCol_FrameBg,
            ImVec4(
                background_colour_.red_ * kDisableColourFactor,
                background_colour_.green_ * kDisableColourFactor,
                background_colour_.blue_ * kDisableColourFactor,
                background_colour_.alpha_
            )
        );
    }

    ImGui::SetWindowFontScale(FontScale());
    ZGuiObject::CallbackGuiObjectPtr() = this;
    if (if_multiline_) {
        GuiSize size = Size();
        ImGui::InputTextMultiline(
            Name(), 
            input_text_.DataPtr<Char*>(),
            input_text_.Size(), 
            ImVec2(size.width_, size.height_),
            input_text_flag_,
            [](ImGuiInputTextCallbackData* _data) {
                (dynamic_cast<ZInputText*>(ZGuiObject::CallbackGuiObjectPtr()))->InputTextChangedCallbackP();
                return 0;
            }
        );
    }
    else {
        if (SizeSet()) {
            ImGui::SetNextItemWidth(Width());
        }
        Int32 flag = input_text_flag_;
        if (!Enabled()) {
            flag = kInputTextFlag_ReadOnly;
        }
        ImGui::InputText(
            Name(), input_text_.DataPtr<Char*>(), input_text_.Size(), flag,
            [](ImGuiInputTextCallbackData* _data) {
                (dynamic_cast<ZInputText*>(ZGuiObject::CallbackGuiObjectPtr()))->InputTextChangedCallbackP();
                return 0;
            }
        );

    }

    ImVec2 temp_pos = ImGui::GetItemRectMin();
    ImVec2 temp_size = ImGui::GetItemRectSize();

    ZGuiObject::SetPos(GuiPos(temp_pos.x, temp_pos.y));
    ZGuiObject::SetSize(GuiSize(temp_size.x, temp_size.y));

    if (if_input_text_changed_) {
        OnInputTextChanged();
        if_input_text_changed_ = false;
    }
    ImGui::PopStyleColor(2);
}

Void ZInputText::Reset() noexcept {
    SuperType_::Reset();
}

Void ZInputText::SetHeight(Float32 _height) noexcept {
    SuperType_::SetHeight(_height);
    if_multiline_ = true;
}

Void ZInputText::SetSize(GuiSize _size) noexcept {
    SuperType_::SetSize(_size);
    if_multiline_ = true;
}

Void ZInputText::SetTextColour(GuiColour _colour) noexcept {
    text_colour_ = _colour;
}

Void ZInputText::SetBackgroundColour(GuiColour _colour) noexcept {
    background_colour_ = _colour;
}

Void ZInputText::SetInputTextBufferSize(UInt32 _buffer_size) noexcept {
    input_text_.Resize(_buffer_size);
}

Void ZInputText::SetInputText(const Char* _input_text) noexcept {
    strcpy(input_text_.DataPtr<Char*>(), _input_text);
}

NODISCARD ZInputText::WidgetTypeEnum_ ZInputText::WidgetType() const noexcept {
    return WidgetTypeEnum_::kWidgetType_InputText;
}

NODISCARD const Char* ZInputText::InputText() const noexcept {
    return input_text_.DataPtr<const Char*>();
}

NODISCARD GuiColour ZInputText::TextColour() const noexcept {
    return text_colour_;
}

NODISCARD GuiColour ZInputText::BackgroundColour() const noexcept {
    return background_colour_;
}

NODISCARD SizeType ZInputText::InputTextBufferSize() const noexcept {
    return input_text_.Size();
}

Void ZInputText::OnInputTextChanged() noexcept {}

Void ZInputText::MoveP(ZInputText&& _input_text) noexcept {
    text_colour_ = _input_text.text_colour_;
    background_colour_ = _input_text.background_colour_;
    input_text_flag_ = _input_text.input_text_flag_;
    if_input_text_changed_ = _input_text.if_input_text_changed_;
    if_multiline_ = _input_text.if_multiline_;
    input_text_ = std::move(_input_text.input_text_);
    _input_text.text_colour_ = kDefaultTextColour;
    _input_text.background_colour_ = kDefaultBackgroundColour;
    _input_text.input_text_flag_ = kDefaultInputTextFlag;
    _input_text.if_input_text_changed_ = false;
    _input_text.if_multiline_ = false;
}

Void ZInputText::InputTextChangedCallbackP() noexcept {
    if_input_text_changed_ = true;
}

}//gui
}//zengine