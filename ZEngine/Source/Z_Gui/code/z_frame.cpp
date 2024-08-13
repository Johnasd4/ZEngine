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
    , background_colour_(kDefaultBackGroundColour)
    , widget_ptr_set_()
    , frame_ptr_set_() {}

ZFrame::ZFrame(ZFrame&& _frame) noexcept 
    : SuperType_(std::forward<ZFrame>(_frame))
{
    MoveP(std::forward<ZFrame>(_frame));
}

ZFrame::ZFrame(const Char* _name, GuiSize _size, GuiPos _pos) noexcept
    : SuperType_(_name, _size, _pos)
    , frame_flag_(kDefaultFrameFlag)
    , frame_level_(kBaseFrameLevel)
    , background_colour_(kDefaultBackGroundColour)
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

    if (!Visiable()) {
        return;
    }

    //base frame update size and pos
    if (frame_level_ == kBaseFrameLevel) {
        //push background colour
        ImGui::PushStyleColor(ImGuiCol_WindowBg, *reinterpret_cast<ImVec4*>(&background_colour_));
        //begin base frame
        ImGui::Begin(Name(), nullptr, frame_flag_);

        if (Enabled()) {
            GuiSize cur_size = Size();
            GuiPos cur_pos = Pos();
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

        ImGui::BeginMainMenuBar();

        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("New");
            ImGui::MenuItem("Open");
            ImGui::MenuItem("Save");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Undo");
            ImGui::MenuItem("Redo");
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();

        //if (ImGui::BeginTabBar("ImPlotDemoTabs")) {
        //    if (ImGui::BeginTabItem("Plots")) {
        //        //DemoHeader("Line Plots", Demo_LinePlots);
        //        //DemoHeader("Filled Line Plots", Demo_FilledLinePlots);
        //        //DemoHeader("Shaded Plots##", Demo_ShadedPlots);
        //        //DemoHeader("Scatter Plots", Demo_ScatterPlots);
        //        //DemoHeader("Realtime Plots", Demo_RealtimePlots);
        //        //DemoHeader("Stairstep Plots", Demo_StairstepPlots);
        //        //DemoHeader("Bar Plots", Demo_BarPlots);
        //        //DemoHeader("Bar Groups", Demo_BarGroups);
        //        //DemoHeader("Bar Stacks", Demo_BarStacks);
        //        //DemoHeader("Error Bars", Demo_ErrorBars);
        //        //DemoHeader("Stem Plots##", Demo_StemPlots);
        //        //DemoHeader("Infinite Lines", Demo_InfiniteLines);
        //        //DemoHeader("Pie Charts", Demo_PieCharts);
        //        //DemoHeader("Heatmaps", Demo_Heatmaps);
        //        //DemoHeader("Histogram", Demo_Histogram);
        //        //DemoHeader("Histogram 2D", Demo_Histogram2D);
        //        //DemoHeader("Digital Plots", Demo_DigitalPlots);
        //        //DemoHeader("Images", Demo_Images);
        //        //DemoHeader("Markers and Text", Demo_MarkersAndText);
        //        //DemoHeader("NaN Values", Demo_NaNValues);
        //        ImGui::EndTabItem();
        //    }
        //    if (ImGui::BeginTabItem("Subplots")) {
        //        //DemoHeader("Sizing", Demo_SubplotsSizing);
        //        //DemoHeader("Item Sharing", Demo_SubplotItemSharing);
        //        //DemoHeader("Axis Linking", Demo_SubplotAxisLinking);
        //        //DemoHeader("Tables", Demo_Tables);
        //        ImGui::EndTabItem();
        //    }
        //    if (ImGui::BeginTabItem("Axes")) {
        //        //DemoHeader("Log Scale", Demo_LogScale);
        //        //DemoHeader("Symmetric Log Scale", Demo_SymmetricLogScale);
        //        //DemoHeader("Time Scale", Demo_TimeScale);
        //        //DemoHeader("Custom Scale", Demo_CustomScale);
        //        //DemoHeader("Multiple Axes", Demo_MultipleAxes);
        //        //DemoHeader("Tick Labels", Demo_TickLabels);
        //        //DemoHeader("Linked Axes", Demo_LinkedAxes);
        //        //DemoHeader("Axis Constraints", Demo_AxisConstraints);
        //        //DemoHeader("Equal Axes", Demo_EqualAxes);
        //        //DemoHeader("Auto-Fitting Data", Demo_AutoFittingData);
        //        ImGui::EndTabItem();
        //    }
        //    if (ImGui::BeginTabItem("Tools")) {
        //        //DemoHeader("Offset and Stride", Demo_OffsetAndStride);
        //        //DemoHeader("Drag Points", Demo_DragPoints);
        //        //DemoHeader("Drag Lines", Demo_DragLines);
        //        //DemoHeader("Drag Rects", Demo_DragRects);
        //        //DemoHeader("Querying", Demo_Querying);
        //        //DemoHeader("Annotations", Demo_Annotations);
        //        //DemoHeader("Tags", Demo_Tags);
        //        //DemoHeader("Drag and Drop", Demo_DragAndDrop);
        //        //DemoHeader("Legend Options", Demo_LegendOptions);
        //        //DemoHeader("Legend Popups", Demo_LegendPopups);
        //        //DemoHeader("Colormap Widgets", Demo_ColormapWidgets);
        //        ImGui::EndTabItem();
        //    }
        //    if (ImGui::BeginTabItem("Custom")) {
        //        //DemoHeader("Custom Styles", Demo_CustomStyles);
        //        //DemoHeader("Custom Data and Getters", Demo_CustomDataAndGetters);
        //        //DemoHeader("Custom Rendering", Demo_CustomRendering);
        //        //DemoHeader("Custom Plotters and Tooltips", Demo_CustomPlottersAndTooltips);
        //        ImGui::EndTabItem();
        //    }
        //    if (ImGui::BeginTabItem("Config")) {
        //        // Demo_Config();
        //        ImGui::EndTabItem();
        //    }
        //    if (ImGui::BeginTabItem("Help")) {
        //        // Demo_Help();
        //        ImGui::EndTabItem();
        //    }
        //    ImGui::EndTabBar();
        //}
        //end base frame
        ImGui::End();
        //pop background colour
        ImGui::PopStyleColor();
    }
    //sub frame update size and pos
    else {
        GuiSize cur_size = Size();
        GuiPos cur_pos = Pos();
        GuiPos abs_pos = AbsPos();
        //update size
        ImGui::SetNextWindowSize(ImVec2(cur_size.width_, cur_size.height_), ImGuiCond_Always);

        //update pos
        ImGui::SetNextWindowPos(ImVec2(abs_pos.x_, abs_pos.y_), ImGuiCond_Always);

        if (Enabled()) {
            SuperType_::Tick(_delta_sec);
        }

        //push background colour
        GuiColour bg_colour = BackgruondColour();
        ImGui::PushStyleColor(ImGuiCol_ChildBg, *reinterpret_cast<ImVec4*>(&bg_colour));
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
        //pop background colour
        ImGui::PopStyleColor();
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
    if (_widget_obj->WidgetType() == kWidgetTypeFrame) {
        frame_ptr_set_.Insert(static_cast<ZFrame*>(_widget_obj));
        dynamic_cast<ZFrame*>(_widget_obj)->UpdateFrameLevelP(frame_level_);
    }
    else {
        widget_ptr_set_.Insert(_widget_obj);
    }
    _widget_obj->OnAdd(this);
}

Void ZFrame::SetBackgruondColour(GuiColour _colour) noexcept {
    background_colour_ = _colour;
}

NODISCARD ZFrame::WidgetTypeEnum ZFrame::WidgetType() const noexcept {
    return WidgetTypeEnum::kWidgetTypeFrame;
}

NODISCARD GuiColour ZFrame::BackgruondColour() const noexcept {
    return background_colour_;
}

Void ZFrame::OnKeyDown(KeyEnum _clicked_button, Int32 _mods) noexcept {
    SuperType_::OnKeyDown(_clicked_button, _mods);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnKeyDown(_clicked_button, _mods);
    }
}

