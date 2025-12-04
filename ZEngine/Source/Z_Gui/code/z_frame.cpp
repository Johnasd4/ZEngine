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

#include "z_frame.h"

namespace zengine {
namespace gui {

ZFrame::ZFrame() noexcept 
    : SuperType_()
    , frame_flag_(kDefaultFrameFlag)
    , frame_level_(kBaseFrameLevel)
    , frame_background_colour_(kDefaultFrameBackgroundColour)
    , scroll_pre_x_(0.0f)
    , scroll_pre_y_(0.0f)
    , scroll_x_(0.0f)
    , scroll_y_(0.0f)
    , scroll_max_x_(0.0f)
    , scroll_max_y_(0.0f)
    , scroll_x_set_(false)
    , scroll_y_set_(false)
    , widget_ptr_set_()
    , frame_ptr_set_() {}

ZFrame::ZFrame(ZFrame&& _frame) noexcept 
    : SuperType_(std::forward<ZFrame>(_frame)), frame_ptr_set_()
{
    MoveP(std::forward<ZFrame>(_frame));
}

ZFrame::ZFrame(ZStringView _name, GuiSize _size, GuiPos _pos) noexcept
    : SuperType_(_name, _size, _pos)
    , frame_flag_(kDefaultFrameFlag)
    , frame_level_(kBaseFrameLevel)
    , frame_background_colour_(kDefaultFrameBackgroundColour)
    , scroll_pre_x_(0.0f)
    , scroll_pre_y_(0.0f)
    , scroll_x_(0.0f)
    , scroll_y_(0.0f)
    , scroll_max_x_(0.0f)
    , scroll_max_y_(0.0f)
    , scroll_x_set_(false)
    , scroll_y_set_(false)
    , widget_ptr_set_()
    , frame_ptr_set_() {}

ZFrame::~ZFrame() noexcept {}

ZFrame& ZFrame::operator=(ZFrame&& _frame) noexcept {
    if (this == &_frame) {
        return *this;
    }
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

    if (!Visiable()) {
        return;
    }

    //base frame update size and pos
    if (frame_level_ == kBaseFrameLevel) {
        //push background colour
        if (Enabled()) {
            ImGui::PushStyleColor(
                ImGuiCol_WindowBg,
                ImVec4(
                    frame_background_colour_.red_,
                    frame_background_colour_.green_,
                    frame_background_colour_.blue_,
                    frame_background_colour_.alpha_
                )
            );
        }
        else {
            ImGui::PushStyleColor(
                ImGuiCol_WindowBg,
                ImVec4(
                    frame_background_colour_.red_ * kDisableColourFactor,
                    frame_background_colour_.green_ * kDisableColourFactor,
                    frame_background_colour_.blue_ * kDisableColourFactor,
                    frame_background_colour_.alpha_
                )
            );
        }
        //begin base frame
        ImGui::Begin(Name().String(), nullptr, frame_flag_);

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

            //tick widgets not added
            TickWidget(_delta_sec);
        }

        //font scale
        ImGui::SetWindowFontScale(FontScale());

        GuiSize cur_size = Size();
        GuiPos cur_pos = Pos();
        ImVec2 frame_size = ImGui::GetWindowSize();
        ImVec2 frame_pos = ImGui::GetWindowPos();
        GuiSize temp_size = { frame_size.x, frame_size.y };
        GuiPos temp_pos = { frame_pos.x, frame_pos.y };

        //update size
        if (SizeSet() && SizeChanged()) {
            ImGui::SetWindowSize(ImVec2(cur_size.width_, cur_size.height_));
        }
        else {
            if (temp_size != cur_size) {
                ZGuiObject::SetSize(temp_size);
            }
        }

        //update pos
        if (PosSet() && PosChanged()) {
            ImGui::SetWindowPos(ImVec2(cur_pos.x_, cur_pos.y_));
        }
        else {
            if (temp_pos != cur_pos) {
                ZGuiObject::SetPos(temp_pos);
            }
        }

        SuperType_::Tick(_delta_sec);

        scroll_x_ = scroll_x_ <= scroll_max_x_ ? scroll_x_ : scroll_max_x_;
        scroll_y_ = scroll_y_ <= scroll_max_y_ ? scroll_y_ : scroll_max_y_;
        scroll_x_ = scroll_x_ >= 0.0F ? scroll_x_ : 0.0F;
        scroll_y_ = scroll_y_ >= 0.0F ? scroll_y_ : 0.0F;


        scroll_x_set_ &= scroll_pre_x_ != scroll_x_;
        scroll_y_set_ &= scroll_pre_y_ != scroll_y_;

        if (scroll_x_set_) {
            scroll_x_set_ = false;
            ImGui::SetScrollX(scroll_x_);
        }
        else {
            scroll_x_ = ImGui::GetScrollX();
        }

        if (scroll_y_set_) {
            scroll_y_set_ = false;
            ImGui::SetScrollY(scroll_y_);
        }
        else {
            scroll_y_ = ImGui::GetScrollY();
        }

        if (scroll_pre_x_ != scroll_x_ || scroll_pre_y_ != scroll_y_) {
            OnScrollChanged(scroll_pre_x_, scroll_pre_y_, scroll_x_, scroll_y_);
        }

        scroll_pre_x_ = scroll_x_;
        scroll_pre_y_ = scroll_y_;

        scroll_max_x_ = ImGui::GetScrollMaxX();
        scroll_max_y_ = ImGui::GetScrollMaxY();

        //end base frame
        ImGui::End();
        //pop background colour
        ImGui::PopStyleColor();
    }
    //sub frame update size and pos
    else {
        //update size, must be called every tick to work
        if (SizeSet()) {
            GuiSize cur_size = Size();
            ImGui::SetNextWindowSize(ImVec2(cur_size.width_, cur_size.height_), ImGuiCond_Always);
        }

        //update pos, must be called every tick to work
        if (PosSet()) {
            GuiPos abs_pos = AbsPos();
            ImGui::SetNextWindowPos(ImVec2(abs_pos.x_, abs_pos.y_), ImGuiCond_Always);
        }

        //push background colour
        GuiColour bg_colour = BackgruondColour();
        ImGui::PushStyleColor(ImGuiCol_ChildBg, *reinterpret_cast<ImVec4*>(&bg_colour));

        //sub frame begin
        if (SizeSet()) {
            ImGui::BeginChild(Name().String(), ImVec2(Width(), Height()), true, frame_flag_);
        }
        else {
            ImGui::BeginChild(
                Name().String(),
                ImVec2(0, 0), 
                true, 
                frame_flag_
            );
        }

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

            //tick widgets not added
            TickWidget(_delta_sec);
        }

