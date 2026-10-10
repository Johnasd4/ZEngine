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
#include "../z_core/z_string_view.h"

#include "z_widget_object.h"

namespace zengine {
namespace error_code {

/**
 * @brief Enumeration for ZCheckBox error codes.
 * @details ReturnType will return kOK if successful, otherwise it will return these error codes.
 */
enum ZCheckBoxErrorCodeEnum : ReturnType::ValueType {
    /** @brief Link error occurred in ZCheckBox. */
    kZCheckBoxErrorCode_LinkError = kErrorCodeBase_ZCheckBox,
    /** @brief System or library error occurred in ZCheckBox. */
    kZCheckBoxErrorCode_SystemOrLibraryError,
    /** @brief Null pointer parameter passed to ZCheckBox. */
    kZCheckBoxErrorCode_NullptrParam,
    /** @brief Parameter is out of valid range in ZCheckBox. */
    kZCheckBoxErrorCode_ParamOutOfRange,
};

} // error_code
} // zengine

namespace zengine {
namespace gui {

/**
 * @brief Check Box class.
 * @details Implementation Principle: This class inherits from ZWidgetObject to provide GUI check box functionality.
 * It manages a boolean state (is_checked_) to track its status. It uses predefined color properties to render
 * differently depending on its current state (normal, hovered, active). When a user interacts with it, it toggles
 * the boolean state and triggers the bound callback function (click_func_) to notify external systems.
 */
class GUI_DLLAPI ZCheckBox : public ZWidgetObject {
public:
    /** @brief Alias type for the base class ZWidgetObject. */
    using SuperType_ = ZWidgetObject;

    /** @brief Default text color for the check box. */
    static inline constexpr GuiColour kDefaultTextColour = { 1.0f, 1.0f, 1.0f, 1.0f };
    /** @brief Default color for the check mark inside the box. */
    static inline constexpr GuiColour kDefaultCheckMarkColour = { 1.0f, 1.0f, 1.0f, 1.0f };
    /** @brief Default background color of the check box. */
    static inline constexpr GuiColour kDefaultCheckBoxColour = { 0.3f, 0.3f, 0.3f, 1.0f };
    /** @brief Default color when the check box is hovered over by the cursor. */
    static inline constexpr GuiColour kDefaultHoveredColour = { 0.5f, 0.5f, 0.5f, 1.0f };
    /** @brief Default color when the check box is actively clicked or pressed. */
    static inline constexpr GuiColour kDefaultActiveColour = { 0.4f, 0.4f, 0.4f, 1.0f };

public:
    /**
     * @brief Default constructor for ZCheckBox.
     */
    ZCheckBox() noexcept;

    /** @brief Deleted copy constructor. */
    ZCheckBox(const ZCheckBox&) = delete;

    /** @brief Deleted move constructor. */
    ZCheckBox(ZCheckBox&&) = delete;

    /**
     * @brief Constructor that initializes the check box with a specific name.
     * @param _name The string view representing the name of the check box.
     */
    ZCheckBox(ZStringView _name) noexcept;

    /** @brief Deleted copy assignment operator. */
    ZCheckBox& operator=(const ZCheckBox&) = delete;

    /** @brief Deleted move assignment operator. */
    ZCheckBox& operator=(ZCheckBox&&) = delete;

    /**
     * @brief Destructor for ZCheckBox.
     */
    virtual ~ZCheckBox() noexcept = default;

    /**
     * @brief Checks if the check box is currently in a checked state.
     * @return True if the check box is checked, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsChecked() noexcept { return is_checked_; }

    /**
     * @brief Retrieves the specific widget type enumeration value for this object.
     * @return The WidgetTypeEnum_ value corresponding to a check box.
     */
    NODISCARD virtual WidgetTypeEnum_ GetWidgetType() const noexcept;

    /**
     * @brief Gets the name of the check box.
     * @return A ZStringView representing the name.
     */
    NODISCARD FORCEINLINE ZStringView GetName() const noexcept { return name_; }

    /**
     * @brief Gets the current text color of the check box.
     * @return The GuiColour used for rendering the text.
     */
    NODISCARD FORCEINLINE GuiColour GetTextColour() const noexcept { return text_colour_; }

