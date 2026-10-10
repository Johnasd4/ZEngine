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

#include "f_system.h"

#include "m_log.h"

namespace zengine {
namespace system {

CORE_DLLAPI Bool IsAdmin() noexcept {
    BOOL is_admin = FALSE;
    PSID admin_group = NULL;
    SID_IDENTIFIER_AUTHORITY nt_authority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&nt_authority, 2,
        SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0, &admin_group))
    {
        if (!CheckTokenMembership(NULL, admin_group, &is_admin))
        {
            is_admin = FALSE;
        }
        FreeSid(admin_group);
    }

    return is_admin == TRUE;
}

CORE_DLLAPI Void RunAsAdmin() noexcept {
    if (IsAdmin()) {
        return;
    }

    WChar path[MAX_PATH];
    GetModuleFileName(NULL, path, ARRAYSIZE(path));

    SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
    sei.lpVerb = L"runas";
    sei.lpFile = path;
    sei.hwnd = NULL;
    sei.nShow = SW_NORMAL;

    // restart program as admin
    if (!ShellExecuteEx(&sei))
    {
        DWORD error = GetLastError();
        if (error == ERROR_CANCELLED) {
            Z_LOG_FAILURE("User denied administrator privileges!");
        }
        else {
            Z_LOG_ERROR(
                error_code::kFSystemErrorCode_SystemOrLibraryError, error, 
                "Unknown system error!"
            );
        }
    }

    // shutdown program
    exit(0);
}

} // system
} // zengine
