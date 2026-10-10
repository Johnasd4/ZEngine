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

#include "../z_core/z_string_view.h"

#include "z_widget_object.h"

namespace zengine {
namespace error_code {

enum ZTextErrorCodeEnum : ReturnType::ValueType {
    /** @brief Text widget linking error occurred. */
    kZTextErrorCode_LinkError = kErrorCodeBase_ZText,
    /** @brief System or third-party library error. */
    kZTextErrorCode_SystemOrLibraryError,
    /** @brief Null pointer parameter detected. */
    kZTextErrorCode_NullptrParam,
    /** @brief Parameter value is out of valid range. */
    kZTextErrorCode_ParamOutOfRange,
};

} // error_code
} // zengine

namespace zengine {
namespace gui {

/**
 * @brief Text widget class.
 *
 * This class represents a static text label rendered using ImGui. It stores text
 * content, color, and wrapping state. Rendering is performed during OnTick(),
 * optionally using text wrapping based on the configured width.
 */
class GUI_DLLAPI ZText : public ZWidgetObject {
public:
    /** @brief Type alias for the base class ZWidgetObject. */
    using SuperType_ = ZWidgetObject;

    /** @brief Default text color. */
    static inline constexpr GuiColour kDefaultTextColour = { 1.0f, 1.0f, 1.0f, 1.0f };

public:
    /**
     * @brief Construct an empty text widget.
     */
    ZText() noexcept;

    /**
     * @brief Construct a text widget with content.
     * @param _text Text content.
     * @param _is_wrapped Enable or disable text wrapping.
     */
    ZText(
        ZStringView _text,
        Bool _is_wrapped = true
    ) noexcept;

    /** @brief Deleted copy constructor. */
    ZText(const ZText&) = delete;

    /** @brief Deleted move constructor. */
    ZText(ZText&&) = delete;

    /**
     * @brief Destroy the text widget.
     */
    virtual ~ZText() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZText& operator=(const ZText&) = delete;

    /** @brief Deleted move assignment operator. */
    ZText& operator=(ZText&&) = delete;

    /**
     * @brief Check if text wrapping is enabled.
     * @return True if wrapping is enabled.
     */
    NODISCARD FORCEINLINE Bool IsWrapped() const noexcept { return is_wrapped_; }

    /**
     * @brief Get widget type identifier.
     * @return Widget type enum value.
     */
    NODISCARD virtual WidgetTypeEnum_ GetWidgetType() const noexcept;

    /**
     * @brief Get text content.
     * @return Text content.
     */
    NODISCARD FORCEINLINE ZStringView GetText() const noexcept { return text_; }

    /**
     * @brief Get text color.
     * @return Text color.
     */
    NODISCARD FORCEINLINE GuiColour GetTextColour() const noexcept { return text_colour_; }

    /**
     * @brief Set text content.
     * @param _text New text content.
     */
    FORCEINLINE Void SetText(ZStringView _text) noexcept { text_ = _text; }

    /**
     * @brief Set text color.
     * @param _colour Text color.
     */
    FORCEINLINE Void SetTextColour(GuiColour _colour) noexcept { text_colour_ = _colour; }

    /**
     * @brief Enable or disable text wrapping.
     * @param _if_wrap True to enable wrapping.
     */
    FORCEINLINE Void SetIfWrapped(Bool _if_wrap) noexcept { is_wrapped_ = _if_wrap; }

    /**
     * @brief Set wrapping width.
     * @param _width Width used for text wrapping.
     */
    FORCEINLINE Void SetWidth(Float32 _width) noexcept { size_.width_ = _width; }

    /**
     * @brief Tick function called every frame.
     * @param _delta_sec Delta time in seconds.
     * @return kOK if successful, otherwise an error code.
     */
    virtual Void OnTick(Float32 _delta_sec) noexcept;

private:
    /** @brief Text content. */
    ZString text_;
    /** @brief Text rendering color. */
    GuiColour text_colour_;
    /** @brief Text wrapping state. */
    Bool is_wrapped_;
};

} // gui
} // zengine
