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

#include "z_widget_object.h"

namespace zengine {
namespace error_code {

enum ZSeparatorErrorCodeEnum : ReturnType::ValueType {
    /** @brief Separator linking error occurred. */
    kZSeparatorErrorCode_LinkError = kErrorCodeBase_ZSeparator,
    /** @brief System or third-party library error. */
    kZSeparatorErrorCode_SystemOrLibraryError,
    /** @brief Null pointer parameter detected. */
    kZSeparatorErrorCode_NullptrParam,
    /** @brief Parameter value is out of valid range. */
    kZSeparatorErrorCode_ParamOutOfRange,
};

} // error_code
} // zengine

namespace zengine {
namespace gui {

/**
 * @brief Separator widget class.
 *
 * This class represents a visual separator line in the GUI. It is a lightweight
 * wrapper around the underlying ImGui separator rendering and only stores visual
 * state such as color. Rendering is performed during OnTick().
 */
class GUI_DLLAPI ZSeparator : public ZWidgetObject {
public:
    /** @brief Type alias for the base class ZWidgetObject. */
    using SuperType_ = ZWidgetObject;

    /** @brief Default separator color. */
    static inline constexpr GuiColour kDefaultColour = { 1.0f, 1.0f, 1.0f, 1.0f };

public:
    /**
     * @brief Construct a separator widget.
     */
    ZSeparator() noexcept;

    /** @brief Deleted copy constructor. */
    ZSeparator(const ZSeparator&) = delete;

    /** @brief Deleted move constructor. */
    ZSeparator(ZSeparator&&) = delete;

    /**
     * @brief Destroy the separator widget.
     */
    virtual ~ZSeparator() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZSeparator& operator=(const ZSeparator&) = delete;

    /** @brief Deleted move assignment operator. */
    ZSeparator& operator=(ZSeparator&&) = delete;

    /**
     * @brief Get widget type identifier.
     * @return Widget type enum value.
     */
    NODISCARD virtual WidgetTypeEnum_ GetWidgetType() const noexcept;

    /**
     * @brief Get separator color.
     * @return Separator color.
     */
    NODISCARD FORCEINLINE GuiColour GetColour() const noexcept { return colour_; }

    /**
     * @brief Set separator color.
     * @param _colour Separator color.
     */
    FORCEINLINE Void SetColour(GuiColour _colour) noexcept { colour_ = _colour; }

    /**
     * @brief Tick function called every frame.
     * @param _delta_sec Delta time in seconds.
     * @return kOK if successful, otherwise an error code.
     */
    virtual Void OnTick(Float32 _delta_sec) noexcept;

private:
    /** @brief Separator rendering color. */
    GuiColour colour_;
};

} // gui
} // zengine
