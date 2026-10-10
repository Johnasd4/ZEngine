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

#include "../z_core/t_array.h"
#include "../z_core/z_string_view.h"

#include "z_gui_object.h"

namespace zengine {
namespace error_code {

/** 
 * @brief Error codes for window-related operations.
 * @return ReturnType returns kOK on success, otherwise returns a specific window error code.
 */
enum ZWindowErrorCodeEnum : ReturnType::ValueType {
    /** @brief Link error occurred when using the window module. */
    kZWindowErrorCode_LinkError = kErrorCodeBase_ZWindow,
    /** @brief System or third-party library error occurred. */
    kZWindowErrorCode_SystemOrLibraryError,
    /** @brief A nullptr parameter was passed to a function. */
    kZWindowErrorCode_NullptrParam,
    /** @brief A parameter value is out of the valid range. */
    kZWindowErrorCode_ParamOutOfRange,
    /** @brief The window has already been created. */
    kZWindowErrorCode_WindowAreadyCreated,
    /** @brief The specified window does not exist. */
    kZWindowErrorCode_WindowNotExist
};

} // error_code
} // zengine

namespace zengine {
namespace gui {

class ZWidgetObject;

} // gui
} // zengine

namespace zengine {
namespace gui {

/**
 * @brief GUI window object.
 *
 * The window acts as a container for GUI widgets and manages layout, scrolling,
 * rendering order, and interaction flags. Internally, it synchronizes window
 * states with the underlying GUI backend every frame and propagates ticking
 * to its child widgets in a deterministic order.
 */
class GUI_DLLAPI ZWindow : public ZGuiObject {
public:
    /** @brief Type alias for the base class ZGuiObject. */
    using SuperType_ = ZGuiObject;

    /**
     * @brief Window behavior and appearance flags.
     */
    enum FlagEnum_ : Int32 {
        /** @brief No special window behavior. */
        kWindowFlag_None = 0,
        /** @brief Disable the title bar. */
        kWindowFlag_NoTitleBar = 1 << 0,
        /** @brief Disable user resizing via the resize grip. */
        kWindowFlag_NoResize = 1 << 1,
        /** @brief Disable user moving the window. */
        kWindowFlag_NoMove = 1 << 2,
        /** @brief Disable scrollbars. */
        kWindowFlag_NoScrollbar = 1 << 3,
        /** @brief Disable mouse wheel vertical scrolling. */
        kWindowFlag_NoScrollWithMouse = 1 << 4,
        /** @brief Disable window collapsing by double click. */
        kWindowFlag_NoCollapse = 1 << 5,
        /** @brief Automatically resize window to fit its contents every frame. */
        kWindowFlag_AlwaysAutoResize = 1 << 6,
        /** @brief Disable drawing window background and border. */
        kWindowFlag_NoBackground = 1 << 7,
        /** @brief Prevent loading and saving window settings. */
        kWindowFlag_NoSavedSettings = 1 << 8,
        /** @brief Disable mouse input handling. */
        kWindowFlag_NoMouseInputs = 1 << 9,
        /** @brief Enable menu bar support. */
        kWindowFlag_MenuBar = 1 << 10,
        /** @brief Allow horizontal scrollbar. */
        kWindowFlag_HorizontalScrollbar = 1 << 11,
        /** @brief Disable taking focus when appearing. */
        kWindowFlag_NoFocusOnAppearing = 1 << 12,
        /** @brief Disable bringing window to front when focused. */
        kWindowFlag_NoBringToFrontOnFocus = 1 << 13,
        /** @brief Always show vertical scrollbar. */
        kWindowFlag_AlwaysVerticalScrollbar = 1 << 14,
        /** @brief Always show horizontal scrollbar. */
        kWindowFlag_AlwaysHorizontalScrollbar = 1 << 15,
        /** @brief Disable keyboard and gamepad navigation. */
        kWindowFlag_NoNavInputs = 1 << 16,
        /** @brief Disable navigation focus. */
        kWindowFlag_NoNavFocus = 1 << 17,
        /** @brief Mark window content as unsaved. */
        kWindowFlag_UnsavedDocument = 1 << 18,
        /** @brief Disable all navigation inputs and focus. */
        kWindowFlag_NoNav = kWindowFlag_NoNavInputs | kWindowFlag_NoNavFocus,
        /** @brief Disable title bar, resizing, scrollbar, and collapsing. */
        kWindowFlag_NoDecoration = kWindowFlag_NoTitleBar | kWindowFlag_NoResize |
                                   kWindowFlag_NoScrollbar | kWindowFlag_NoCollapse,
        /** @brief Disable all user inputs. */
        kWindowFlag_NoInputs = kWindowFlag_NoMouseInputs | kWindowFlag_NoNavInputs |
                               kWindowFlag_NoNavFocus
    };

    /** @brief Default window flags. */
    static inline constexpr Int32 kDefaultWindowFlag = kWindowFlag_None;

    /** @brief Default background color of the window. */
    static inline constexpr GuiColour kDefaultBackgroundColour = { 0.2f, 0.2f, 0.2f, 1.0f };

public:
    /**
     * @brief Construct an empty window.
     */
    ZWindow() noexcept;

