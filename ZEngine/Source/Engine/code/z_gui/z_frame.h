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
    static constexpr Int32 kDefaultFrameLevel = 1;
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

    NODISCARD FORCEINLINE Int32 FrameLevel() const noexcept { return frame_level_; }

    /*
        Will be called when the object is added to another object.
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

    virtual Void SetWidth(Int32 _width) noexcept;
    virtual Void SetHeight(Int32 _height) noexcept;
    virtual Void SetSize(Int32 _width, Int32 _height) noexcept;
    virtual Void SetXPos(Int32 _x_pos) noexcept;
    virtual Void SetYPos(Int32 _y_pos) noexcept;
    virtual Void SetPos(Int32 _x_pos, Int32 _y_pos) noexcept;

    virtual Void SetBackgruondColour(Float32 _red, Float32 _green, Float32 _blue, Float32 _alpha) noexcept;

    /*
        Use | to combine all the flags that needs.
    */
    virtual Void SetFrameFlag(Int32 _frame_flag) noexcept;

    NODISCARD virtual Int32 Width() const noexcept;
    NODISCARD virtual Int32 Height() const noexcept;
    NODISCARD virtual GuiSize Size() const noexcept;
    NODISCARD virtual Int32 XPos() const noexcept;
    NODISCARD virtual Int32 YPos() const noexcept;
    NODISCARD virtual GuiPos Pos() const noexcept;

    NODISCARD virtual GuiColour BackgruondColour() const noexcept;
    NODISCARD virtual Int32 FrameFlag() const noexcept;

    virtual Void OnAdd(ZGuiObject* _owner_ptr) noexcept;

    virtual Void OnClose() noexcept;
    virtual Void OnDestroy() noexcept;

protected:
    using SuperType_ = ZWidgetObject;

private:
    ZFrame(const ZFrame&) = delete;
    ZFrame& operator=(const ZFrame&) = delete;

    Void MoveP(ZFrame&& _frame) noexcept;

    /*
        Updates the current frame level and all the sub frame level.
    */
    Void UpdateFrameLevel() noexcept;

    Int32 frame_level_;
    Int32 frame_flag_;
    GuiColour frame_background_colour_;
    TSet<ZWidgetObject*> widget_ptr_set_;
    TSet<ZFrame*> frame_ptr_set_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_FRAME_H_
