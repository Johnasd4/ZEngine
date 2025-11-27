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

#include "z_file.h"

#include "f_file_system.h"
#include "m_log.h"

namespace zengine {

ZFile::ZFile() noexcept : SuperType_(), file_ptr_(nullptr) {}

ZFile::ZFile(ZFile&& _file) noexcept : SuperType_(std::forward<ZFile>(_file)) { 
    MoveP(std::forward<ZFile>(_file)); 
}

ZFile::~ZFile() noexcept { if (file_ptr_ != nullptr) { fclose(file_ptr_); } }

ZFile& ZFile::operator=(ZFile&& _file) noexcept {
    SuperType_::operator=(std::forward<ZFile>(_file));
    MoveP(std::forward<ZFile>(_file));
    return *this;
}

NODISCARD ReturnType ZFile::Read(Void* _data_ptr, SizeType _data_size) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCode_NoFileOpened, L"No file opened!");

    SizeType read_size = fread(_data_ptr, 1LL, _data_size, file_ptr_);
    if (read_size != _data_size) {
        ret_val = error_code::kZFileErrorCode_ReadFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to read file! data_size: %lld, read_size: %lld", _data_size, read_size);
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Write(const Void* _data_ptr, SizeType _data_size) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCode_NoFileOpened, L"No file opened!");

    SizeType write_size = fwrite(_data_ptr, 1LL, _data_size, file_ptr_);
    if (write_size != _data_size) {
        ret_val = error_code::kZFileErrorCode_WriteFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to write file! data_size: %lld, write_size: %lld", _data_size, write_size);
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Scan(const Char* _format, ArgListType _args) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCode_NoFileOpened, L"No file opened!");

    if (vfscanf(file_ptr_, _format, _args) == EOF) {
        ret_val = error_code::kZFileErrorCode_ReadFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to scan file!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Scan(const Char* _format, ...) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCode_NoFileOpened, L"No file opened!");

    ArgListType args;
    va_start(args, _format);

    if (vfscanf(file_ptr_, _format, args) == EOF) {
        va_end(args);
        ret_val = error_code::kZFileErrorCode_ReadFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to scan file!");
        return ret_val;
    }
    va_end(args);

    return ret_val;
}

NODISCARD ReturnType ZFile::Scan(const WChar* _format, ArgListType _args) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCode_NoFileOpened, L"No file opened!");

    if (vfwscanf(file_ptr_, _format, _args) == EOF) {
        ret_val = error_code::kZFileErrorCode_ReadFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to scan file!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Scan(const WChar* _format, ...) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCode_NoFileOpened, L"No file opened!");

    ArgListType args;
    va_start(args, _format);

    if (vfwscanf(file_ptr_, _format, args) == EOF) {
        va_end(args);
        ret_val = error_code::kZFileErrorCode_ReadFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to scan file!");
        return ret_val;
    }
    va_end(args);

    return ret_val;
}

NODISCARD ReturnType ZFile::Print(const Char* _format, ArgListType _args) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCode_NoFileOpened, L"No file opened!");

    if (vfprintf(file_ptr_, _format, _args) < 0) {
        ret_val = error_code::kZFileErrorCode_WriteFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to print file!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Print(const Char* _format, ...) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCode_NoFileOpened, L"No file opened!");

    ArgListType args;
    va_start(args, _format);

    if (vfprintf(file_ptr_, _format, args) < 0) {
        va_end(args);
        ret_val = error_code::kZFileErrorCode_WriteFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to print file!");
        return ret_val;
    }
    va_end(args);

    return ret_val;
}

NODISCARD ReturnType ZFile::Print(const WChar* _format, ArgListType _args) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCode_NoFileOpened, L"No file opened!");

    if (vfwprintf(file_ptr_, _format, _args) < 0) {
        ret_val = error_code::kZFileErrorCode_WriteFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to print file!");
        return ret_val;
    }

    return ret_val;
}


NODISCARD ReturnType ZFile::Print(const WChar* _format, ...) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCode_NoFileOpened, L"No file opened!");

    ArgListType args;
    va_start(args, _format);

    if (vfwprintf(file_ptr_, _format, args) < 0) {
        va_end(args);
        ret_val = error_code::kZFileErrorCode_WriteFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to print file!");
        return ret_val;
    }
    va_end(args);

    return ret_val;
}

NODISCARD ReturnType ZFile::Open(const WChar* _file_dir, const WChar* _open_type) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ != nullptr, error_code::kZFileErrorCode_OtherFileOpened, L"Another file is opened!");

    file_ptr_ = _wfopen(_file_dir, _open_type);
    if (file_ptr_ == nullptr) {
        ret_val = error_code::kZFileErrorCode_OpenFileFailed;
        Z_LOG_ERROR(ret_val, 0, L"Open file failed! file_dir: %s, open_type: %s", _file_dir, _open_type);
        return ret_val;
    }
    
    return ret_val;
}

NODISCARD ReturnType ZFile::OpenSafe(const WChar* _file_dir, const WChar* _open_type) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    file_system::ZFileInfo file_info;
    link_code = file_system::GetFileInfoByPath(_file_dir, &file_info);
    if (link_code != kOK) {
        ret_val = error_code::kZFileErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"file_system::GetFileInfoByPath() link error!");
        return ret_val;
    }

    if (!file_system::PathExist(file_info.directory_.String())) {
        link_code = file_system::CreateDirectoryByPath(file_info.directory_.String());
        if (link_code != kOK) {
            ret_val = error_code::kZFileErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"file_system::CreateDirectoryByPath() link error!");
            return ret_val;
        }
    }

    link_code = Open(_file_dir, _open_type);
    if (link_code != kOK) {
        ret_val = error_code::kZFileErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZFile::Open() link error!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Close() noexcept {
    ReturnType ret_val = kOK;

    if (file_ptr_ == nullptr) {
        return ret_val;
    }
    
    if (fclose(file_ptr_) != 0) {
        ret_val = error_code::kZFileErrorCode_CloseFileFailed;
        Z_LOG_ERROR(ret_val, 0, L"Close file failed!");
        return ret_val;
    }
    
    file_ptr_ = nullptr;

    return ret_val;
}

NODISCARD ReturnType ZFile::Seek(Int32 _offset, SeekType_ _seek_type) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCode_NoFileOpened, L"No file opened!");

    if (fseek(file_ptr_, _offset, _seek_type) != 0) {
        ret_val = error_code::kZFileErrorCode_CreatePathFailed;
        Z_LOG_ERROR(ret_val, 0, L"Failed to seek! offset: %d, seek_type: %d", _offset, _seek_type);
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Tell(Int32* _pos_ptr) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr_ == nullptr, error_code::kZFileErrorCode_NoFileOpened, L"No file opened!");

    *_pos_ptr = ftell(file_ptr_);

    return ret_val;
}

NODISCARD Int32 ZFile::Size() noexcept {
    if (file_ptr_ == nullptr) {
        return 0;
    }
    Int32 pre_index = ftell(file_ptr_);
    fseek(file_ptr_, 0, kZFileSeekType_FileEnd);
    Int32 size = ftell(file_ptr_);
    fseek(file_ptr_, pre_index, kZFileSeekType_FileHead);
    return size;
}

NODISCARD Void ZFile::Flush() noexcept {
    if (file_ptr_ == nullptr) {
        return;
    }
    fflush(file_ptr_);
}

Void ZFile::MoveP(ZFile&& _file) noexcept {
    file_ptr_ = _file.file_ptr_;
    _file.file_ptr_ = nullptr;
}

}//zengine
