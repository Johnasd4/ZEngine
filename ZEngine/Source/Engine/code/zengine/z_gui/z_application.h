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
#include "../z_core/t_function.h"
#include "../z_core/t_queue.h"
#include "../z_core/z_task.h"

namespace zengine {
namespace error_code {
/** 
 * @brief Error codes specific to ZApplication operations.
 */
enum ZApplicationErrorCodeEnum : ReturnType::ValueType {
    /** @brief Base error code for ZApplication. */
    kZApplicationErrorCode_LinkError = kErrorCodeBase_ZApplication,
    /** @brief Error occurred in the system or external library. */
    kZApplicationErrorCode_SystemOrLibraryError,
    /** @brief A nullptr parameter was provided where it is not allowed. */
    kZApplicationErrorCode_NullptrParam,
    /** @brief A parameter value is out of the valid range. */
    kZApplicationErrorCode_ParamOutOfRange,
    /** @brief The application is in an invalid state for the requested operation. */
    kZApplicationErrorCode_StateError,
    /** @brief The specified font language does not exist. */
    kZApplicationErrorCode_FontLanguageNotExist,
    /** @brief Failed to load the font language. */
    kZApplicationErrorCode_FontLanguageLoadFailed,
    /** @brief The provided INI file directory is invalid. */
    kZApplicationErrorCode_INIFileDirNotValid,
    /** @brief The provided INI file directory does not exist. */
    kZApplicationErrorCode_INIFileDirNotExist,
};
} // error_code
} // zengine

struct GLFWwindow;
struct ImGuiContext;
struct ImGuiIO;
struct ImFont;

namespace zengine {
namespace gui {
namespace internal {
class ZApplicationCallback;
} // internal
} // gui
} // zengine

namespace zengine {
namespace gui {
class ZWindow;
} // gui
} // zengine

namespace zengine {
namespace gui {

/**
 * @brief Application class responsible for creating the main window and managing the GUI lifecycle.
 *
 * This class encapsulates the main application window using GLFW and integrates with ImGui for rendering.
 * It manages a collection of ZWindow objects (which contain widgets) and handles the main application loop.
 * The implementation relies on an internal task queue to process GUI operations safely on the main thread tick.
 * Input events (keyboard, mouse) are captured and dispatched to bound callback functions.
 */
class GUI_DLLAPI CACHE_ALIGN ZApplication : public ZObject {
public:
    /** @brief Alias for the base class ZObject. */
    using SuperType_ = ZObject;

    /**
     * @brief Enum representing the window screen mode.
     */
    enum class ScreenModeEnum_ : Int32 {
        /** @brief Windowed mode with borders. */
        kWindow,
        /** @brief Full-screen mode with custom resolution. */
        kFullScreenCustomSize,
        /** @brief Full-screen mode using the desktop's default resolution. */
        kFullScreenDefaultSize,
    };

    /**
     * @brief Enum representing the current state of the application.
     */
    enum class StateEnum_ : Int32 {
        /** @brief The application is terminated or not yet initialized. */
        kTerminated,
        /** @brief The application has been initialized but not yet executed. */
        kInitialized,
        /** @brief The application is currently running. */
        kExecuted
    };

    /**
     * @brief Enum representing supported font languages.
     */
    enum class FontLanguageEnum_ : Int32 {
        /** @brief Common Simplified Chinese characters. */
        kSimplifiedChineseCommon,
        /** @brief All Simplified Chinese characters. */
        kSimplifiedChineseAll
    };

public:
    /**
     * @brief Constructs a new ZApplication instance.
     * @param _name The title of the application window.
     * @param _size The initial size of the window.
     * @param _pos The initial position of the window.
     * @param _screen_mode The initial screen mode (windowed or full-screen).
     */
    ZApplication(
        const Char* _name,
        GuiSize _size,
        GuiPos _pos,
        ScreenModeEnum_ _screen_mode = ScreenModeEnum_::kWindow
    ) noexcept;
    
    /** @brief Deleted copy constructor. */
    ZApplication(const ZApplication&) = delete;

    /** @brief Deleted move constructor. */
    ZApplication(ZApplication&&) = delete;

    /**
     * @brief Destroys the ZApplication instance and cleans up resources.
     */
    virtual ~ZApplication() noexcept;

    /** @brief Deleted copy assignment operator. */
    ZApplication& operator=(const ZApplication&) = delete;

    /** @brief Deleted move assignment operator. */
    ZApplication& operator=(ZApplication&&) = delete;

    /**
     * @brief Checks if the application is currently in full-screen mode.
     * @return True if full-screen, false otherwise.
     */
    NODISCARD Bool IsFullScreen() const noexcept;

    /**
     * @brief Gets the current size of the application window.
     * @return The size of the window.
     */
    NODISCARD GuiSize GetSize() const noexcept;

    /**
     * @brief Gets the current position of the application window.
     * @return The position of the window.
     */
    NODISCARD GuiPos GetPos() const noexcept;

