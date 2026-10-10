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

#include "z_widget_object.h"

namespace zengine {
namespace error_code {

enum ZFrameErrorCodeEnum : ReturnType::ValueType {
    /** @brief Frame linking error occurred. */
    kZFrameErrorCode_LinkError = kErrorCodeBase_ZFrame,
    /** @brief System or third-party library error. */
    kZFrameErrorCode_SystemOrLibraryError,
    /** @brief Null pointer parameter detected. */
    kZFrameErrorCode_NullptrParam,
    /** @brief Parameter value is out of valid range. */
    kZFrameErrorCode_ParamOutOfRange,
    /** @brief Frame has already been created. */
    kZFrameErrorCode_FrameAreadyCreated,
    /** @brief Frame does not exist. */
    kZFrameErrorCode_FrameNotExist
};

} // error_code
} // zengine

namespace zengine {
namespace gui {

/**
 * @brief GUI frame widget class.
 *
 * This class is a high-level wrapper of an ImGui window. It manages window state, flags,
 * background color, scroll state, and owns a list of child widgets. Rendering is driven
 * by OnTick(), which synchronizes ImGui window state and then dispatches rendering to
 * contained widgets.
 */
class GUI_DLLAPI ZFrame : public ZWidgetObject {
public:
    /** @brief Alias for the base class. */
    using SuperType_ = ZWidgetObject;

    /**
     * @brief Frame behavior and appearance flags.
     */
    enum FlagEnum_ : Int32 {
        /** @brief No special flags enabled. */
        kFrameFlag_None = 0,
        /** @brief Disable title bar rendering. */
        kFrameFlag_NoTitleBar = 1 << 0,
        /** @brief Disable user resizing using the resize grip. */
        kFrameFlag_NoResize = 1 << 1,
        /** @brief Disable user moving the frame. */
        kFrameFlag_NoMove = 1 << 2,
        /** @brief Disable scrollbars rendering. */
        kFrameFlag_NoScrollbar = 1 << 3,
        /** @brief Disable mouse wheel vertical scrolling. */
        kFrameFlag_NoScrollWithMouse = 1 << 4,
        /** @brief Disable collapsing behavior. */
        kFrameFlag_NoCollapse = 1 << 5,
        /** @brief Always resize frame to fit its content. */
        kFrameFlag_AlwaysAutoResize = 1 << 6,
        /** @brief Disable background and border rendering. */
        kFrameFlag_NoBackground = 1 << 7,
        /** @brief Disable saving/loading frame state from ini file. */
        kFrameFlag_NoSavedSettings = 1 << 8,
        /** @brief Disable mouse input handling. */
        kFrameFlag_NoMouseInputs = 1 << 9,
        /** @brief Enable menu bar at the top of the frame. */
        kFrameFlag_MenuBar = 1 << 10,
        /** @brief Allow horizontal scrollbar to appear. */
        kFrameFlag_HorizontalScrollbar = 1 << 11,
        /** @brief Disable focusing when frame appears. */
        kFrameFlag_NoFocusOnAppearing = 1 << 12,
        /** @brief Disable bringing frame to front on focus. */
        kFrameFlag_NoBringToFrontOnFocus = 1 << 13,
        /** @brief Always show vertical scrollbar. */
        kFrameFlag_AlwaysVerticalScrollbar = 1 << 14,
        /** @brief Always show horizontal scrollbar. */
        kFrameFlag_AlwaysHorizontalScrollbar = 1 << 15,
        /** @brief Disable keyboard/gamepad navigation input. */
        kFrameFlag_NoNavInputs = 1 << 16,
        /** @brief Disable navigation focus. */
        kFrameFlag_NoNavFocus = 1 << 17,
        /** @brief Mark frame as having unsaved changes. */
        kFrameFlag_UnsavedDocument = 1 << 18,
        /** @brief Disable all navigation features. */
        kFrameFlag_NoNav = kFrameFlag_NoNavInputs | kFrameFlag_NoNavFocus,
        /** @brief Disable all decoration elements. */
        kFrameFlag_NoDecoration = kFrameFlag_NoTitleBar | kFrameFlag_NoResize |
                                  kFrameFlag_NoScrollbar | kFrameFlag_NoCollapse,
        /** @brief Disable all input handling. */
        kFrameFlag_NoInputs = kFrameFlag_NoMouseInputs | kFrameFlag_NoNavInputs |
                              kFrameFlag_NoNavFocus,
    };

    /** @brief Default frame background color. */
    static inline constexpr GuiColour kDefaultBackgroundColour = { 0.2f, 0.2f, 0.2f, 1.0f };
    /** @brief Default frame flag configuration. */
    static inline constexpr Int32 kDefaultFrameFlag = kFrameFlag_None;

public:
    /**
     * @brief Construct an empty frame.
     */
    ZFrame() noexcept;

    /** @brief Deleted copy constructor. */
    ZFrame(const ZFrame&) = delete;

    /** @brief Deleted move constructor. */
    ZFrame(ZFrame&&) = delete;

    /**
     * @brief Construct a frame with a name.
     * @param _name Frame display name.
     */
    ZFrame(ZStringView _name) noexcept;

