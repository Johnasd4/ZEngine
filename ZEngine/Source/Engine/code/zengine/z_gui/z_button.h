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
 * @brief Error codes specific to ZButton operations.
 */
enum ZButtonErrorCodeEnum : ReturnType::ValueType {
    /** @brief Base error code for ZButton. */
    kZButtonErrorCode_LinkError = kErrorCodeBase_ZButton,
    /** @brief Error occurred in the system or external library. */
    kZButtonErrorCode_SystemOrLibraryError,
    /** @brief A nullptr parameter was provided where it is not allowed. */
    kZButtonErrorCode_NullptrParam,
    /** @brief A parameter value is out of the valid range. */
    kZButtonErrorCode_ParamOutOfRange,
};
} // error_code
} // zengine

namespace zengine {
namespace gui {

/**
 * @brief A standard GUI button class.
 *
 * This class encapsulates an ImGui button widget within the ZEngine object framework.
 * It manages the button's text label, styling colours (normal, hovered, active), and execution logic.
 * The button detects interaction during the OnTick update loop and executes a bound callback function
 * when a click event occurs.
 */
class GUI_DLLAPI ZButton : public ZWidgetObject {
public:
    /** @brief Alias for the base widget object type. */
    using SuperType_ = ZWidgetObject;

    /** @brief Default text colour for the button. */
    static inline constexpr GuiColour kDefaultTextColour = { 1.0f, 1.0f, 1.0f, 1.0f };
    /** @brief Default background colour for the button in its normal state. */
    static inline constexpr GuiColour kDefaultButtonColour = { 0.3f, 0.3f, 0.3f, 1.0f };
    /** @brief Default background colour when the mouse hovers over the button. */
    static inline constexpr GuiColour kDefaultHoveredColour = { 0.5f, 0.5f, 0.5f, 1.0f };
    /** @brief Default background colour when the button is actively pressed. */
    static inline constexpr GuiColour kDefaultActiveColour = { 0.4f, 0.4f, 0.4f, 1.0f };

public:
    /**
     * @brief Default constructor. Initializes a button with default settings.
     */
    ZButton() noexcept;

    /** @brief Deleted copy constructor. */
    ZButton(const ZButton&) = delete;

    /** @brief Deleted move constructor. */
    ZButton(ZButton&&) = delete;

    /**
     * @brief Constructor that initializes the button with a specific name/label.
     * @param _name The text label to display on the button.
     */
    ZButton(ZStringView _name) noexcept;

    /** @brief Deleted copy assignment operator. */
    ZButton& operator=(const ZButton&) = delete;

    /** @brief Deleted move assignment operator. */
    ZButton& operator=(ZButton&&) = delete;

    /**
     * @brief Virtual destructor.
     */
    virtual ~ZButton() noexcept = default;

    /**
     * @brief Gets the widget type identifier.
     * @return The specific WidgetTypeEnum_ for buttons.
     */
    NODISCARD virtual WidgetTypeEnum_ GetWidgetType() const noexcept;

    /**
     * @brief Gets the current name/label of the button.
     * @return The name string view.
     */
    NODISCARD FORCEINLINE ZStringView GetName() const noexcept { return name_; }

    /**
     * @brief Gets the current text colour.
     * @return The text colour (RGBA).
     */
    NODISCARD FORCEINLINE GuiColour GetTextColour() const noexcept { return text_colour_; }

    /**
     * @brief Gets the current normal background colour.
     * @return The button colour (RGBA).
     */
    NODISCARD FORCEINLINE GuiColour GetButtonColour() const noexcept { return button_colour_; }

    /**
     * @brief Gets the current hovered background colour.
     * @return The hovered colour (RGBA).
     */
    NODISCARD FORCEINLINE GuiColour GetHoveredColour() const noexcept { return hovered_colour_; }

    /**
     * @brief Gets the current active background colour.
     * @return The active colour (RGBA).
     */
    NODISCARD FORCEINLINE GuiColour GetActiveColour() const noexcept { return active_colour_; }

    /**
     * @brief Sets the name/label of the button.
     * @param _name The new label text.
     */
    FORCEINLINE Void SetName(ZStringView _name) noexcept { name_ = _name; }

    /**
     * @brief Sets the text colour of the button label.
     * @param _colour The new text colour (RGBA).
     */
    FORCEINLINE Void SetTextColour(GuiColour _colour) noexcept { text_colour_ = _colour; }

    /**
     * @brief Sets the background colour of the button in its normal state.
     * @param _colour The new button colour (RGBA).
     */
    FORCEINLINE Void SetButtonColour(GuiColour _colour) noexcept { button_colour_ = _colour; }

    /**
     * @brief Sets the background colour of the button when hovered.
     * @param _colour The new hovered colour (RGBA).
     */
    FORCEINLINE Void SetHoveredColour(GuiColour _colour) noexcept { hovered_colour_ = _colour; }

    /**
     * @brief Sets the background colour of the button when active (pressed).
     * @param _colour The new active colour (RGBA).
     */
    FORCEINLINE Void SetActiveColour(GuiColour _colour) noexcept { active_colour_ = _colour; }

    /**
     * @brief Binds a function to be executed when the button is clicked.
     * @param _click_func The function to call on click events.
     */
    FORCEINLINE Void BindClickFunction(const TFunction<Void()>& _click_func) noexcept {
        click_func_ = _click_func;
    }

    /**
     * @brief Updates and renders the button. Called every frame.
     * @param _delta_sec The time elapsed since the last frame in seconds.
     */
    virtual Void OnTick(Float32 _delta_sec) noexcept;

    /**
     * @brief Callback handler triggered internally when the button detects a click.
     */
    virtual Void OnButtonClicked() noexcept;

private:
    /** @brief The label text displayed on the button. */
    ZString name_;
    /** @brief The colour of the label text. */
    GuiColour text_colour_;
    /** @brief The background colour in the normal state. */
    GuiColour button_colour_;
    /** @brief The background colour when the mouse hovers over the button. */
    GuiColour hovered_colour_;
    /** @brief The background colour when the button is pressed. */
    GuiColour active_colour_;
    /** @brief The user-defined callback function to execute on click. */
    TFunction<Void()> click_func_;
};

} // gui
} // zengine