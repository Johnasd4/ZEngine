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

#include "m_log.h"

#include "z_file.h"

namespace zengine {

NODISCARD ReturnType ZFile::Read(Void* data_ptr, SizeType data_size) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, "No file opened!");

    SizeType read_size = fread(data_ptr, data_size, 1LL, file_ptr_);
    if (read_size != data_size) {
        ret_val = error_code::kZFileErrorCodeReadFailed;
        Z_LOG_ERROR(ret_val, 0, "Failed to read file! data_size: %lld, read_size: %lld", data_size, read_size);
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Write(Void* data_ptr, SizeType data_size) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, "No file opened!");

    SizeType write_size = fwrite(data_ptr, data_size, 1LL, file_ptr_);
    if (write_size != data_size) {
        ret_val = error_code::kZFileErrorCodeWriteFailed;
        Z_LOG_ERROR(ret_val, 0, "Failed to write file! data_size: %lld, write_size: %lld", data_size, write_size);
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Scan(const Char* format, ArgListType args) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, "No file opened!");

    if (vfscanf(file_ptr_, format, args) == EOF) {
        ret_val = error_code::kZFileErrorCodeReadFailed;
        Z_LOG_ERROR(ret_val, 0, "Failed to scan file!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Scan(const Char* format, ...) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, "No file opened!");

    ArgListType args;
    va_start(args, format);

    if (vfscanf(file_ptr_, format, args) == EOF) {
        va_end(args);
        ret_val = error_code::kZFileErrorCodeReadFailed;
        Z_LOG_ERROR(ret_val, 0, "Failed to scan file!");
        return ret_val;
    }
    va_end(args);

    return ret_val;
}

NODISCARD ReturnType ZFile::Scan(const WChar* format, ArgListType args) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, "No file opened!");

    if (vfwscanf(file_ptr_, format, args) == EOF) {
        ret_val = error_code::kZFileErrorCodeReadFailed;
        Z_LOG_ERROR(ret_val, 0, "Failed to scan file!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Scan(const WChar* format, ...) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, "No file opened!");

    ArgListType args;
    va_start(args, format);

    if (vfwscanf(file_ptr_, format, args) == EOF) {
        va_end(args);
        ret_val = error_code::kZFileErrorCodeReadFailed;
        Z_LOG_ERROR(ret_val, 0, "Failed to scan file!");
        return ret_val;
    }
    va_end(args);

    return ret_val;
}

NODISCARD ReturnType ZFile::Print(const Char* format, ArgListType args) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, "No file opened!");

    if (vfprintf(file_ptr_, format, args) < 0) {
        ret_val = error_code::kZFileErrorCodeWriteFailed;
        Z_LOG_ERROR(ret_val, 0, "Failed to print file!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Print(const Char* format, ...) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, "No file opened!");

    ArgListType args;
    va_start(args, format);

    if (vfprintf(file_ptr_, format, args) < 0) {
        va_end(args);
        ret_val = error_code::kZFileErrorCodeWriteFailed;
        Z_LOG_ERROR(ret_val, 0, "Failed to print file!");
        return ret_val;
    }
    va_end(args);

    return ret_val;
}

NODISCARD ReturnType ZFile::Print(const WChar* format, ArgListType args) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, "No file opened!");

    if (vfwprintf(file_ptr_, format, args) < 0) {
        ret_val = error_code::kZFileErrorCodeWriteFailed;
        Z_LOG_ERROR(ret_val, 0, "Failed to print file!");
        return ret_val;
    }

    return ret_val;
}


NODISCARD ReturnType ZFile::Print(const WChar* format, ...) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, "No file opened!");

    ArgListType args;
    va_start(args, format);

    if (vfwprintf(file_ptr_, format, args) < 0) {
        va_end(args);
        ret_val = error_code::kZFileErrorCodeWriteFailed;
        Z_LOG_ERROR(ret_val, 0, "Failed to print file!");
        return ret_val;
    }
    va_end(args);

    return ret_val;
}

NODISCARD Bool ZFile::PathExist(const Char* path_dir) noexcept {
    return GetFileAttributesA(path_dir) != INVALID_FILE_ATTRIBUTES;
}

