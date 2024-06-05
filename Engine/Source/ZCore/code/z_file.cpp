/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#define CORE_DLLFILE

#include "z_file.h"
#include "m_log.h"

namespace zengine {

CORE_DLLAPI NODISCARD ReturnType ZFile::Seek(Int32 offset, SeekType seek_type) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr == nullptr, kZFileErrorCodeNoFileOpened, "No file opened!");

    if (fseek(file_ptr, offset, seek_type) != 0) {
        ret_val = kZFileErrorCodeCreatePathFailed;
        Z_LOG_ERROR(ret_val, 0, "Failed to seek! offset: %d, seek_type: %d", offset, seek_type);
        return ret_val;
    }

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType ZFile::Tell(Int32* pos_ptr) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr == nullptr, kZFileErrorCodeNoFileOpened, "No file opened!");

    *pos_ptr = ftell(file_ptr);
    
    return ret_val;
}

CORE_DLLAPI NODISCARD Bool ZFile::PathExist(const CChar* path_dir) noexcept {
    return GetFileAttributesA(path_dir) == INVALID_FILE_ATTRIBUTES;
}

CORE_DLLAPI NODISCARD Bool ZFile::PathExist(const TChar* path_dir) noexcept {
    return GetFileAttributesW(path_dir) == INVALID_FILE_ATTRIBUTES;
}

CORE_DLLAPI NODISCARD ReturnType ZFile::CreatePath(const CChar* path_dir) noexcept {
    ReturnType ret_val = kOK;

    if (!CreateDirectoryA(path_dir, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        ret_val = kZFileErrorCodeCreatePathFailed;
        Z_LOG_ERROR(ret_val, 0, "Create path failed! path_dir: %s", path_dir);
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType ZFile::CreatePath(const TChar* path_dir) noexcept {
    ReturnType ret_val = kOK;

    if (!CreateDirectoryW(path_dir, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        ret_val = kZFileErrorCodeCreatePathFailed;
        Z_LOG_ERROR(ret_val, 0, "Create path failed! path_dir: %s", path_dir);
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType ZFile::Open(const CChar* file_dir, const CChar* open_type) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr != nullptr, kZFileErrorCodeOtherFileOpened, "Another file is opened!");

    file_ptr = fopen(file_dir, open_type);
    if (file_ptr == nullptr) {
        ret_val = kZFileErrorCodeOpenFileFailed;
        Z_LOG_ERROR(ret_val, 0, "Open file failed! file_dir: %s, open_type: %s", file_dir, open_type);
        return ret_val;
    }

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType ZFile::Open(const TChar* file_dir, const TChar* open_type) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr != nullptr, kZFileErrorCodeOtherFileOpened, "Another file is opened!");

    file_ptr = _wfopen(file_dir, open_type);
    if (file_ptr == nullptr) {
        ret_val = kZFileErrorCodeOpenFileFailed;
        Z_LOG_ERROR(ret_val, 0, "Open file failed! file_dir: %s, open_type: %s", file_dir, open_type);
        return ret_val;
    }
    
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType ZFile::OpenSafe(const CChar* path_dir, const CChar* file_dir,
                                                 const CChar* open_type) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (!PathExist(path_dir)) {
        link_code = CreatePath(path_dir);
        if (link_code != kOK) {
            ret_val = kZFileErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, link_code, "ZFile::CreatePath() link error!");
            return ret_val;
        }
    }

    link_code = Open(file_dir, open_type);
    if (link_code != kOK) {
        ret_val = kZFileErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZFile::Open() link error!");
        return ret_val;
    }

    return ret_val;
}


CORE_DLLAPI NODISCARD ReturnType ZFile::OpenSafe(const TChar* path_dir, const TChar* file_dir, 
                                                 const TChar* open_type) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (!PathExist(path_dir)) {
        link_code = CreatePath(path_dir);
        if (link_code != kOK) {
            ret_val = kZFileErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, link_code, "ZFile::CreatePath() link error!");
            return ret_val;
        }
    }

    link_code = Open(file_dir, open_type);
    if (link_code != kOK) {
        ret_val = kZFileErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZFile::Open() link error!");
        return ret_val;
    }

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType ZFile::Close() noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr == nullptr, kZFileErrorCodeNoFileOpened, "No file opened!");
    
    if (fclose(file_ptr) != 0) {
        ret_val = kZFileErrorCodeCloseFileFailed;
        Z_LOG_ERROR(ret_val, 0, "Close file failed!");
        return ret_val;
    }
    
    file_ptr = nullptr;

    return ret_val;
}

}//zengine
