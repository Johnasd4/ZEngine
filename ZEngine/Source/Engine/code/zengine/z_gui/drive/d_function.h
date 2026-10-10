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
#include "d_type.h"

namespace zengine {
namespace gui {

/**
 * @brief Get current screen size.
 * @return The current size of the screen.
 */
GUI_DLLAPI NODISCARD GuiSize ScreenSize() noexcept;

/**
 * @brief Get current screen center pos.
 * @param _gui_obj_size The size of the GUI object to be centered. Defaults to (0.0f, 0.0f).
 * @return The calculated position representing the center of the screen relative to the object size.
 */
GUI_DLLAPI NODISCARD GuiPos ScreenCenterPos(GuiSize _gui_obj_size = GuiSize(0.0f, 0.0f)) noexcept;

/**
 * @brief Get text line height.
 * @return The height of a single line of text in the current font.
 */
GUI_DLLAPI NODISCARD Float32 TextLineHeight() noexcept;

/**
 * @brief Get text line height with spacing.
 * @return The height of a single line of text including vertical spacing.
 */
GUI_DLLAPI NODISCARD Float32 TextLineHeightWithSpacing() noexcept;

} // gui
} // zengine