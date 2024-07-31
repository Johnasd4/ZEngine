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
#ifndef Z_GUI_Z_FRAME_H_
#define Z_GUI_Z_FRAME_H_

#include "internal/z_drive.h"

#include "../z_core/t_set.h"
#include "../z_core/z_string.h"

#include "z_widget_object.h"

namespace zengine {
namespace gui {

namespace error_code {

enum ZFrameErrorCode : ReturnType {
    kZFrameErrorCodeLinkError = kErrorCodeBaseZFrame,
    kZFrameErrorCodeFrameAreadyCreated,
    kZFrameErrorCodeFrameNotExist
};

}//error_code

/*
    Frame class.
*/
class GUI_DLLAPI ZFrame : public ZWidgetObject {
public:
    static constexpr Int32 kBaseFrameLevel = 0;

    static constexpr Int32 kDefaultFrameFlag = 1;   //TODO
    static constexpr GuiColour kDefaultFrameBackGroundColour = { 0.0f,0.0f,0.0f,0.0f };   //TODO

    /*
        The frame screen mode enum.
    */
    enum FrameFlagEnum_ {

    };

    ZFrame() noexcept;
    ZFrame(ZFrame&& _frame) noexcept;
    ZFrame(const Char* _name, GuiSize _size, GuiPos _pos) noexcept;

    ~ZFrame() noexcept;

    ZFrame& operator=(ZFrame&& _frame) noexcept;

    /*
        Use | to combine all the flags that needs.
    */
    FORCEINLINE Void SetFrameFlag(Int32 _frame_flag) noexcept { frame_flag_ = _frame_flag; }

    NODISCARD FORCEINLINE Int32 FrameFlag() const noexcept { return frame_flag_; }
    NODISCARD FORCEINLINE Int32 FrameLevel() const noexcept { return frame_flag_; }

    /*
        Will be called when the application executes.
    */
    virtual Void Begin() noexcept;
    /*
        Ticks every frame.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;
    /*
        Resets the object.
    */
    virtual Void Reset() noexcept;

    /*
        Adds a widget to the frame.
    */
    virtual Void Add(ZWidgetObject* _widget_obj) noexcept;

    virtual Void SetBackgruondColour(GuiColour _colour) noexcept;

    NODISCARD virtual WidgetTypeEnum WidgetType() const noexcept;

    NODISCARD virtual GuiColour BackgruondColour() const noexcept;

protected:
    using SuperType_ = ZWidgetObject;

private:
    ZFrame(const ZFrame&) = delete;
    ZFrame& operator=(const ZFrame&) = delete;

    Void MoveP(ZFrame&& _frame) noexcept;

    Void UpdateFrameLevelP(Int32 _super_frame_level) noexcept;

    Int32 frame_flag_;
    Int32 frame_level_;
    GuiColour frame_background_colour_;
    TMultiset<ZWidgetObject*, ZWidgetObjectCompare> widget_ptr_set_;
    TMultiset<ZFrame*, ZWidgetObjectCompare> frame_ptr_set_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_FRAME_H_