    /**
     * @brief Gets the current absolute position of the application window on the screen.
     * @return The absolute position.
     */
    NODISCARD GuiPos GetAbsPos() const noexcept;

    /**
     * @brief Gets the title of the application window.
     * @return The title string.
     */
    NODISCARD const Char* GetName() const noexcept;

    /**
     * @brief Gets the background colour of the application window.
     * @return The background colour.
     */
    NODISCARD GuiColour GetBackgroundColour() const noexcept;

    /**
     * @brief Gets the current state of the application.
     * @return The application state.
     */
    NODISCARD FORCEINLINE StateEnum_ GetState() const noexcept { return state_; }

    /**
     * @brief Sets the visibility of the application window.
     * @param _visible True to show the window, false to hide it.
     */
    Void SetVisible(Bool _visible) noexcept;

    /**
     * @brief Sets the size of the application window.
     * @param _size The new size of the window.
     */
    Void SetSize(GuiSize _size) noexcept;

    /**
     * @brief Sets the position of the application window.
     * @param _pos The new position of the window.
     */
    Void SetPos(GuiPos _pos) noexcept;

    /**
     * @brief Sets the absolute position of the application window on the screen.
     * @param _pos The new absolute position.
     */
    Void SetAbsPos(GuiPos _pos) noexcept;

    /**
     * @brief Sets the title of the application window.
     * @param _name The new title string.
     */
    Void SetName(const Char* _name) noexcept;

    /**
     * @brief Sets the background colour of the application window.
     * @param _colour The new background colour.
     */
    Void SetBackgroundColour(GuiColour _colour) noexcept;

    /**
     * @brief Sets the screen mode (windowed or full-screen).
     * @param _screen_mode The desired screen mode.
     */
    Void SetScreenMode(ScreenModeEnum_ _screen_mode) noexcept;

    /**
     * @brief Moves the application window to the center of the screen.
     */
    Void SetPosToScreenCenter() noexcept;

    /**
     * @brief Sets the vertical synchronization interval.
     * @param _tick_per_app_tick The number of screen updates to wait for before swapping buffers (0 for immediate).
     */
    Void SetVerticalSynchronization(Int32 _tick_per_app_tick) noexcept;

    /**
     * @brief Binds a callback function for key down events.
     * @param _callback_func The function to call when a key is pressed down. 
     *        (KeyEnum key, Bool shift, Bool ctrl, Bool alt)
     */
    FORCEINLINE Void BindKeyDownCallback(TFunction<Void(KeyEnum, Bool, Bool, Bool)>&& _callback_func) noexcept {
        key_down_callback_func_ = std::move(_callback_func);
    }
    /**
     * @brief Binds a callback function for key up events.
     * @param _callback_func The function to call when a key is released.
     *        (KeyEnum key, Bool shift, Bool ctrl, Bool alt)
     */
    FORCEINLINE Void BindKeyUpCallback(TFunction<Void(KeyEnum, Bool, Bool, Bool)>&& _callback_func) noexcept {
        key_up_callback_func_ = std::move(_callback_func);
    }
    /**
     * @brief Binds a callback function for key press (repeat) events.
     * @param _callback_func The function to call when a key is pressed (repeatedly).
     *        (KeyEnum key, Bool shift, Bool ctrl, Bool alt)
     */
    FORCEINLINE Void BindKeyPressCallback(TFunction<Void(KeyEnum, Bool, Bool, Bool)>&& _callback_func) noexcept {
        key_press_callback_func_ = std::move(_callback_func);
    }
    /**
     * @brief Binds a callback function for mouse button down events.
     * @param _callback_func The function to call when a mouse button is pressed down.
     *        (MouseButtonEnum button, Bool shift, Bool ctrl, Bool alt)
     */
    FORCEINLINE Void BindMouseDownCallback(TFunction<Void(MouseButtonEnum, Bool, Bool, Bool)>&& _callback_func) noexcept {
        mouse_down_callback_func_ = std::move(_callback_func);
    }
    /**
     * @brief Binds a callback function for mouse button up events.
     * @param _callback_func The function to call when a mouse button is released.
     *        (MouseButtonEnum button, Bool shift, Bool ctrl, Bool alt)
     */
    FORCEINLINE Void BindMouseUpCallback(TFunction<Void(MouseButtonEnum, Bool, Bool, Bool)>&& _callback_func) noexcept {
        mouse_up_callback_func_ = std::move(_callback_func);
    }
    /**
     * @brief Binds a callback function for scroll wheel movement events.
     * @param _callback_func The function to call when the scroll wheel moves.
     *        (Float32 x_offset, Float32 y_offset)
     */
    FORCEINLINE Void BindScrollMoveCallback(TFunction<Void(Float32, Float32)>&& _callback_func) noexcept {
        scroll_move_callback_func_ = std::move(_callback_func);
    }
    /**
     * @brief Binds a callback function for mouse movement events.
     * @param _callback_func The function to call when the mouse moves.
     *        (GuiPos pre_pos, GuiPos cur_pos)
     */
    FORCEINLINE Void BindMouseMoveCallback(TFunction<Void(GuiPos, GuiPos)>&& _callback_func) noexcept {
        mouse_move_callback_func_ = std::move(_callback_func);
    }
    
