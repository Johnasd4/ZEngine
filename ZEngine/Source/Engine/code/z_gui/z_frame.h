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
    static constexpr Int32 kDefaultFrameFlag = 1;   //TODO
    static constexpr GuiColour kDefaultFrameBackGroundColour = { 0.0f,0.0f,0.0f,0.0f };   //TODO

    /*
        The frame screen mode enum.
    */
    enum FrameFlagEnum_ {

    };

    /*
        The frame state enum.
    */
    enum FrameStateEnum_ {
        kFrameStateTerminated,
        kFrameStateOpened,
        kFrameStateClosed,
        kFrameStateHidden
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
    NODISCARD FORCEINLINE FrameStateEnum_ FrameState() const noexcept { return frame_state_; }
    
    /*
        Will be called when the application executes.
    */
    virtual Void Begin() noexcept;
    /*
        Ticks every frame.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;
    /*
        Hides the frame.
    */
    virtual Void Hide() noexcept;
    /*
        Shows the frame, if the frame was closed, will call begin().
    */
    virtual Void Show() noexcept;
    /*
        Resets the object.
    */
    virtual Void Reset() noexcept;
    
    /*
        Close the frame, calls reset when the frame is opened again.
    */
    virtual Void Close() noexcept;
    /*
        Destroy the frame, release the resourses.
    */
    virtual Void Destroy() noexcept;
    /*
        Adds a widget to the frame.
    */
    virtual Void Add(ZWidgetObject* _widget_obj) noexcept;

    virtual Void SetBackgruondColour(GuiColour _colour) noexcept;

    NODISCARD virtual WidgetTypeEnum WidgetType() const noexcept;

    NODISCARD virtual GuiColour BackgruondColour() const noexcept;

    virtual Void OnClose() noexcept;
    virtual Void OnDestroy() noexcept;

protected:
    using SuperType_ = ZWidgetObject;

private:
    ZFrame(const ZFrame&) = delete;
    ZFrame& operator=(const ZFrame&) = delete;

    Void MoveP(ZFrame&& _frame) noexcept;

    Int32 frame_flag_;
    GuiColour frame_background_colour_;
    TSet<ZWidgetObject*> widget_ptr_set_;
    TSet<ZFrame*> frame_ptr_set_;
    FrameStateEnum_ frame_state_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_FRAME_H_
