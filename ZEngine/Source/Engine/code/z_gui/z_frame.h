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

#include "../z_core/t_vector.h"
#include "../z_core/z_string.h"

#include "z_gui_object.h"

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
class GUI_DLLAPI ZFrame : public ZGuiObject {
public:
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
    
    ~ZFrame() noexcept;

    ZFrame& operator=(ZFrame&& _frame) noexcept;

    /*
    Will be called when the object is added to another object.
*/
    virtual Void Begin() noexcept;
    /*
        Ticks every frame.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;
    /*
        Hides the window.
    */
    virtual Void Hide() noexcept;
    /*
        Shows the window, if the window was closed, will call begin().
    */
    virtual Void Show() noexcept;
    /*
        Close the window, calls reset when the window is opened again.
    */
    virtual Void Close() noexcept;
    /*
        Destroy the window, release the resourses.
    */
    virtual Void Destroy() noexcept;
    /*
        Adds a frame to the window.
    */
    virtual Void AddFrame(ZFrame* _frame) noexcept;

    virtual Void SetWidth(Int32 _width) noexcept;
    virtual Void SetHeight(Int32 _height) noexcept;
    virtual Void SetSize(Int32 _width, Int32 _height) noexcept;
    virtual Void SetXPos(Int32 _x_pos) noexcept;
    virtual Void SetYPos(Int32 _y_pos) noexcept;
    virtual Void SetPos(Int32 _x_pos, Int32 _y_pos) noexcept;
    virtual Void SetBackgruondColour(Float32 _red, Float32 _green, Float32 _blue, Float32 _alpha) noexcept;
    virtual Void SetName(const Char* _name) noexcept;

    /*
        Use | to combine all the flags that needs.
    */
    virtual Void SetFrameFlag(Int32 _frame_flag) noexcept;
    /*
        Set priority 
    */
    virtual Void SetFrameFlag(Int32 _frame_flag) noexcept;

    NODISCARD virtual Int32 Width() const noexcept;
    NODISCARD virtual Int32 Height() const noexcept;
    NODISCARD virtual GuiSize Size() const noexcept;
    NODISCARD virtual Int32 XPos() const noexcept;
    NODISCARD virtual Int32 YPos() const noexcept;
    NODISCARD virtual GuiPos Pos() const noexcept;
    NODISCARD virtual GuiColour BackgruondColour() const noexcept;
    NODISCARD virtual const Char* Name() const noexcept;

    NODISCARD virtual Int32 FrameFlag() const noexcept;


    virtual Void OnClose() noexcept;
    virtual Void OnDestroy() noexcept;

protected:
    using SuperType_ = ZGuiObject;

private:
    GuiSize size_;
    GuiPos pos_;
    Int32 priority_;
    Int32 frame_flag_;
    ZString name_;
    TVector<ZFrame*> frame_ptr_vec_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_FRAME_H_
