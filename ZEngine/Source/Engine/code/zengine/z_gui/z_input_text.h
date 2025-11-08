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
#pragma once

#include "drive.h"

#include "../z_core/z_memory.h"

#include "z_widget_object.h"

namespace zengine {
namespace error_code {
enum ZInputTextErrorCode : ReturnType {
    kZInputTextErrorCode_LinkError = kErrorCodeBase_ZInputText,
    kZInputTextErrorCode_SystemError,
    kZInputTextErrorCode_NullptrParam,
    kZInputTextErrorCode_ParamOutOfRange,
};
}//error_code
}//zengine

namespace zengine {
namespace gui {

/*
    Input text class.
*/
class GUI_DLLAPI ZInputText : public ZWidgetObject {
public:
    /*
        The button react mode enum.
    */
    enum InputTextFlagEnum_ : Int32 {
        // Basic filters (also see ImGuiInputTextFlags_CallbackCharFilter)
        kInputTextFlag_None = 0,
        // Allow 0123456789.+-*
        kInputTextFlag_CharsDecimal = 1 << 0,
        // Allow 0123456789ABCDEFabcdef
        kInputTextFlag_CharsHexadecimal = 1 << 1,
        // Allow 0123456789.+-*/eE (Scientific notation input)
        kInputTextFlag_CharsScientific = 1 << 2,
        // Turn a..z into A..Z
        kInputTextFlag_CharsUppercase = 1 << 3,
        // Filter out spaces, tabs
        kInputTextFlag_CharsNoBlank = 1 << 4,

        // Inputs
        // Pressing TAB input a '\t' character into the text field
        kInputTextFlag_AllowTabInput = 1 << 5,
        // Return 'true' when Enter is pressed (as opposed to every time the value was modified). 
        // Consider looking at the IsItemDeactivatedAfterEdit() function.
        kInputTextFlag_EnterReturnsTruee = 1 << 6,
        // Escape key clears content if not empty, and deactivate otherwise 
        // (contrast to default behavior of Escape to revert) 
        kInputTextFlag_EscapeClearsAll = 1 << 7,
        // In multi-line mode, validate with Enter, add new line with Ctrl+Enter (default is opposite: validate with Ctrl+Enter, add line with Enter).
        kInputTextFlag_CtrlEnterForNewLine = 1 << 8,

        // Other options
        // Read-only mode
        kInputTextFlag_ReadOnly = 1 << 9,
        // Password mode, display all characters as '*', disable copy
        kInputTextFlag_Password = 1 << 10,
        // Overwrite mode 
        kInputTextFlag_AlwaysOverwrite = 1 << 11,
        // AutoSelectAll
        kInputTextFlag_AutoSelectAll = 1 << 12,
        // InputFloat(), InputInt(), InputScalar() etc. only: parse empty string as zero value.
        kInputTextFlag_ParseEmptyRefVal = 1 << 13,
        // InputFloat(), InputInt(), InputScalar() etc. only: when value is zero, do not display it. 
        // Generally used with ImGuiInputTextFlags_ParseEmptyRefVal.
        kInputTextFlag_DisplayEmptyRefVal = 1 << 14,
        // Disable following the cursor horizontally
        kInputTextFlag_NoHorizontalScroll = 1 << 15,
        // Disable undo/redo. Note that input text owns the text data while active, 
        // if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().
        kInputTextFlag_NoUndoRedo = 1 << 16,

        // Callback features(when used, OnInputTextChanged() will be called.)
        // Callback on pressing TAB (for completion handling)
        kInputTextFlag_CallbackCompletion = 1 << 17,
        // Callback on pressing Up/Down arrows (for history handling)
        kInputTextFlag_CallbackHistory = 1 << 18,
        // Callback on each iteration. User code may query cursor position, modify text buffer.
        kInputTextFlag_CallbackAlways = 1 << 19,
        // Callback on character inputs to replace or discard them. 
        // Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
        kInputTextFlag_CallbackCharFilter = 1 << 20,
        // Callback on buffer capacity changes request (beyond 'buf_size' parameter value), 
        // allowing the string to grow. Notify when the string wants to be resized 
        // (for string types which hold a cache of their Size). 
        // You will be provided a new BufSize in the callback and NEED to honor it. 
        // (see misc/cpp/imgui_stdlib.h for an example of using this)
        kInputTextFlag_CallbackResize = 1 << 21,
        // Callback on any edit (note that InputText() already returns true on edit, 
        // the callback is useful mainly to manipulate the underlying buffer while focus is active)
        kInputTextFlag_CallbackEdit = 1 << 22
    };

    static constexpr GuiColour kDefaultTextColour = { 1.0f, 1.0f, 1.0f, 1.0f };
    static constexpr GuiColour kDefaultBackgroundColour = { 0.5f, 0.5f, 0.5f, 1.0f };
    static constexpr UInt32 kDefaultInputTextSize = 112;
    static constexpr Int32 kDefaultInputTextFlag = kInputTextFlag_None;

    ZInputText() noexcept;
    ZInputText(ZInputText&& _input_text) noexcept;
    /*
        Add ## at the start of the name if you want an empty input text.
    */
    ZInputText(const Char* _name, GuiPos _pos = kBasePos) noexcept;

    ~ZInputText() noexcept;

    ZInputText& operator=(ZInputText&& _input_text) noexcept;

    /*
        Use | to combine all the flags that needs.
    */
    FORCEINLINE Void SetInputTextFlag(Int32 _input_text_flag) noexcept { input_text_flag_ = _input_text_flag; }
    /*
        Can only set one flag at a time.
    */
    FORCEINLINE Void SetInputTextFlag(InputTextFlagEnum_ _input_text_flag, Bool _if_active) noexcept {
        _if_active ? input_text_flag_ |= _input_text_flag : input_text_flag_ &= ~_input_text_flag;
    }

    NODISCARD FORCEINLINE Int32 InputTextFlag() const noexcept { return input_text_flag_; }

    /*
        Called before objects first tick.
    */
    virtual Void Begin() noexcept;
    /*
        Ticks every frame, used for rendering.
    */
    virtual Void Tick(Float32 _delta_sec) noexcept;
    /*
        Resets the object.
    */
    virtual Void Reset() noexcept;

    virtual Void SetHeight(Float32 _height) noexcept;
    virtual Void SetSize(GuiSize _size) noexcept;
    virtual Void SetTextColour(GuiColour _colour) noexcept;
    virtual Void SetBackgroundColour(GuiColour _colour) noexcept;
    virtual Void SetInputTextBufferSize(UInt32 _buffer_size) noexcept;
    virtual Void SetInputText(const Char* _input_text) noexcept;

    NODISCARD virtual WidgetTypeEnum_ WidgetType() const noexcept;

    NODISCARD virtual const Char* InputText() const noexcept;
    NODISCARD virtual GuiColour TextColour() const noexcept;
    NODISCARD virtual GuiColour BackgroundColour() const noexcept;
    NODISCARD virtual SizeType InputTextBufferSize() const noexcept;

    virtual Void OnInputTextChanged() noexcept;

protected:
    using SuperType_ = ZWidgetObject;

private:
    ZInputText(const ZInputText&) = delete;
    ZInputText& operator=(const ZInputText&) = delete;

    Void MoveP(ZInputText&& _text) noexcept;
    Void InputTextChangedCallbackP() noexcept;

    GuiColour text_colour_;
    GuiColour background_colour_;
    ZMemory input_text_;
    Int32 input_text_flag_;
    Bool if_input_text_changed_;
    Bool if_multiline_;

};

}//gui
}//zengine