NODISCARD Bool ZFile::PathExist(const WChar* path_dir) noexcept {
    return GetFileAttributesW(path_dir) != INVALID_FILE_ATTRIBUTES;
}

NODISCARD ReturnType ZFile::CreatePath(const Char* path_dir) noexcept {
    ReturnType ret_val = kOK;

    if (!CreateDirectoryA(path_dir, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        ret_val = error_code::kZFileErrorCodeCreatePathFailed;
        Z_LOG_ERROR(ret_val, 0, "Create path failed! path_dir: %s", path_dir);
        return ret_val;
    }
    return ret_val;
}

NODISCARD ReturnType ZFile::CreatePath(const WChar* path_dir) noexcept {
    ReturnType ret_val = kOK;

    if (!CreateDirectoryW(path_dir, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        ret_val = error_code::kZFileErrorCodeCreatePathFailed;
        Z_LOG_ERROR(ret_val, 0, "Create path failed! path_dir: %s", path_dir);
        return ret_val;
    }
    return ret_val;
}

NODISCARD ReturnType ZFile::Open(const Char* file_dir, const Char* open_type) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ != nullptr, error_code::kZFileErrorCodeOtherFileOpened, "Another file is opened!");

    file_ptr_ = fopen(file_dir, open_type);
    if (file_ptr_ == nullptr) {
        ret_val = error_code::kZFileErrorCodeOpenFileFailed;
        Z_LOG_ERROR(ret_val, 0, "Open file failed! file_dir: %s, open_type: %s", file_dir, open_type);
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Open(const WChar* file_dir, const WChar* open_type) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ != nullptr, error_code::kZFileErrorCodeOtherFileOpened, "Another file is opened!");

    file_ptr_ = _wfopen(file_dir, open_type);
    if (file_ptr_ == nullptr) {
        ret_val = error_code::kZFileErrorCodeOpenFileFailed;
        Z_LOG_ERROR(ret_val, 0, "Open file failed! file_dir: %s, open_type: %s", file_dir, open_type);
        return ret_val;
    }
    
    return ret_val;
}

NODISCARD ReturnType ZFile::OpenSafe(const Char* path_dir, const Char* file_dir,
                                                 const Char* open_type) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (!PathExist(path_dir)) {
        link_code = CreatePath(path_dir);
        if (link_code != kOK) {
            ret_val = error_code::kZFileErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, link_code, "ZFile::CreatePath() link error!");
            return ret_val;
        }
    }

    link_code = Open(file_dir, open_type);
    if (link_code != kOK) {
        ret_val = error_code::kZFileErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZFile::Open() link error!");
        return ret_val;
    }

    return ret_val;
}


NODISCARD ReturnType ZFile::OpenSafe(const WChar* path_dir, const WChar* file_dir, 
                                                 const WChar* open_type) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (!PathExist(path_dir)) {
        link_code = CreatePath(path_dir);
        if (link_code != kOK) {
            ret_val = error_code::kZFileErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, link_code, "ZFile::CreatePath() link error!");
            return ret_val;
        }
    }

    link_code = Open(file_dir, open_type);
    if (link_code != kOK) {
        ret_val = error_code::kZFileErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZFile::Open() link error!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Close() noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, "No file opened!");
    
    if (fclose(file_ptr_) != 0) {
        ret_val = error_code::kZFileErrorCodeCloseFileFailed;
        Z_LOG_ERROR(ret_val, 0, "Close file failed!");
        return ret_val;
    }
    
    file_ptr_ = nullptr;

    return ret_val;
}

NODISCARD ReturnType ZFile::Seek(Int32 offset, SeekType seek_type) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, "No file opened!");

    if (fseek(file_ptr_, offset, seek_type) != 0) {
        ret_val = error_code::kZFileErrorCodeCreatePathFailed;
        Z_LOG_ERROR(ret_val, 0, "Failed to seek! offset: %d, seek_type: %d", offset, seek_type);
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Tell(Int32* pos_ptr) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, "No file opened!");

    *pos_ptr = ftell(file_ptr_);

    return ret_val;
}


}//zengine
