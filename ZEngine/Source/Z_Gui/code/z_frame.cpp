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

#include "z_frame.h"

#include "../z_core/m_log.h"

namespace zengine {
namespace gui {

ZFrame::ZFrame() noexcept 
    : SuperType_()
    , frame_flag_(kDefaultFrameFlag)
    , frame_state_(kFrameStateTerminated)
    , frame_background_colour_(kDefaultFrameBackGroundColour)
    , widget_ptr_set_()
    , frame_ptr_set_() {}

ZFrame::ZFrame(ZFrame&& _frame) noexcept 
    : SuperType_(std::forward<ZFrame>(_frame)), frame_ptr_set_()
{
    MoveP(std::forward<ZFrame>(_frame));
}

ZFrame::ZFrame(const Char* _name, GuiSize _size, GuiPos _pos) noexcept
    : SuperType_(_name, _size, _pos)
    , frame_flag_(kDefaultFrameFlag)
    , frame_state_(kFrameStateOpened)
    , frame_background_colour_(kDefaultFrameBackGroundColour)
    , widget_ptr_set_()
    , frame_ptr_set_() {}

ZFrame::~ZFrame() noexcept {}

ZFrame& ZFrame::operator=(ZFrame&& _frame) noexcept {
    SuperType_::operator=(std::forward<ZFrame>(_frame));
    MoveP(std::forward<ZFrame>(_frame));
    return *this;
}

Void ZFrame::Begin() noexcept {
    SuperType_::Begin();

    //begin widgets
    for (auto widget_ptr_iter = widget_ptr_set_.Begin(); widget_ptr_iter != widget_ptr_set_.End(); ++widget_ptr_iter) {
        ZWidgetObject* widget_ptr = *widget_ptr_iter;
        widget_ptr->Begin();
    }

    //begin frame
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->Begin();
    }
}

Void ZFrame::Tick(Float32 _delta_sec) noexcept {
    GuiSize cur_size = Size();
    GuiPos cur_pos = Pos();
    ImVec2 frame_size = ImGui::GetWindowSize();
    ImVec2 frame_pos = ImGui::GetWindowPos();
    GuiSize temp_size = { static_cast<Int32>(frame_size.x), static_cast<Int32>(frame_size.y) };
    GuiPos temp_pos = { static_cast<Int32>(frame_size.x), static_cast<Int32>(frame_size.y) };

    //update size, system events first
    if (temp_size != cur_size) {
        SetSize(temp_size);
    }
    else {
        if (SizeChanged()) {
            ImGui::SetWindowSize(ImVec2(static_cast<Float32>(cur_size.width_), static_cast<Float32>(cur_size.height_)));
        }
    }

    //update pos, system events first
    if (temp_pos != cur_pos) {
        SetPos(temp_pos);
    }
    else {
        if (PosChanged()) {
            ImGui::SetWindowPos(ImVec2(static_cast<Float32>(cur_pos.x_), static_cast<Float32>(cur_pos.y_)));
        }
    }

    SuperType_::Tick(_delta_sec);

    //tick widgets
    for (auto widget_ptr_iter = widget_ptr_set_.Begin(); widget_ptr_iter != widget_ptr_set_.End(); ++widget_ptr_iter) {
        ZWidgetObject* widget_ptr = *widget_ptr_iter;
        widget_ptr->Tick(_delta_sec);
    }

    //tick frame
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        Bool if_open = false;
        if (frame_ptr->Enabled()) {
            ImGui::Begin(frame_ptr->Name(), &if_open, frame_ptr->FrameFlag());
            if (if_open) {
                if (frame_ptr->frame_state_ = kFrameStateOpened) {
                    frame_ptr->Tick(_delta_sec);
                }
            }
            else {
                frame_ptr->Close();
            }
            ImGui::End();
        }
    }
}

Void ZFrame::Hide() noexcept {
    SuperType_::Hide();
    frame_state_ = kFrameStateHidden;
}

Void ZFrame::Show() noexcept {
    SuperType_::Show();
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    switch (frame_state_) {
    case kFrameStateTerminated:
        Z_LOG_ERROR(error_code::kZFrameErrorCodeFrameNotExist, 0, L"Frame does not exist!");
        break;
    case kFrameStateOpened:
        break;
    case kFrameStateClosed:
        Begin();
        frame_state_ = kFrameStateOpened;
        break;
    case kFrameStateHidden:
        frame_state_ = kFrameStateOpened;
        break;
    }
}

Void ZFrame::Reset() noexcept {
    SuperType_::Reset();
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->Reset();
    }
}

Void ZFrame::Close() noexcept {
    OnClose();
    frame_state_ = kFrameStateClosed;
}

Void ZFrame::Destroy() noexcept {
    OnDestroy();
    frame_flag_ = kDefaultFrameFlag;
    frame_background_colour_ = kDefaultFrameBackGroundColour;
    widget_ptr_set_.Clear();
    frame_ptr_set_.Clear();
    frame_state_ = kFrameStateTerminated;
}

Void ZFrame::Add(ZWidgetObject* _widget_obj) noexcept {
    if (WidgetType() == kWidgetTypeFrame) {
        frame_ptr_set_.Insert(static_cast<ZFrame*>(_widget_obj));
    }
    else {
        widget_ptr_set_.Insert(_widget_obj);
    }
    _widget_obj->OnAdd(this);
}

Void ZFrame::SetBackgruondColour(GuiColour _colour) noexcept {
    frame_background_colour_ = _colour;
}

NODISCARD ZFrame::WidgetTypeEnum ZFrame::WidgetType() const noexcept {
    return WidgetTypeEnum::kWidgetTypeFrame;
}

NODISCARD GuiColour ZFrame::BackgruondColour() const noexcept {
    return frame_background_colour_;
}

Void ZFrame::OnClose() noexcept {}
Void ZFrame::OnDestroy() noexcept {}

Void ZFrame::MoveP(ZFrame&& _obj) noexcept {
    frame_flag_ = _obj.frame_flag_;
    frame_background_colour_ = _obj.frame_background_colour_;
    frame_state_ = _obj.frame_state_;
    widget_ptr_set_ = std::move(_obj.widget_ptr_set_);
    frame_ptr_set_ = std::move(_obj.frame_ptr_set_);
    _obj.frame_flag_ = kDefaultFrameFlag;
    _obj.frame_background_colour_ = kDefaultFrameBackGroundColour;
    _obj.frame_state_ = kFrameStateTerminated;
}

}//gui
}//zengine