    /**
     * @brief Gets the current color of the check mark.
     * @return The GuiColour used for rendering the check mark.
     */
    NODISCARD FORCEINLINE GuiColour GetCheckMarkColour() const noexcept { return check_mark_colour_; }

    /**
     * @brief Gets the current background color of the check box.
     * @return The GuiColour used for rendering the check box background.
     */
    NODISCARD FORCEINLINE GuiColour GetCheckBoxColour() const noexcept { return check_box_colour_; }

    /**
     * @brief Gets the color used when the check box is hovered.
     * @return The GuiColour used for the hovered state.
     */
    NODISCARD FORCEINLINE GuiColour GetHoveredColour() const noexcept { return hovered_colour_; }

    /**
     * @brief Gets the color used when the check box is active.
     * @return The GuiColour used for the active state.
     */
    NODISCARD FORCEINLINE GuiColour GetActiveColour() const noexcept { return active_colour_; }

    /**
     * @brief Sets a new name for the check box.
     * @param _name The new name string view to be assigned.
     * @return Void.
     */
    FORCEINLINE Void SetName(ZStringView _name) noexcept { name_ = _name; }

    /**
     * @brief Sets a new text color for the check box.
     * @param _colour The GuiColour to be set for the text.
     * @return Void.
     */
    FORCEINLINE Void SetTextColour(GuiColour _colour) noexcept { text_colour_ = _colour; }

    /**
     * @brief Sets a new color for the check mark.
     * @param _colour The GuiColour to be set for the check mark.
     * @return Void.
     */
    FORCEINLINE Void SetCheckMarkColour(GuiColour _colour) noexcept { check_mark_colour_ = _colour; }

    /**
     * @brief Sets a new background color for the check box.
     * @param _colour The GuiColour to be set for the check box background.
     * @return Void.
     */
    FORCEINLINE Void SetCheckBoxColour(GuiColour _colour) noexcept { check_box_colour_ = _colour; }

    /**
     * @brief Sets a new hover color for the check box.
     * @param _colour The GuiColour to be set for the hovered state.
     * @return Void.
     */
    FORCEINLINE Void SetHoveredColour(GuiColour _colour) noexcept { hovered_colour_ = _colour; }

    /**
     * @brief Sets a new active color for the check box.
     * @param _colour The GuiColour to be set for the active state.
     * @return Void.
     */
    FORCEINLINE Void SetActiveColour(GuiColour _colour) noexcept { active_colour_ = _colour; }

    /**
     * @brief Sets the checked state of the check box.
     * @param _if_checked Boolean flag indicating whether to check (true) or uncheck (false) the box.
     * @return Void.
     */
    FORCEINLINE Void SetChecked(Bool _if_checked) noexcept { is_checked_ = _if_checked; }

    /**
     * @brief Binds a callback function to the click event of the check box.
     * @param _click_func The function to be invoked when clicked, taking the boolean state as an argument.
     * @return Void.
     */
    FORCEINLINE Void BindClickFunction(const TFunction<Void(Bool)>& _click_func) noexcept {
        click_func_ = _click_func;
    }

    /**
     * @brief Tick update function called every frame to update the logic of the check box.
     * @param _delta_sec The time elapsed since the last tick in seconds.
     * @return Void.
     */
    virtual Void OnTick(Float32 _delta_sec) noexcept;

    /**
     * @brief Event handler invoked when the check box receives a click interaction.
     * @param _if_checked The new state of the check box after the click event.
     * @return Void.
     */
    virtual Void OnClicked(Bool _if_checked) noexcept;

private:
    /** @brief The internal stored name of the check box. */
    ZString name_;
    /** @brief The internal stored text color. */
    GuiColour text_colour_;
    /** @brief The internal stored check mark color. */
    GuiColour check_mark_colour_;
    /** @brief The internal stored background color of the check box. */
    GuiColour check_box_colour_;
    /** @brief The internal stored hover color. */
    GuiColour hovered_colour_;
    /** @brief The internal stored active color. */
    GuiColour active_colour_;
    /** @brief The callback function bound to the click event. */
    TFunction<Void(Bool)> click_func_;
    /** @brief The internal state tracking if the box is checked. */
    Bool is_checked_;
};

} // gui
} // zengine