        //update size
        if (!SizeSet()) {
            ImVec2 frame_size = ImGui::GetWindowSize();
            GuiSize temp_size = { frame_size.x, frame_size.y };
            ZGuiObject::SetSize(temp_size);
        }

        //update pos
        if (!PosSet()) {
            ImVec2 frame_pos = ImGui::GetWindowPos();
            GuiPos temp_pos = { frame_pos.x, frame_pos.y };
            GuiPos abs_pos = AbsPos();
            if (temp_pos != abs_pos) {
                ZGuiObject::SetAbsPos(temp_pos);
            }
        }

        SuperType_::Tick(_delta_sec);

        scroll_x_ = scroll_x_ <= scroll_max_x_ ? scroll_x_ : scroll_max_x_;
        scroll_y_ = scroll_y_ <= scroll_max_y_ ? scroll_y_ : scroll_max_y_;
        scroll_x_ = scroll_x_ >= 0.0F ? scroll_x_ : 0.0F;
        scroll_y_ = scroll_y_ >= 0.0F ? scroll_y_ : 0.0F;

        scroll_x_set_ &= scroll_pre_x_ != scroll_x_;
        scroll_y_set_ &= scroll_pre_y_ != scroll_y_;

        if (scroll_x_set_) {
            scroll_x_set_ = false;
            ImGui::SetScrollX(scroll_x_);
        }
        else {
            scroll_x_ = ImGui::GetScrollX();
        }

        if (scroll_y_set_) {
            scroll_y_set_ = false;
            ImGui::SetScrollY(scroll_y_);
        }
        else {
            scroll_y_ = ImGui::GetScrollY();
        }

        if (scroll_pre_x_ != scroll_x_ || scroll_pre_y_ != scroll_y_) {
            OnScrollChanged(scroll_pre_x_, scroll_pre_y_, scroll_x_, scroll_y_);
        }

        scroll_pre_x_ = scroll_x_;
        scroll_pre_y_ = scroll_y_;

        scroll_max_x_ = ImGui::GetScrollMaxX();
        scroll_max_y_ = ImGui::GetScrollMaxY();

        //sub frame end
        ImGui::EndChild();
        //pop background colour
        ImGui::PopStyleColor();
    }
}

Void ZFrame::TickWidget(Float32 _delta_sec) noexcept {}

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

ReturnType ZFrame::Add(ZWidgetObject* _widget_obj_ptr) noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        _widget_obj_ptr == nullptr,
        error_code::kZFrameErrorCode_NullptrParam,
        L"_widget_obj is nullptr!"
    );

    if (_widget_obj_ptr->WidgetType() == ZWidgetObject::WidgetTypeEnum_::kFrame) {
        frame_ptr_set_.Insert(static_cast<ZFrame*>(_widget_obj_ptr));
        dynamic_cast<ZFrame*>(_widget_obj_ptr)->UpdateFrameLevelP(frame_level_);
    }
    else {
        widget_ptr_set_.Insert(_widget_obj_ptr);
    }
    _widget_obj_ptr->OnAdd(this);

    return ret_val;
}

