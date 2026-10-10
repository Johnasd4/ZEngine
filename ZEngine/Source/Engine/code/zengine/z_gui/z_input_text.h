/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "drive.h"

#include "../z_core/t_function.h"
#include "../z_core/z_memory.h"
#include "../z_core/z_string_view.h"

#include "z_widget_object.h"

namespace zengine {
namespace error_code {

enum ZInputTextErrorCodeEnum : ReturnType::ValueType {
    /** @brief Input text linking error occurred. */
    kZInputTextErrorCode_LinkError = kErrorCodeBase_ZInputText,
    /** @brief System or third-party library error. */
    kZInputTextErrorCode_SystemOrLibraryError,
    /** @brief Null pointer parameter detected. */
    kZInputTextErrorCode_NullptrParam,
    /** @brief Parameter value is out of valid range. */
    kZInputTextErrorCode_ParamOutOfRange,
};

} // error_code
} // zengine

struct ImGuiInputTextCallbackData;

namespace zengine {
namespace gui {

/**
 * @brief Input text widget class.
 *
 * This class is a wrapper around ImGui input text widgets. It manages an internal
 * dynamic text buffer, supports single-line and multi-line modes, and provides
 * automatic buffer extension. Text changes are detected through an ImGui callback
 * and forwarded via a user-defined function.
 */
class GUI_DLLAPI ZInputText : public ZWidgetObject {
public:
    /** @brief Alias for the base widget object type. */
    using SuperType_ = ZWidgetObject;

    /**
     * @brief Input text behavior and filtering flags.
     */
    enum FlagEnum_ : Int32 {
        /** @brief No special behavior enabled. */
        kInputTextFlag_None = 0,
        /** @brief Allow decimal characters only. */
        kInputTextFlag_CharsDecimal = 1 << 0,
        /** @brief Allow hexadecimal characters only. */
        kInputTextFlag_CharsHexadecimal = 1 << 1,
        /** @brief Allow scientific notation characters. */
        kInputTextFlag_CharsScientific = 1 << 2,
        /** @brief Convert all characters to uppercase. */
        kInputTextFlag_CharsUppercase = 1 << 3,
        /** @brief Filter out spaces and tabs. */
        kInputTextFlag_CharsNoBlank = 1 << 4,
        /** @brief Allow tab character input. */
        kInputTextFlag_AllowTabInput = 1 << 5,
        /** @brief Return true only when Enter key is pressed. */
        kInputTextFlag_EnterReturnsTrue = 1 << 6,
        /** @brief Clear content when Escape is pressed. */
        kInputTextFlag_EscapeClearsAll = 1 << 7,
        /** @brief Use Ctrl+Enter to insert a new line in multi-line mode. */
        kInputTextFlag_CtrlEnterForNewLine = 1 << 8,
        /** @brief Enable read-only mode. */
        kInputTextFlag_ReadOnly = 1 << 9,
        /** @brief Enable password display mode. */
        kInputTextFlag_Password = 1 << 10,
        /** @brief Enable overwrite mode. */
        kInputTextFlag_AlwaysOverwrite = 1 << 11,
        /** @brief Automatically select all text on focus. */
        kInputTextFlag_AutoSelectAll = 1 << 12,
        /** @brief Parse empty string as zero for scalar inputs. */
        kInputTextFlag_ParseEmptyRefVal = 1 << 13,
        /** @brief Do not display zero values for scalar inputs. */
        kInputTextFlag_DisplayEmptyRefVal = 1 << 14,
        /** @brief Disable horizontal scrolling. */
        kInputTextFlag_NoHorizontalScroll = 1 << 15,
        /** @brief Disable undo and redo operations. */
        kInputTextFlag_NoUndoRedo = 1 << 16
    };

    /** @brief Default text color. */
    static inline constexpr GuiColour kDefaultTextColour = { 1.0f, 1.0f, 1.0f, 1.0f };
    /** @brief Default background color. */
    static inline constexpr GuiColour kDefaultBackgroundColour = { 0.5f, 0.5f, 0.5f, 1.0f };
    /** @brief Default input text buffer size. */
    static inline constexpr UInt32 kDefaultInputTextSize = 112;
    /** @brief Default input text flag configuration. */
    static inline constexpr Int32 kDefaultInputTextFlag = kInputTextFlag_None;
    /** @brief Buffer auto-extend growth factor. */
    static inline constexpr Int32 kInputTextBufferExtendFactor = 2;

public:
    /**
     * @brief Construct an empty input text widget.
     */
    ZInputText() noexcept;

    /** @brief Deleted copy constructor. */
    ZInputText(const ZInputText&) = delete;

    /** @brief Deleted move constructor. */
    ZInputText(ZInputText&&) = delete;

    /**
     * @brief Construct an input text widget with a name.
     * @param _name Input text label name.
     */
    ZInputText(ZStringView _name) noexcept;

    /**
     * @brief Destroy the input text widget.
     */
    virtual ~ZInputText() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZInputText& operator=(const ZInputText&) = delete;

    /** @brief Deleted move assignment operator. */
    ZInputText& operator=(ZInputText&&) = delete;

    /**
     * @brief Check if multi-line mode is enabled.
     * @return True if multi-line is enabled.
     */
    FORCEINLINE Bool IsMultiline() noexcept { return is_multiline_; }

    /**
     * @brief Check if automatic buffer extension is enabled.
     * @return True if auto extension is enabled.
     */
    FORCEINLINE Bool IsAutoExtendBuffer() noexcept { return is_auto_extend_buffer_; }


