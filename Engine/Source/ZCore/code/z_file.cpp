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
#include "z_string.h"

namespace zengine {

NODISCARD ReturnType ZFile::Read(Void* _data_ptr, SizeType _data_size) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, L"No file opened!");

    SizeType read_size = fread(_data_ptr, _data_size, 1LL, file_ptr_);
    if (read_size != _data_size) {
        ret_val = error_code::kZFileErrorCodeReadFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to read file! data_size: %lld, read_size: %lld", _data_size, read_size);
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Write(Void* _data_ptr, SizeType _data_size) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, L"No file opened!");

    SizeType write_size = fwrite(_data_ptr, _data_size, 1LL, file_ptr_);
    if (write_size != _data_size) {
        ret_val = error_code::kZFileErrorCodeWriteFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to write file! data_size: %lld, write_size: %lld", _data_size, write_size);
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Scan(const Char* _format, ArgListType _args) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, L"No file opened!");

    if (vfscanf(file_ptr_, _format, _args) == EOF) {
        ret_val = error_code::kZFileErrorCodeReadFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to scan file!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Scan(const Char* _format, ...) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, L"No file opened!");

    ArgListType args;
    va_start(args, _format);

    if (vfscanf(file_ptr_, _format, args) == EOF) {
        va_end(args);
        ret_val = error_code::kZFileErrorCodeReadFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to scan file!");
        return ret_val;
    }
    va_end(args);

    return ret_val;
}

NODISCARD ReturnType ZFile::Scan(const WChar* _format, ArgListType _args) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, L"No file opened!");

    if (vfwscanf(file_ptr_, _format, _args) == EOF) {
        ret_val = error_code::kZFileErrorCodeReadFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to scan file!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Scan(const WChar* _format, ...) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, L"No file opened!");

    ArgListType args;
    va_start(args, _format);

    if (vfwscanf(file_ptr_, _format, args) == EOF) {
        va_end(args);
        ret_val = error_code::kZFileErrorCodeReadFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to scan file!");
        return ret_val;
    }
    va_end(args);

    return ret_val;
}

NODISCARD ReturnType ZFile::Print(const Char* _format, ArgListType _args) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, L"No file opened!");

    if (vfprintf(file_ptr_, _format, _args) < 0) {
        ret_val = error_code::kZFileErrorCodeWriteFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to print file!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Print(const Char* _format, ...) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, L"No file opened!");

    ArgListType args;
    va_start(args, _format);

    if (vfprintf(file_ptr_, _format, args) < 0) {
        va_end(args);
        ret_val = error_code::kZFileErrorCodeWriteFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to print file!");
        return ret_val;
    }
    va_end(args);

    return ret_val;
}

NODISCARD ReturnType ZFile::Print(const WChar* _format, ArgListType _args) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, L"No file opened!");

    if (vfwprintf(file_ptr_, _format, _args) < 0) {
        ret_val = error_code::kZFileErrorCodeWriteFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to print file!");
        return ret_val;
    }

    return ret_val;
}


NODISCARD ReturnType ZFile::Print(const WChar* _format, ...) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, L"No file opened!");

    ArgListType args;
    va_start(args, _format);

    if (vfwprintf(file_ptr_, _format, args) < 0) {
        va_end(args);
        ret_val = error_code::kZFileErrorCodeWriteFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to print file!");
        return ret_val;
    }
    va_end(args);

    return ret_val;
}

NODISCARD Bool ZFile::PathExist(const Char* _path_dir) noexcept {
    return GetFileAttributesA(_path_dir) != INVALID_FILE_ATTRIBUTES;
}

NODISCARD Bool ZFile::PathExist(const WChar* _path_dir) noexcept {
    return GetFileAttributesW(_path_dir) != INVALID_FILE_ATTRIBUTES;
}