    /**
     * @brief Starts the main application loop.
     * @return kOK on successful execution, or an error code on failure.
     */
    NODISCARD ReturnType Execute() noexcept;

    /**
     * @brief Adds a window to the application.
     * @param _window_ptr Pointer to the ZWindow to add.
     */
    Void AddWindow(ZWindow* _window_ptr) noexcept;

    /**
     * @brief Removes a window from the application.
     * @param _window_ptr Pointer to the ZWindow to remove.
     */
    Void RemoveWindow(ZWindow* _window_ptr) noexcept;

    /**
     * @brief Removes all windows from the application.
     */
    Void RemoveAllWindow() noexcept;

    /**
     * @brief Adds a task to the application's task queue.
     * @param _task The task to be executed during the application tick.
     */
    Void AddTask(ZTask&& _task) noexcept;

    /**
     * @brief Loads a font from a TTF file.
     * @param _file_path Path to the TTF font file.
     * @param _font_size Size of the font to load.
     * @param _font_language The language character set to load.
     * @return kOK on success, or an error code on failure.
     */
    NODISCARD virtual ReturnType LoadFontFromFileTTF(
        const Char* _file_path,
        Float32 _font_size,
        FontLanguageEnum_ _font_language
    ) noexcept;

    /**
     * @brief Saves the current application settings (e.g., window layout) to an INI file.
     * @param _file_dir Directory where the settings file should be saved.
     * @return kOK on success, or an error code on failure.
     */
    NODISCARD virtual ReturnType SaveSettings(const Char* _file_dir) noexcept;

    /**
     * @brief Loads application settings from an INI file.
     * @param _file_dir Directory where the settings file is located.
     * @return kOK on success, or an error code on failure.
     */
    NODISCARD virtual ReturnType LoadSettings(const Char* _file_dir) noexcept;

    /**
     * @brief Called every frame to update the application logic.
     * @param _delta_sec Time elapsed since the last frame in seconds.
     */
    virtual Void OnTick(Float32 _delta_sec) noexcept;

    /**
     * @brief Called when the application is being destroyed.
     */
    virtual Void OnDestroy() noexcept;

private:
    /**
     * @brief Internal helper to create the window platform resources.
     * @param _name The window title.
     * @param _size The window size.
     * @param _pos The window position.
     * @param _screen_mode The screen mode.
     * @return kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType CreateP(
        const Char* _name, 
        GuiSize _size, 
        GuiPos _pos,
        ScreenModeEnum_ _screen_mode
    ) noexcept;

    /**
     * @brief Internal helper to destroy the window platform resources.
     */
    Void DestroyP() noexcept;

    /**
     * @brief Internal helper to create the ImGui context.
     * @return kOK on success, or an error code on failure.
     */
    NODISCARD ReturnType CreateContextP() noexcept;
    
private:
    friend class internal::ZApplicationCallback;

private:
    /** @brief Array of pointers to managed ZWindow objects. */
    TArray<ZWindow*> window_ptr_set_;
    /** @brief Thread-safe queue for storing tasks to be executed on the main thread. */
    TQueueSafe<ZTask> task_queue_;
    /** @brief Pointer to the GLFW window handle. */
    GLFWwindow* application_handle_;
    /** @brief Pointer to the ImGui context. */
    ImGuiContext* imgui_context_ptr_;
    /** @brief Pointer to the ImGui IO structure. */
    ImGuiIO* imgui_io_ptr_;
    /** @brief Current state of the application. */
    StateEnum_ state_;

    /** @brief Callback function for key down events. */
    TFunction<Void(KeyEnum, Bool, Bool, Bool)> key_down_callback_func_;
    /** @brief Callback function for key up events. */
    TFunction<Void(KeyEnum, Bool, Bool, Bool)> key_up_callback_func_;
    /** @brief Callback function for key press (repeat) events. */
    TFunction<Void(KeyEnum, Bool, Bool, Bool)> key_press_callback_func_;
    /** @brief Callback function for mouse button down events. */
    TFunction<Void(MouseButtonEnum, Bool, Bool, Bool)> mouse_down_callback_func_;
    /** @brief Callback function for mouse button up events. */
    TFunction<Void(MouseButtonEnum, Bool, Bool, Bool)> mouse_up_callback_func_;
    /** @brief Callback function for scroll wheel movement events. */
    TFunction<Void(Float32, Float32)> scroll_move_callback_func_;
    /** @brief Callback function for mouse movement events. */
    TFunction<Void(GuiPos, GuiPos)> mouse_move_callback_func_;
};

} // gui
} // zengine