    /**
     * @brief Get widget type identifier.
     * @return Widget type enum value.
     */
    NODISCARD virtual WidgetTypeEnum_ GetWidgetType() const noexcept;

    /**
     * @brief Get input text name.
     * @return Input text name.
     */
    NODISCARD FORCEINLINE ZStringView GetName() const noexcept { return name_; }

    /**
     * @brief Get text color.
     * @return Text color.
     */
    NODISCARD FORCEINLINE GuiColour GetTextColour() const noexcept { return text_colour_; }

    /**
     * @brief Get background color.
     * @return Background color.
     */
    NODISCARD FORCEINLINE GuiColour GetBackgroundColour() const noexcept { return background_colour_; }

    /**
     * @brief Get input text buffer size.
     * @return Buffer size.
     */
    NODISCARD FORCEINLINE SizeType GetInputTextBufferSize() const noexcept {
        return input_text_buffer_.GetSize();
    }

    /**
     * @brief Get current input text.
     * @return Input text string view.
     */
    NODISCARD FORCEINLINE ZStringView GetInputText() const noexcept {
        return ZStringView(input_text_buffer_.GetDataPtr<const Char>(), input_text_length_);
    }

    /**
     * @brief Get current input text flags.
     * @return Combined input text flags.
     */
    NODISCARD FORCEINLINE Int32 GetFlag() const noexcept { return flag_; }

    /**
     * @brief Set the input text name.
     * @param _name New input text name.
     */
    FORCEINLINE Void SetName(ZStringView _name) noexcept { name_ = _name; }

    /**
     * @brief Set text color.
     * @param _colour Text color.
     */
    FORCEINLINE Void SetTextColour(GuiColour _colour) noexcept { text_colour_ = _colour; }

    /**
     * @brief Set background color.
     * @param _colour Background color.
     */
    FORCEINLINE Void SetBackgroundColour(GuiColour _colour) noexcept { background_colour_ = _colour; }

    /**
     * @brief Set input text buffer size.
     * @param _buffer_size New buffer size.
     */
    FORCEINLINE Void SetInputTextBufferSize(UInt32 _buffer_size) noexcept {
        input_text_buffer_.Resize(_buffer_size);
    }

    /**
     * @brief Set input text content.
     * @param _input_text Text to set.
     * @return kOK if successful, otherwise an error code.
     */
    Void SetInputText(ZStringView _input_text) noexcept;

    /**
     * @brief Set widget width.
     * @param _width Width value.
     */
    FORCEINLINE Void SetWidth(Float32 _width) noexcept { size_.width_ = _width; }

    /**
     * @brief Set all input text flags at once.
     * @param _flag Combined input text flags.
     */
    FORCEINLINE Void SetFlag(Int32 _flag) noexcept { flag_ = (kDefaultInputTextFlag | _flag); }

    /**
     * @brief Enable or disable a specific input text flag.
     * @param _flag Flag to modify.
     * @param _if_active True to enable, false to disable.
     */
    FORCEINLINE Void SetFlag(FlagEnum_ _flag, Bool _if_active) noexcept {
        _if_active ? flag_ |= _flag : flag_ &= ~_flag;
    }

    /**
     * @brief Enable or disable multi-line mode.
     * @param _is_multiline True to enable multi-line input.
     */
    FORCEINLINE Void SetMultiline(Bool _is_multiline) noexcept { is_multiline_ = _is_multiline; }

    /**
     * @brief Enable or disable automatic buffer extension.
     * @param _is_auto_extend_buffer True to enable auto extension.
     */
    FORCEINLINE Void SetAutoExtendBuffer(Bool _is_auto_extend_buffer) noexcept {
        is_auto_extend_buffer_ = _is_auto_extend_buffer;
    }

    /**
     * @brief Bind a callback function for input text changes.
     * @param _func Callback function.
     */
    FORCEINLINE Void BindInputTextChangedFunc(TFunction<Void(ZStringView)> _func) noexcept {
        input_text_changed_func_ = std::move(_func);
    }

    /**
     * @brief Tick function called every frame.
     * @param _delta_sec Delta time in seconds.
     * @return kOK if successful, otherwise an error code.
     */
    virtual Void OnTick(Float32 _delta_sec) noexcept;

    /**
     * @brief Called when input text content changes.
     * @return kOK if successful, otherwise an error code.
     */
    virtual Void OnInputTextChanged() noexcept;

private:
    /**
     * @brief ImGui callback function for input text changes.
     * @param _data_ptr Pointer to the ImGui input text callback data.
     */
    static Int32 InputTextCallbackFunc(ImGuiInputTextCallbackData* _data_ptr) noexcept;

private:
    /** @brief Input text widget name. */
    ZString name_;
    /** @brief Text rendering color. */
    GuiColour text_colour_;
    /** @brief Background rendering color. */
    GuiColour background_colour_;
    /** @brief Callback function invoked when text changes. */
    TFunction<Void(ZStringView)> input_text_changed_func_;
    /** @brief Internal text buffer. */
    ZMemory input_text_buffer_;
    /** @brief Current input text length. */
    Int32 input_text_length_;
    /** @brief Input text flag configuration. */
    Int32 flag_;
    /** @brief Multi-line mode state. */
    Bool is_multiline_;
    /** @brief Automatic buffer extension state. */
    Bool is_auto_extend_buffer_;
};

} // gui
} // zengine