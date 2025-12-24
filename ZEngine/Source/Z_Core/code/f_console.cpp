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
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "f_console.h"

#include "f_string.h"
#include "m_log.h"
#include "t_lock_guard.h"
#include "z_mutex.h"

namespace zengine {
namespace console {
namespace internal {

/** @brief Print manager class. */
/** @brief Handles thread-safe print operations to the console. */
class CACHE_LINE_ALIGN ZPrintManager {
public:
    /** @brief Retrieves the singleton instance of the print manager. */
    /** @return Reference to the singleton instance. */
    NODISCARD static ZPrintManager& InstanceP() {
        static ZPrintManager instance;
        return instance;
    }

    /** @brief Prints formatted output string in a thread-safe manner. */
    /** @param _format The format string view. */
    /** @param _args The format arguments. */
    FORCEINLINE Void Print(
        ZStringView _format, 
        fmt::format_args _args
    ) noexcept {
        TLockGuard lock_guard(mutex_);
        PrintP(
            _format,
            _args
        );
    }

    /** @brief Prints formatted output string with specific style and colors in a thread-safe manner. */
    /** @param _front_colour The foreground color. */
    /** @param _back_colour The background color. */
    /** @param _text_style The text style. */
    /** @param _format The format string view. */
    /** @param _args The format arguments. */
    FORCEINLINE Void Print(
        Colour _front_colour,
        Colour _back_colour,
        Int32 _text_style,
        ZStringView _format, 
        fmt::format_args _args
    ) noexcept {
        TLockGuard lock_guard(mutex_);
        PrintP(
            _front_colour,
            _back_colour,
            _text_style,
            _format,
            _args
        );
    }

    /** @brief Prints formatted output string and flushes the buffer immediately in a thread-safe manner. */
    /** @param _format The format string view. */
    /** @param _args The format arguments. */
    FORCEINLINE Void PrintImmediately(
        ZStringView _format, 
        fmt::format_args _args
    ) noexcept {
        TLockGuard lock_guard(mutex_);
        PrintP(
            _format,
            _args
        );
        FlushP();
    }

    /** @brief Prints formatted output string with style and flushes immediately in a thread-safe manner. */
    /** @param _front_colour The foreground color. */
    /** @param _back_colour The background color. */
    /** @param _text_style The text style. */
    /** @param _format The format string view. */
    /** @param _args The format arguments. */
    FORCEINLINE Void PrintImmediately(
        Colour _front_colour,
        Colour _back_colour,
        Int32 _text_style,
        ZStringView _format,
        fmt::format_args _args
    ) noexcept {
        TLockGuard lock_guard(mutex_);
        PrintP(
            _front_colour,
            _back_colour,
            _text_style,
            _format,
            _args
        );
        FlushP();
    }

    /** @brief Flushes the console output buffer in a thread-safe manner. */
    FORCEINLINE Void Flush() noexcept {
        TLockGuard lock_guard(mutex_);
        FlushP();
    }

private:
    /** @brief Deleted operator new to prevent heap allocation. */
    static Void* operator new(SizeType) = delete;
    /** @brief Deleted operator delete. */
    static Void operator delete(Void*) = delete;

    /** @brief Default constructor. */
    /** @brief Initializes the console settings. On Windows, it enables ANSI escape codes and sets UTF-8 output. */
    ZPrintManager() noexcept 
        : mutex_()
    {
#ifdef _WIN32
        // Windows  open to supply ANSI
        HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (output_handle == INVALID_HANDLE_VALUE) return;

        DWORD console_mode = 0;
        if (!GetConsoleMode(output_handle, &console_mode)) return;

        console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(output_handle, console_mode);

        //set output as UTF-8
        SetConsoleOutputCP(CP_UTF8);
#else
        // Linux / macOS default supply ANSI and UTF-8.
#endif   
    }

    /** @brief Destructor. */
    ~ZPrintManager() noexcept {}

    /** @brief Internal print implementation using fmt library. */
    /** @param _format The format string view. */
    /** @param _args The format arguments. */
    FORCEINLINE Void PrintP(
        ZStringView _format, 
        fmt::format_args _args
    ) noexcept {
        try {
            fmt::vprint(_format.STDStringView(), _args);
        }
        catch (const fmt::format_error&) {
            Z_LOG_ERROR(
                zengine::error_code::kFStringErrorCode_FormatError, 0,
                "console::Print() format error! _format: {}",
                _format
            );
        }
    }

    /** @brief Internal print implementation with style using fmt library. */
    /** @param _front_colour The foreground color. */
    /** @param _back_colour The background color. */
    /** @param _text_style The text style. */
    /** @param _format The format string view. */
    /** @param _args The format arguments. */
    FORCEINLINE Void PrintP(
        Colour _front_colour,
        Colour _back_colour,
        Int32 _text_style,
        ZStringView _format,
        fmt::format_args _args
    ) noexcept {
        fmt::text_style text_style = static_cast<fmt::emphasis>(_text_style);
        if (_front_colour != kNoChangeColour) {
            text_style |= fmt::fg(fmt::rgb(_front_colour.red_, _front_colour.green_, _front_colour.blue_));
        }
        if (_back_colour != kNoChangeColour) {
            text_style |= fmt::bg(fmt::rgb(_back_colour.red_, _back_colour.green_, _back_colour.blue_));
        }

        try {
            fmt::vprint(stdout, text_style, _format.STDStringView(), _args);
        }
        catch (const fmt::format_error&) {
            Z_LOG_ERROR(
                zengine::error_code::kFStringErrorCode_FormatError, 0,
                "console::Print() format error! _format: {}",
                _format
            );
        }
    }

    /** @brief Internal flush implementation. */
    FORCEINLINE Void FlushP() noexcept {
        std::fflush(stdout);
    }

private:
    /** @brief Mutex for ensuring thread safety. */
    ZMutex mutex_;
};

}//internal
}//console
}//zengine

namespace zengine {
namespace console {
namespace internal {

CORE_DLLAPI Void Print(
    ZStringView _format, 
    fmt::format_args _args
) noexcept {
    internal::ZPrintManager::InstanceP().Print(
        _format, 
        _args
    );
}

CORE_DLLAPI Void Print(
    Colour _front_colour,
    Colour _back_colour,
    Int32 _text_style,
    ZStringView _format,
    fmt::format_args _args
) noexcept {
    internal::ZPrintManager::InstanceP().Print(
        _front_colour,
        _back_colour,
        _text_style,
        _format,
        _args
    );
}

CORE_DLLAPI Void PrintImmediately(
    ZStringView _format,
    fmt::format_args _args
) noexcept {
    internal::ZPrintManager::InstanceP().PrintImmediately(
        _format,
        _args
    );
}

CORE_DLLAPI Void PrintImmediately(
    Colour _front_colour,
    Colour _back_colour,
    Int32 _text_style,
    ZStringView _format,
    fmt::format_args _args
) noexcept {
    internal::ZPrintManager::InstanceP().PrintImmediately(
        _front_colour,
        _back_colour,
        _text_style,
        _format,
        _args
    );
}

}//internal
}//console
}//zengine

namespace zengine {
namespace console {

CORE_DLLAPI Void Flush() noexcept {
    internal::ZPrintManager::InstanceP().Flush();
}

}//console
}//zengine