NODISCARD ReturnType ZFile::CreatePath(const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;

    if (!CreateDirectoryA(_path_dir, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        ret_val = error_code::kZFileErrorCodeCreatePathFailed;
        Z_LOG_ERROR(ret_val, 0, L"Create path failed! path_dir: %ls", string::String2WString(_path_dir).String());
        return ret_val;
    }
    return ret_val;
}

NODISCARD ReturnType ZFile::CreatePath(const WChar* _path_dir) noexcept {
    ReturnType ret_val = kOK;

    if (!CreateDirectoryW(_path_dir, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        ret_val = error_code::kZFileErrorCodeCreatePathFailed;
        Z_LOG_ERROR(ret_val, 0, L"Create path failed! path_dir: %ls", _path_dir);
        return ret_val;
    }
    return ret_val;
}

NODISCARD ReturnType ZFile::Open(const Char* _file_dir, const Char* _open_type) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ != nullptr, error_code::kZFileErrorCodeOtherFileOpened, L"Another file is opened!");

    file_ptr_ = fopen(_file_dir, _open_type);
    if (file_ptr_ == nullptr) {
        ret_val = error_code::kZFileErrorCodeOpenFileFailed;
        Z_LOG_ERROR(
            ret_val, 0, L"Open file failed! file_dir: %ls, open_type: %ls", 
            string::String2WString(_file_dir).String(), string::String2WString(_open_type).String());
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Open(const WChar* _file_dir, const WChar* _open_type) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ != nullptr, error_code::kZFileErrorCodeOtherFileOpened, L"Another file is opened!");

    file_ptr_ = _wfopen(_file_dir, _open_type);
    if (file_ptr_ == nullptr) {
        ret_val = error_code::kZFileErrorCodeOpenFileFailed;
        Z_LOG_ERROR(ret_val, 0, L"Open file failed! file_dir: %s, open_type: %s", _file_dir, _open_type);
        return ret_val;
    }
    
    return ret_val;
}

NODISCARD ReturnType ZFile::OpenSafe(const Char* _path_dir, const Char* _file_dir, const Char* _open_type) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (!PathExist(_path_dir)) {
        link_code = CreatePath(_path_dir);
        if (link_code != kOK) {
            ret_val = error_code::kZFileErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZFile::CreatePath() link error!");
            return ret_val;
        }
    }

    link_code = Open(_file_dir, _open_type);
    if (link_code != kOK) {
        ret_val = error_code::kZFileErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZFile::Open() link error!");
        return ret_val;
    }

    return ret_val;
}


NODISCARD ReturnType ZFile::OpenSafe(const WChar* _path_dir, const WChar* _file_dir, const WChar* _open_type) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (!PathExist(_path_dir)) {
        link_code = CreatePath(_path_dir);
        if (link_code != kOK) {
            ret_val = error_code::kZFileErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZFile::CreatePath() link error!");
            return ret_val;
        }
    }

    link_code = Open(_file_dir, _open_type);
    if (link_code != kOK) {
        ret_val = error_code::kZFileErrorCodeLinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZFile::Open() link error!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Close() noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, L"No file opened!");
    
    if (fclose(file_ptr_) != 0) {
        ret_val = error_code::kZFileErrorCodeCloseFileFailed;
        Z_LOG_ERROR(ret_val, 0, L"Close file failed!");
        return ret_val;
    }
    
    file_ptr_ = nullptr;

    return ret_val;
}

NODISCARD ReturnType ZFile::Seek(Int32 _offset, SeekType_ _seek_type) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, L"No file opened!");

    if (fseek(file_ptr_, _offset, _seek_type) != 0) {
        ret_val = error_code::kZFileErrorCodeCreatePathFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to seek! offset: %d, seek_type: %d", _offset, _seek_type);
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Tell(Int32* _pos_ptr) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCodeNoFileOpened, L"No file opened!");

    *_pos_ptr = ftell(file_ptr_);

    return ret_val;
}

}//zengine
