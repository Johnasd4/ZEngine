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

#include "../z_core/z_object.h"

namespace zengine {
namespace gui {

/**
 * @brief The base class of the gui classes.
 *
 * Inheriting from this class allows the instance to apply memory from the memory pool,
 * instead of applying memory directly from the system. This relies on the ZObject base class
 * mechanism for overridden new/delete operators optimized for ZEngine.
 */
class GUI_DLLAPI ZGuiObject : public ZObject {
public:
    /** @brief Type alias for the base class ZObject. */
    using SuperType_ = ZObject;

    /** @brief The default base size initialized to an invalid value. */
    static inline constexpr GuiSize kBaseSize = { -10000.0f, -10000.0f };
    /** @brief The default base position initialized to an invalid value. */
    static inline constexpr GuiPos kBasePos = { -10000.0f, -10000.0f };
    /** @brief The default font scale factor. */
    static inline constexpr Float32 kBaseFontScale = 1.0f;

public:
    /**
     * @brief Checks if the GUI object is enabled.
     * @return True if enabled, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsEnabled() const noexcept { return enabled_; }

    /**
     * @brief Checks if the GUI object is visible.
     * @return True if visible, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsVisible() const noexcept { return visible_; }

    /**
     * @brief Gets the current size of the GUI object.
     * @return The current size.
     */
    NODISCARD virtual GuiSize GetSize() const noexcept;

    /**
     * @brief Gets the current position of the GUI object.
     * @return The current position.
     */
    NODISCARD virtual GuiPos GetPos() const noexcept;

    /**
     * @brief Gets the absolute position of the GUI object in the window.
     * This takes into account the positions of all parent/owner objects.
     * @return The absolute position.
     */
    NODISCARD virtual GuiPos GetAbsPos() const noexcept;

    /**
     * @brief Gets the current font scale.
     * @return The current font scale factor.
     */
    NODISCARD Float32 GetFontScale() const noexcept;

    /**
     * @brief Sets the enabled state of the GUI object.
     * @param _enabled True to enable the object, false to disable it.
     */
    FORCEINLINE Void SetEnabled(Bool _enabled) noexcept { enabled_ = _enabled; }

    /**
     * @brief Sets the visibility state of the GUI object.
     * @param _visible True to make the object visible, false to hide it.
     */
    virtual Void SetVisible(Bool _visible) noexcept;

    /**
     * @brief Sets the size of the GUI object.
     * @param _size The new size to set.
     */
    virtual Void SetSize(GuiSize _size) noexcept;

    /**
     * @brief Sets the position of the GUI object.
     * @param _pos The new position to set.
     */
    virtual Void SetPos(GuiPos _pos) noexcept;

    /**
     * @brief Sets the absolute position of the GUI object in the window.
     * This overrides any parent/owner positioning.
     * @param _abs_pos The absolute position to set.
     */
    virtual Void SetAbsPos(GuiPos _pos) noexcept;

    /**
     * @brief Sets the font scale for text rendering within this object.
     * @param _scale The font scaling factor.
     */
    virtual Void SetFontScale(Float32 _scale) noexcept;

    /**
     * @brief Called every frame to update the object logic.
     * @param _delta_sec The time elapsed since the last frame in seconds.
     */
    virtual Void OnTick(Float32 _delta_sec) noexcept = 0;

    /**
     * @brief Called when this object is added to a parent/owner.
     * @param _owner_ptr Pointer to the new owner object.
     */
    virtual Void OnAdd(ZGuiObject* _owner_ptr) noexcept;

    /**
     * @brief Called when this object is removed from a parent/owner.
     * @param _owner_ptr Pointer to the previous owner object.
     */
    virtual Void OnRemove() noexcept;

protected:
    /** @brief Enumeration for position and size state tracking. */
    enum class PosSizeStateEnum_ : UInt8 {
        /** @brief Indicates that no size or position has been set or needs to get. */
        kNone = 0,
        /** @brief Indicates that the size or position has been explicitly set. */
        kSet,
        /** @brief Indicates that the size or position is at its default value, gets the value at the next tick. */
        kGet
    };

    /**
     * @brief Default constructor.
     */
    ZGuiObject() noexcept;

    /** @brief Deleted copy constructor. */
    ZGuiObject(const ZGuiObject&) = delete;

    /** @brief Deleted move constructor. */
    ZGuiObject(ZGuiObject&&) = delete;

    /**
     * @brief Parameterized constructor to initialize size and position.
     * @param _size The initial size of the GUI object.
     * @param _pos The initial position of the GUI object.
     */
    ZGuiObject(
        GuiSize _size, 
        GuiPos _pos = kBasePos
    ) noexcept;

    /**
     * @brief Virtual destructor.
     * Ensures proper cleanup of derived classes.
     */
    virtual ~ZGuiObject() noexcept = default;

    /** @brief Deleted copy assignment operator. */
    ZGuiObject& operator=(const ZGuiObject&) = delete;

    /** @brief Deleted move assignment operator. */
    ZGuiObject& operator=(ZGuiObject&&) = delete;

private:
    /**
     * @brief Helper function to handle the logic of moving an object.
     * @param _obj The object to move from.
     */
    Void MoveP(ZGuiObject&& _obj) noexcept;

protected:
    /** @brief The size of the GUI object. */
    GuiSize size_;
    /** @brief The position of the GUI object. */
    GuiPos pos_;
    /** @brief Flag indicating if the size has been explicitly set or needs to get. */
    PosSizeStateEnum_ size_state_;
    /** @brief Flag indicating if the position has been explicitly set or needs to get. */
    PosSizeStateEnum_ pos_state_;
    /** @brief Flag indicating if the object is enabled (can interact). */
    Bool enabled_;
    /** @brief Flag indicating if the object is visible (rendered). */
    Bool visible_;
    /** @brief The font scaling factor for this object. */
    Float32 font_scale_;
    /** @brief Pointer to the owner (parent) of this object. */
    ZGuiObject* owner_ptr_;
};

} // gui
} // zengine