ReturnType ZFrame::Remove(ZWidgetObject* _widget_obj_ptr) noexcept {
    ReturnType ret_val = kOK;
    Z_CHECK(
        _widget_obj_ptr == nullptr,
        error_code::kZFrameErrorCode_NullptrParam,
        L"_widget_obj is nullptr!"
    );

    if (_widget_obj_ptr->WidgetType() == ZWidgetObject::WidgetTypeEnum_::kFrame) {
        frame_ptr_set_.Erase(static_cast<ZFrame*>(_widget_obj_ptr));
    }
    else {
        widget_ptr_set_.Erase(_widget_obj_ptr);
    }
    _widget_obj_ptr->OnRemove(this);

    return ret_val;
}

Void ZFrame::RemoveAll() noexcept {
    ReturnType ret_val = kOK;

    TArray<ZWidgetObject*> remove_widget_ptr_array;
    for (auto iter = frame_ptr_set_.Begin(); iter != frame_ptr_set_.End(); ++iter) {
        remove_widget_ptr_array.PushBack(*iter);
    }
    for (auto iter = widget_ptr_set_.Begin(); iter != widget_ptr_set_.End(); ++iter) {
        remove_widget_ptr_array.PushBack(*iter);
    }
    frame_ptr_set_.Clear();
    widget_ptr_set_.Clear();
    for (auto iter = remove_widget_ptr_array.Begin(); iter != remove_widget_ptr_array.End(); ++iter) {
        (*iter)->OnRemove(this);
    }
}

Void ZFrame::SetBackgruondColour(GuiColour _colour) noexcept {
    frame_background_colour_ = _colour;
}
Void ZFrame::SetScrollX(Float32 _scroll_x) noexcept {
    scroll_x_ = _scroll_x;
    scroll_x_set_ = true;
}
Void ZFrame::SetScrollY(Float32 _scroll_y) noexcept {
    scroll_y_ = _scroll_y;
    scroll_y_set_ = true;
}

NODISCARD ZFrame::WidgetTypeEnum_ ZFrame::WidgetType() const noexcept {
    return WidgetTypeEnum_::kFrame;
}

NODISCARD GuiColour ZFrame::BackgruondColour() const noexcept {
    return frame_background_colour_;
}
NODISCARD Float32 ZFrame::ScrollX() const noexcept {
    return scroll_x_;
}
NODISCARD Float32 ZFrame::ScrollY() const noexcept {
    return scroll_y_;
}
NODISCARD Float32 ZFrame::ScrollMaxX() const noexcept {
    return scroll_max_x_;
}
NODISCARD Float32 ZFrame::ScrollMaxY() const noexcept {
    return scroll_max_y_;
}
Void ZFrame::OnKeyDown(KeyEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {
    SuperType_::OnKeyDown(_clicked_button, _shift, _ctrl, _alt);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnKeyDown(_clicked_button, _shift, _ctrl, _alt);
    }
}

Void ZFrame::OnKeyUp(KeyEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {
    SuperType_::OnKeyUp(_clicked_button, _shift, _ctrl, _alt);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnKeyUp(_clicked_button, _shift, _ctrl, _alt);
    }
}

Void ZFrame::OnKeyPress(KeyEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {
    SuperType_::OnKeyPress(_clicked_button, _shift, _ctrl, _alt);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnKeyPress(_clicked_button, _shift, _ctrl, _alt);
    }
}

Void ZFrame::OnMouseDown(MouseButtonEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {
    SuperType_::OnMouseDown(_clicked_button, _shift, _ctrl, _alt);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnMouseDown(_clicked_button, _shift, _ctrl, _alt);
    }
}

Void ZFrame::OnMouseUp(MouseButtonEnum _clicked_button, Bool _shift, Bool _ctrl, Bool _alt) noexcept {
    SuperType_::OnMouseUp(_clicked_button, _shift, _ctrl, _alt);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnMouseUp(_clicked_button, _shift, _ctrl, _alt);
    }
}

Void ZFrame::OnScrollMove(Float32 _x_offset, Float32 _y_offset) noexcept {
    SuperType_::OnScrollMove(_x_offset, _y_offset);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnScrollMove(_x_offset, _y_offset);
    }
}

Void ZFrame::OnMouseMove(GuiPos _pre_pos, GuiPos _cur_pos) noexcept {
    SuperType_::OnMouseMove(_pre_pos, _cur_pos);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnMouseMove(_pre_pos, _cur_pos);
    }
}

Void ZFrame::OnScrollChanged(
    Float32 _pre_scroll_x,
    Float32 _pre_scroll_y,
    Float32 _cur_scroll_x,
    Float32 _cur_scroll_y
) noexcept 
{}

Void ZFrame::MoveP(ZFrame&& _obj) noexcept {
    frame_flag_ = _obj.frame_flag_;
    frame_level_ = _obj.frame_level_;
    frame_background_colour_ = _obj.frame_background_colour_;
    widget_ptr_set_ = std::move(_obj.widget_ptr_set_);
    frame_ptr_set_ = std::move(_obj.frame_ptr_set_);
    _obj.frame_flag_ = kDefaultFrameFlag;
    _obj.frame_level_ = kBaseFrameLevel;
    _obj.frame_background_colour_ = kDefaultFrameBackgroundColour;
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