Void ZFrame::OnKeyUp(KeyEnum _clicked_button, Int32 _mods) noexcept {
    SuperType_::OnKeyUp(_clicked_button, _mods);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnKeyUp(_clicked_button, _mods);
    }
}

Void ZFrame::OnKeyPress(KeyEnum _clicked_button, Int32 _mods) noexcept {
    SuperType_::OnKeyPress(_clicked_button, _mods);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnKeyPress(_clicked_button, _mods);
    }
}

Void ZFrame::OnMouseDown(MouseButtonEnum _clicked_button, Int32 _mods) noexcept {
    SuperType_::OnMouseDown(_clicked_button, _mods);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnMouseDown(_clicked_button, _mods);
    }
}

Void ZFrame::OnMouseUp(MouseButtonEnum _clicked_button, Int32 _mods) noexcept {
    SuperType_::OnMouseUp(_clicked_button, _mods);
    for (auto frame_ptr_iter = frame_ptr_set_.Begin(); frame_ptr_iter != frame_ptr_set_.End(); ++frame_ptr_iter) {
        ZFrame* frame_ptr = *frame_ptr_iter;
        frame_ptr->OnMouseUp(_clicked_button, _mods);
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
    for (auto widget_ptr_iter = widget_ptr_set_.Begin(); widget_ptr_iter != widget_ptr_set_.End(); ++widget_ptr_iter) {
        ZWidgetObject* widget_ptr = *widget_ptr_iter;
        widget_ptr->OnMouseMove(_pre_pos, _cur_pos);
    }
}

Void ZFrame::MoveP(ZFrame&& _frame) noexcept {
    frame_flag_ = _frame.frame_flag_;
    frame_level_ = _frame.frame_level_;
    background_colour_ = _frame.background_colour_;
    widget_ptr_set_ = std::move(_frame.widget_ptr_set_);
    frame_ptr_set_ = std::move(_frame.frame_ptr_set_);
    _frame.frame_flag_ = kDefaultFrameFlag;
    _frame.frame_level_ = kBaseFrameLevel;
    _frame.background_colour_ = kDefaultBackGroundColour;
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