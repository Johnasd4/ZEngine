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

#include "d_lib.h"

namespace zengine {
namespace gui {

/**
 * @brief The size of the GUI objects.
 *
 * This structure represents the 2D dimensions (width and height) of a GUI element using floating-point values.
 * It is designed as a lightweight value type and explicitly prohibits heap allocation via new/delete
 * to enforce stack allocation or embedding within other structures, ensuring memory predictability.
 */
struct GuiSize {
    NO_NEW_DELETE
public:
    /**
     * @brief Checks if two GuiSize objects are equal.
     * @param _size The other GuiSize object to compare with.
     * @return True if both width and height are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const GuiSize& _size) const noexcept { 
        return width_ == _size.width_ && height_ == _size.height_;
    }

    /**
     * @brief Checks if two GuiSize objects are different.
     * @param _size The other GuiSize object to compare with.
     * @return True if either width or height is different, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const GuiSize& _size) const noexcept {
        return width_ != _size.width_ || height_ != _size.height_;
    }

    /** @brief The width of the object. */
    Float32 width_;
    /** @brief The height of the object. */
    Float32 height_;
};

/**
 * @brief The position of the GUI objects.
 *
 * This structure represents the 2D coordinates (x and y) of a GUI element using floating-point values.
 * It supports basic vector arithmetic operations (addition and subtraction) to facilitate layout calculations.
 * Heap allocation is explicitly forbidden to prevent fragmentation and ensure deterministic memory usage.
 */
struct GuiPos {
    NO_NEW_DELETE
public:
    /**
     * @brief Checks if two GuiPos objects are equal.
     * @param _pos The other GuiPos object to compare with.
     * @return True if both x and y coordinates are equal, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const GuiPos& _pos) const noexcept {
        return x_ == _pos.x_ && y_ == _pos.y_;
    }

    /**
     * @brief Checks if two GuiPos objects are different.
     * @param _pos The other GuiPos object to compare with.
     * @return True if either x or y coordinates are different, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const GuiPos& _pos) const noexcept {
        return x_ != _pos.x_ || y_ != _pos.y_;
    }

    /**
     * @brief Adds two GuiPos objects component-wise.
     * @param _pos The other GuiPos object to add.
     * @return A new GuiPos containing the sum of the x and y components.
     */
    NODISCARD FORCEINLINE GuiPos operator+(const GuiPos& _pos) const noexcept {
        return GuiPos(x_ + _pos.x_, y_ + _pos.y_);
    }

    /**
     * @brief Subtracts a GuiPos object from this one component-wise.
     * @param _pos The other GuiPos object to subtract.
     * @return A new GuiPos containing the difference of the x and y components.
     */
    NODISCARD FORCEINLINE GuiPos operator-(const GuiPos& _pos) const noexcept {
        return GuiPos(x_ - _pos.x_, y_ - _pos.y_);
    }

    /** @brief The x coordinate. */
    Float32 x_;
    /** @brief The y coordinate. */
    Float32 y_;
};

/**
 * @brief The colour of the GUI objects.
 *
 * This structure represents an RGBA color value using floating-point components ranging from 0.0 to 1.0.
 * It encapsulates the red, green, blue, and alpha channels necessary for rendering UI elements with transparency.
 */
struct GuiColour {
    /** @brief The red component (0.0 to 1.0). */
    Float32 red_;
    /** @brief The green component (0.0 to 1.0). */
    Float32 green_;
    /** @brief The blue component (0.0 to 1.0). */
    Float32 blue_;
    /** @brief The alpha component (0.0 to 1.0). */
    Float32 alpha_;
};

} // gui
} // zengine