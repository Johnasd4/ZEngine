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

/**
 * @brief Print manager class.
 *
 * Handles thread-safe print operations to the console.
 */
class ZPrintManager : public ZObject {
public:
    /**
     * @brief Prints a string view to the standard output.
     *
     * This method is thread-safe.
     *
     * @param _str_view The string view content to print.
     */
    static Void Print(ZStringView _str_view) noexcept {
        static ZPrintManager& print_manager = ZPrintManager::InstanceP();
        TLockGuard lock_guard(print_manager.print_mutex_);
        std::fwrite(_str_view.DataPtr(), sizeof(Char), _str_view.Size(), stdout);
    }

    /**
     * @brief Prints a string view to the standard output.
     *
     * This method is thread-safe.
     *
     * @param _str_view The string view content to print.
     */
    static Void Print(ZWStringView _str_view) noexcept {
        static ZPrintManager& print_manager = ZPrintManager::InstanceP();
        TLockGuard lock_guard(print_manager.print_mutex_);
        std::fwrite(_str_view.DataPtr(), sizeof(WChar), _str_view.Size(), stdout);
    }

protected:
    /** Type alias for the base class. */
    using SuperType_ = ZObject;

private:
    /**
     * @brief Retrieves the singleton instance of the print manager.
     *
     * @return Reference to the singleton instance.
     */
    NODISCARD static ZPrintManager& InstanceP() {
        static ZPrintManager instance;
        return instance;
    }

    /**
     * @brief Default constructor.
     *
     * Initializes the console settings. On Windows, it enables ANSI escape codes
     * and sets the console output code page to UTF-8.
     */
    ZPrintManager() : SuperType_() {
#ifdef _WIN32
        // Windows  open to supply ANSI
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return;

        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) return;

        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);

        //set output as UTF-8
        SetConsoleOutputCP(CP_UTF8);
#else
        // Linux / macOS default supply ANSI and UTF-8.
#endif   
    }

    /** Mutex for synchronizing print operations. */
    ZMutex print_mutex_;
};

}//internal
}//console
}//zengine

namespace zengine {
namespace console {
namespace internal {

Void PrintP(ZStringView _format, fmt::format_args _args, SizeType _arg_num) noexcept {
    try {
        ZString str;
        str.Reserve(_format.Size() + _arg_num * 16ULL);
        fmt::vformat_to(
            std::back_inserter(str.STDString()),
            _format.STDStringView(),
            _args
        );
        Print(str);
    }
    catch (const fmt::format_error&) {
        Z_LOG_ERROR(
            zengine::error_code::kFConsoleErrorCode_FormatError, 0,
            "console::Print() format error! _format: %s",
            _format.ToString().DataPtr()
        );
    }
}

Void PrintP(ZWStringView _format, fmt::wformat_args _args, SizeType _arg_num) noexcept {
    try {
        ZWString str;
        str.Reserve(_format.Size() + _arg_num * 16ULL);
        fmt::vformat_to(
            std::back_inserter(str.STDString()),
            _format.STDStringView(),
            _args
        );
        Print(str);
    }
    catch (const fmt::format_error&) {
        Z_LOG_ERROR(
            zengine::error_code::kFConsoleErrorCode_FormatError, 0,
            "console::Print() format error! _format: %s",
            string::WStringToString(_format).DataPtr()
        );
    }
}

}//internal
}//console
}//zengine

namespace zengine {
namespace console {

CORE_DLLAPI Void Print(ZStringView _str_view) noexcept {
    internal::ZPrintManager::Print(_str_view);
}

CORE_DLLAPI Void Print(ZWStringView _str_view) noexcept {
    internal::ZPrintManager::Print(_str_view);
}

}//console
}//zengine
