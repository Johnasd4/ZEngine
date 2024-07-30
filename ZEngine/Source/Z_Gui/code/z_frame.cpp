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
    , frame_level_(kBaseFrameLevel)
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
    , frame_level_(kBaseFrameLevel)
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
    Bool if_sub_frame_show = (frame_level_ != kBaseFrameLevel && Visiable());
    //base frame update size and pos
    if (frame_level_ == kBaseFrameLevel) {
        ImVec2 frame_size = ImGui::GetWindowSize();
        ImVec2 frame_pos = ImGui::GetWindowPos();
        GuiSize temp_size = { frame_size.x, frame_size.y };
        GuiPos temp_pos = { frame_pos.x, frame_pos.y };

        //update size
        if (SizeChanged()) {
            ImGui::SetWindowSize(ImVec2(cur_size.width_, cur_size.height_));
        }
        else {
            if (temp_size != cur_size) {
                SetSize(temp_size);
            }
        }

        //update pos
        if (PosChanged()) {
            ImGui::SetWindowPos(ImVec2(cur_pos.x_, cur_pos.y_));
        }
        else {
            if (temp_pos != cur_pos) {
                SetPos(temp_pos);
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
            frame_ptr->Tick(_delta_sec);
        }

    }
    //sub frame update size and pos
    else if(if_sub_frame_show){
        GuiPos abs_pos = OwnerPtr()->Pos() + cur_pos;
        //update size
        ImGui::SetNextWindowSize(ImVec2(cur_size.width_, cur_size.height_), ImGuiCond_Always);

        //update pos
        ImGui::SetNextWindowPos(ImVec2(abs_pos.x_, abs_pos.y_), ImGuiCond_Always);

        SuperType_::Tick(_delta_sec);

        //sub frame begin
        ImGui::BeginChild(Name(), ImVec2(Width(), Height()), true, FrameFlag());

        if (Enabled()) {
            //tick widgets
            for (auto widget_ptr_iter = widget_ptr_set_.Begin(); widget_ptr_iter != widget_ptr_set_.End(); ++widget_ptr_iter) {
                ZWidgetObject* widget_ptr = *widget_ptr_iter;
                widget_ptr->Tick(_delta_sec);
            }

            //tick frame
            for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
                ZFrame* frame_ptr = *frame_ptr_iter;
                frame_ptr->Tick(_delta_sec);
            }
        }

        //sub frame end
        ImGui::EndChild();
    }
}

Void ZFrame::Reset() noexcept {
    SuperType_::Reset();

    //reset widgets
    for (auto widget_ptr_iter = widget_ptr_set_.Begin(); widget_ptr_iter != widget_ptr_set_.End(); ++widget_ptr_iter) {
        ZWidgetObject* widget_ptr = *widget_ptr_iter;
        widget_ptr->Reset();
    }

    //reset frame
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->Reset();
    }
}

Void ZFrame::Add(ZWidgetObject* _widget_obj) noexcept {
    if (WidgetType() == kWidgetTypeFrame) {
        frame_ptr_set_.Insert(static_cast<ZFrame*>(_widget_obj));
        dynamic_cast<ZFrame*>(_widget_obj)->UpdateFrameLevelP(frame_level_);
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

Void ZFrame::MoveP(ZFrame&& _obj) noexcept {
    frame_flag_ = _obj.frame_flag_;
    frame_level_ = _obj.frame_level_;
    frame_background_colour_ = _obj.frame_background_colour_;
    widget_ptr_set_ = std::move(_obj.widget_ptr_set_);
    frame_ptr_set_ = std::move(_obj.frame_ptr_set_);
    _obj.frame_flag_ = kDefaultFrameFlag;
    _obj.frame_level_ = kBaseFrameLevel;
    _obj.frame_background_colour_ = kDefaultFrameBackGroundColour;
}

Void ZFrame::UpdateFrameLevelP(Int32 _super_frame_level) noexcept {
    frame_level_ = _super_frame_level + 1;
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->UpdateFrameLevelP(frame_level_);
    }
}

}//gui
}//zengine