    /** @brief Deleted copy constructor. */
    ZWindow(const ZWindow&) = delete;

    /** @brief Deleted move constructor. */
    ZWindow(ZWindow&&) = delete;

    /**
     * @brief Construct a window with a name.
     * @param _name Window name.
     */
    ZWindow(ZStringView _name) noexcept;

    /**
     * @brief Construct a window with name, size, and position.
     * @param _name Window name.
     * @param _size Initial window size.
     * @param _pos Initial window position.
     */
    ZWindow(ZStringView _name, GuiSize _size, GuiPos _pos = kBasePos) noexcept;

    /**
     * @brief Destroy the window and release internal resources.
     */
    virtual ~ZWindow() noexcept;

    /** @brief Deleted copy assignment operator. */
    ZWindow& operator=(const ZWindow&) = delete;

    /** @brief Deleted move assignment operator. */
    ZWindow& operator=(ZWindow&&) = delete;

    /**
     * @brief Get the window name.
     * @return Window name.
     */
    NODISCARD FORCEINLINE ZStringView GetName() const noexcept { return name_; }

    /**
     * @brief Get the window background color.
     * @return Background color.
     */
    NODISCARD FORCEINLINE GuiColour GetBackgroundColour() const noexcept { return background_colour_; }

    /**
     * @brief Get horizontal scroll position.
     * @return Horizontal scroll value.
     */
    NODISCARD FORCEINLINE Float32 GetScrollX() const noexcept { return scroll_pos_.x_; }

    /**
     * @brief Get vertical scroll position.
     * @return Vertical scroll value.
     */
    NODISCARD FORCEINLINE Float32 GetScrollY() const noexcept { return scroll_pos_.y_; }

    /**
     * @brief Get current window flags.
     * @return Flag bitmask.
     */
    NODISCARD FORCEINLINE Int32 GetFlag() const noexcept { return flag_; }

    /**
     * @brief Set the window name.
     * @param _name New window name.
     */
    FORCEINLINE Void SetName(ZStringView _name) noexcept { name_ = _name; }

    /**
     * @brief Set the window background color.
     * @param _colour Background color.
     */
    FORCEINLINE Void SetBackgroundColour(GuiColour _colour) noexcept { background_colour_ = _colour; }

    /**
     * @brief Set horizontal scroll position.
     * @param _scroll_x Horizontal scroll value.
     */
    FORCEINLINE Void SetScrollX(Float32 _scroll_x) noexcept {
        scroll_pos_.x_ = _scroll_x;
        scroll_x_state_ = PosSizeStateEnum_::kSet;
    }

    /**
     * @brief Set vertical scroll position.
     * @param _scroll_y Vertical scroll value.
     */
    FORCEINLINE Void SetScrollY(Float32 _scroll_y) noexcept {
        scroll_pos_.y_ = _scroll_y;
        scroll_y_state_ = PosSizeStateEnum_::kSet;
    }

    /**
     * @brief Set all window flags at once.
     * @param _flag Combined flag value.
     */
    FORCEINLINE Void SetFlag(Int32 _flag) noexcept { flag_ = _flag; }

    /**
     * @brief Enable or disable a specific window flag.
     * @param _flag Flag to modify.
     * @param _if_active Whether the flag should be active.
     */
    FORCEINLINE Void SetFlag(FlagEnum_ _flag, Bool _if_active) noexcept {
        _if_active ? flag_ |= _flag : flag_ &= ~_flag;
    }

    /**
     * @brief Add a widget to the window.
     * @param _widget_ptr Pointer to the widget.
     */
    Void AddWidget(ZWidgetObject* _widget_ptr) noexcept;

    /**
     * @brief Remove a widget from the window.
     * @param _widget_ptr Pointer to the widget.
     */
    Void RemoveWidget(ZWidgetObject* _widget_ptr) noexcept;

    /**
     * @brief Remove all widgets from the window.
     */
    Void RemoveAllWidget() noexcept;

    /**
     * @brief Tick the window every frame for rendering.
     * @param _delta_sec Delta time in seconds.
     */
    virtual Void OnTick(Float32 _delta_sec) noexcept;

    /**
     * @brief Tick widgets after they are added to the window.
     * @param _delta_sec Delta time in seconds.
     */
    virtual Void OnWidgetTick(Float32 _delta_sec) noexcept;

private:
    /** @brief Window name string. */
    ZString name_;
    /** @brief Array of widget pointers contained in the window. */
    TArray<ZWidgetObject*, false> widget_ptr_array_;
    /** @brief Window background color. */
    GuiColour background_colour_;
    /** @brief Horizontal scroll state. */
    PosSizeStateEnum_ scroll_x_state_;
    /** @brief Vertical scroll state. */
    PosSizeStateEnum_ scroll_y_state_;
    /** @brief Current scroll position. */
    GuiPos scroll_pos_;
    /** @brief Maximum scrollable position. */
    GuiPos scroll_max_pos_;
    /** @brief Window behavior flags. */
    Int32 flag_;
};

} // gui
} // zengine
