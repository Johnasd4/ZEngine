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

#include "../z_core/z_string.h"

#include "z_gui_object.h"

namespace zengine {
namespace gui {

/**
 * @brief The base class of the gui classes.
 *
 * Inheriting from this class allows the instance to apply memory from the memorypool,
 * instead of applying memory directly from the system.
 * The class manages basic widget properties such as priority, layout positioning (same-line),
 * and spacing.
 */
class GUI_DLLAPI ZWidgetObject : public ZGuiObject {
public:
    /** @brief Type alias for the base class. */
    using SuperType_ = ZGuiObject;

    /** @brief The default priority value for a widget. */
    static inline constexpr Int32 kDefaultPriority = 0;
    /** @brief The default distance between widgets when placed on the same line. */
    static inline constexpr Float32 kDefaultSameLineWidgetDistance = 0;
    /** @brief The default spacing value for widgets on the same line. */
    static inline constexpr Float32 kDefaultSameLineWidgetSpacing = -1;

    /** @brief Enumeration representing the specific type of the widget. */
    enum class WidgetTypeEnum_ : Int32 {
        /** @brief Represents a frame container widget. */
        kFrame,
        /** @brief Represents a static text display widget. */
        kText,
        /** @brief Represents an input text field widget. */
        kInputText,
        /** @brief Represents a clickable button widget. */
        kButton,
        /** @brief Represents a visual separator widget. */
        kSeparator,
        /** @brief Represents a boolean checkbox widget. */
        kCheckBox,
    };

public:
    /**
     * @brief Checks if the widget is configured to be on the same line.
     * @return True if on the same line, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsSameLine() const noexcept { return same_line_; }

    /**
     * @brief Gets the distance offset used when the widget is on the same line.
     * @return The distance value.
     */
    NODISCARD FORCEINLINE Float32 GetSameLineWidgetDistance() const noexcept { return same_line_widget_distance_; }

    /**
     * @brief Gets the spacing value used for same-line layout.
     * @return The spacing value.
     */
    NODISCARD FORCEINLINE Float32 GetSameLineWidgetSpacing() const noexcept { return same_line_widget_spacing_; }

    /**
     * @brief Gets the specific type of the widget.
     * @return The widget type enumeration.
     */
    NODISCARD virtual WidgetTypeEnum_ GetWidgetType() const noexcept = 0;

    /**
     * @brief Sets whether the widget should be positioned on the same line as the previous widget.
     * @param _same_line True to place on the same line, false otherwise.
     */
    FORCEINLINE Void SetSameLine(Bool _same_line) noexcept { same_line_ = _same_line; }

    /**
     * @brief Sets the distance offset for the widget when placed on the same line.
     * @param _distance The distance value.
     */
    FORCEINLINE Void SetSameLineWidgetDistance(Float32 _distance) noexcept { same_line_widget_distance_ = _distance; }

    /**
     * @brief Sets the spacing used when multiple widgets are on the same line.
     * @param _spacing The spacing value.
     */
    FORCEINLINE Void SetSameLineWidgetSpacing(Float32 _spacing) noexcept { same_line_widget_spacing_ = _spacing; }

    /**
     * @brief Updates the widget logic.
     * @param _delta_sec The time elapsed since the last frame in seconds.
     */
    virtual Void OnTick(Float32 _delta_sec) noexcept;

protected:
    /**
     * @brief Default constructor.
     */
    ZWidgetObject() noexcept;

    /** @brief Deleted copy constructor. */
    ZWidgetObject(const ZWidgetObject&) = delete;

    /** @brief Deleted move constructor. */
    ZWidgetObject(ZWidgetObject&&) = delete;

    /**
     * @brief Parameterized constructor to initialize size and position.
     * @param _size The size of the widget.
     * @param _pos The position of the widget.
     */
    ZWidgetObject(
        GuiSize _size,
        GuiPos _pos = kBasePos
    ) noexcept;

    /**
     * @brief Virtual destructor.
     */
    virtual ~ZWidgetObject() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZWidgetObject& operator=(const ZWidgetObject&) = delete;

    /** @brief Deleted move assignment operator. */
    ZWidgetObject& operator=(ZWidgetObject&&) = delete;

private:
    /** @brief The specific distance adjustment when placed on the same line. */
    Float32 same_line_widget_distance_;
    /** @brief The spacing value for same-line positioning calculation. */
    Float32 same_line_widget_spacing_;
    /** @brief Flag indicating if this widget follows the previous one on the same line. */
    Bool same_line_;
};

} // gui
} // zengine