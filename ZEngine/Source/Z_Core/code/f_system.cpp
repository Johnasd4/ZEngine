/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

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

    //restart program as admin
    if (!ShellExecuteEx(&sei))
    {
        DWORD error = GetLastError();
        if (error == ERROR_CANCELLED) {
            Z_LOG_FAILURE("User denied administrator privileges!");
        }
        else {
            Z_LOG_ERROR(
                error_code::kFSystemErrorCode_SystemError, error, 
                "Unknown system error!"
            );
        }
    }

    //shutdown program
    exit(0);
}

}//system
}//zengine
