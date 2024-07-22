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

#include "z_gui_object.h"

namespace zengine {
namespace gui {

Void ZGuiObject::Initialize() noexcept {}

Void ZGuiObject::Tick(Float32 _delta_time) noexcept {}

Void ZGuiObject::Reset() noexcept {}

Void ZGuiObject::Hide() noexcept {}
Void ZGuiObject::Show() noexcept {}
Void ZGuiObject::SetWidth(Int32 _width) noexcept {}
Void ZGuiObject::SetHeight(Int32 _height) noexcept {}
Void ZGuiObject::SetXPos(Int32 _x_pos) noexcept {}
Void ZGuiObject::SetYPos(Int32 _y_pos) noexcept {}
Void ZGuiObject::SetSize(Int32 _width, Int32 _height) noexcept {}
Void ZGuiObject::SetPos(Int32 _x_pos, Int32 _y_pos) noexcept {}
Void ZGuiObject::SetBackgruondColour(ColourRGBA _colour) noexcept {}
Void SetBackgruondColour(Int32 _red, Int32 _green, Int32 _blue, Int32 _alpha) noexcept {}

GuiSize ZGuiObject::Size() noexcept { return GuiSize(0, 0); }
GuiPos ZGuiObject::Pos() noexcept { return GuiPos(0, 0); }
Int32 ZGuiObject::Width() noexcept { return 0; }
Int32 ZGuiObject::Height() noexcept { return 0; }
Int32 ZGuiObject::XPos() noexcept { return 0; }
Int32 ZGuiObject::YPos() noexcept { return 0; }
ColourRGBA ZGuiObject::BackgruondColour() noexcept { return ColourRGBA(0, 0, 0, 0); }

Void OnClick(Int32 _shift, Int32 _pos_x, Int32 _pos_y) noexcept {}

}//gui
}//zengine