    /**
     * @brief Construct a frame with name, size, and position.
     * @param _name Frame display name.
     * @param _size Initial frame size.
     * @param _pos Initial frame position.
     */
    ZFrame(
        ZStringView _name,
        GuiSize _size,
        GuiPos _pos = kBasePos
    ) noexcept;

    /**
     * @brief Destroy the frame.
     */
    virtual ~ZFrame() noexcept;

    /** @brief Deleted copy assignment operator. */
    ZFrame& operator=(const ZFrame&) = delete;

    /** @brief Deleted move assignment operator. */
    ZFrame& operator=(ZFrame&&) = delete;

    /**
     * @brief Get widget type identifier.
     * @return Widget type enum value.
     */
    NODISCARD virtual WidgetTypeEnum_ GetWidgetType() const noexcept;

    /**
     * @brief Get frame name.
     * @return Frame name string view.
     */
    NODISCARD FORCEINLINE ZStringView GetName() const noexcept { return name_; }

    /**
     * @brief Get background color.
     * @return Frame background color.
     */
    NODISCARD FORCEINLINE GuiColour GetBackgroundColour() const noexcept { return background_colour_; }

    /**
     * @brief Get horizontal scroll position.
     * @return Scroll X value.
     */
    NODISCARD FORCEINLINE Float32 GetScrollX() const noexcept { return scroll_pos_.x_; }

    /**
     * @brief Get vertical scroll position.
     * @return Scroll Y value.
     */
    NODISCARD FORCEINLINE Float32 GetScrollY() const noexcept { return scroll_pos_.y_; }

    /**
     * @brief Get current frame flags.
     * @return Combined frame flags.
     */
    NODISCARD FORCEINLINE Int32 GetFlag() const noexcept { return flag_; }

    /**
     * @brief Set the frame display name.
     * @param _name New frame name.
     */
    FORCEINLINE Void SetName(ZStringView _name) noexcept { name_ = _name; }

    /**
     * @brief Set frame background color.
     * @param _colour Background color.
     */
    FORCEINLINE Void SetBackgroundColour(GuiColour _colour) noexcept { background_colour_ = _colour; }

    /**
     * @brief Set horizontal scroll position.
     * @param _scroll_x Scroll offset on X axis.
     */
    FORCEINLINE Void SetScrollX(Float32 _scroll_x) noexcept {
        scroll_pos_.x_ = _scroll_x;
        scroll_x_state_ = PosSizeStateEnum_::kSet;
    }

    /**
     * @brief Set vertical scroll position.
     * @param _scroll_y Scroll offset on Y axis.
     */
    FORCEINLINE Void SetScrollY(Float32 _scroll_y) noexcept {
        scroll_pos_.y_ = _scroll_y;
        scroll_y_state_ = PosSizeStateEnum_::kSet;
    }

    /**
     * @brief Set all frame flags at once.
     * @param _flag Combined frame flags.
     */
    FORCEINLINE Void SetFlag(Int32 _flag) noexcept { flag_ = _flag; }

    /**
     * @brief Enable or disable a specific frame flag.
     * @param _flag Flag to modify.
     * @param _if_active True to enable, false to disable.
     */
    FORCEINLINE Void SetFlag(FlagEnum_ _flag, Bool _if_active) noexcept {
        _if_active ? flag_ |= _flag : flag_ &= ~_flag;
    }

    /**
     * @brief Add a widget to the frame.
     * @param _widget_ptr Pointer to widget object.
     * @return kOK if successful, otherwise an error code.
     */
    Void AddWidget(ZWidgetObject* _widget_ptr) noexcept;

    /**
     * @brief Remove a widget from the frame.
     * @param _widget_ptr Pointer to widget object.
     * @return kOK if successful, otherwise an error code.
     */
    Void RemoveWidget(ZWidgetObject* _widget_ptr) noexcept;

    /**
     * @brief Remove all widgets from the frame.
     * @return kOK if successful, otherwise an error code.
     */
    Void RemoveAllWidget() noexcept;

    /**
     * @brief Frame tick function called every frame.
     * @param _delta_sec Delta time in seconds.
     * @return kOK if successful, otherwise an error code.
     */
    virtual Void OnTick(Float32 _delta_sec) noexcept;

    /**
     * @brief Widget tick function called after frame setup.
     * @param _delta_sec Delta time in seconds.
     * @return kOK if successful, otherwise an error code.
     */
    virtual Void OnWidgetTick(Float32 _delta_sec) noexcept;

private:
    /** @brief Frame display name. */
    ZString name_;
    /** @brief Array of child widget pointers. */
    TArray<ZWidgetObject*, false> widget_ptr_array_;
    /** @brief Frame background color. */
    GuiColour background_colour_;
    /** @brief Horizontal scroll state. */
    PosSizeStateEnum_ scroll_x_state_;
    /** @brief Vertical scroll state. */
    PosSizeStateEnum_ scroll_y_state_;
    /** @brief Current scroll position. */
    GuiPos scroll_pos_;
    /** @brief Maximum scrollable position. */
    GuiPos scroll_max_pos_;
    /** @brief Frame flag configuration. */
    Int32 flag_;
};

} // gui
} // zengine
