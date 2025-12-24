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

#include "z_file.h"

#include "f_file_system.h"
#include "m_log.h"

namespace zengine {

ZFile::ZFile() noexcept 
    : SuperType_()
    , file_()
    , output_buffer_()
    , output_buffer_data_size_()
{}

ZFile::ZFile(ZFile&& _file) noexcept 
    : SuperType_(std::forward<ZFile>(_file))
    , file_(std::move(_file.file_))
    , output_buffer_(std::move(_file.output_buffer_))
    , output_buffer_data_size_(_file.output_buffer_data_size_)
{
    _file.output_buffer_data_size_ = 0ULL;
}

ZFile::~ZFile() noexcept {
    Close();
}

ZFile& ZFile::operator=(ZFile&& _file) noexcept {
    if (this == &_file) {
        return *this;
    }
    SuperType_::operator=(std::forward<ZFile>(_file));

    ReturnType link_code = kOK;
    Close();
    file_ = std::move(_file.file_);
    output_buffer_ = std::move(_file.output_buffer_);
    output_buffer_data_size_ = _file.output_buffer_data_size_;
    _file.output_buffer_data_size_ = 0ULL;
    return *this;
}

NODISCARD Int64 ZFile::ComputeSize() noexcept {
    if (IsOpen() == false) {
        return kFailed;
    }

    ReturnType link_code = FlushP();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kZFileErrorCode_LinkError, link_code,
            "ZFile::FlushP() link error!"
        );
        return kFailed;
    }

    try {
        return file_.size();
    }
    catch (const std::system_error& error) {
        Z_LOG_ERROR(
            error_code::kZFileErrorCode_GetSizeFailed, 0,
            "Get file size failed, {}! error_code: {}",
            error.what(), error.code().value()
        );
        return kFailed;
    }
}

NODISCARD ReturnType ZFile::Open(const Char* _file_dir, Int32 _open_flag) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        _file_dir == nullptr,
        error_code::kZFileErrorCode_NullptrParam,
        "_file_dir is nullptr!"
    );
    Z_CHECK(
        IsOpen(), 
        error_code::kZFileErrorCode_OtherFileOpened, 
        "Another file is opened!"
    );

    try {
        file_ = fmt::file(_file_dir, _open_flag);
    }
    catch (const std::system_error& error) {
        ret_val = error_code::kZFileErrorCode_OpenFileFailed;
        Z_LOG_ERROR(
            ret_val, 0, 
            "Open file failed, {}! error_code: {} file_dir: {}, _open_flag: 0x{:x}", 
            error.what(), error.code().value(), _file_dir, _open_flag
        );     
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::OpenSafe(const Char* _file_dir, Int32 _open_flag) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    file_system::ZFileInfo file_info = file_system::GetFileInfoByPath(_file_dir);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            ret_val, link_code, 
            "file_system::GetFileInfoByPath() link error!"
        );
        return error_code::kZFileErrorCode_LinkError;
    }

    if (!file_system::IsPathExist(file_info.directory_.GetDataPtr())) {
        link_code = file_system::CreateDirectoryByPath(file_info.directory_.GetDataPtr());
        if (link_code != kOK) {
            Z_LOG_ERROR(
                ret_val, link_code,
                "file_system::CreateDirectoryByPath() link error!"
            );
            return error_code::kZFileErrorCode_LinkError;
        }
    }

    link_code = Open(_file_dir, _open_flag);
    if (link_code != kOK) {
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZFile::Open() link error!"
        );
        return error_code::kZFileErrorCode_LinkError;
    }

    return ret_val;
}

Void ZFile::Close() noexcept {
    ReturnType link_code = kOK;

    if (IsOpen()) {
        link_code = FlushP();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kZFileErrorCode_LinkError, link_code,
                "ZFile::FlushP() link error!"
            );
        }
        //might throw error
        try {
            file_.close();
        } catch (...) {}
    }
}

NODISCARD ReturnType ZFile::SetOutputBufferSize(
    SizeType _buffer_size
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    link_code = FlushP();
    if (link_code != kOK) {
        ret_val = error_code::kZFileErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZFile::FlushP() link error!"
        );
        return ret_val;
    }
    output_buffer_.Resize(_buffer_size);

    return ret_val;
}

ReturnType ZFile::Read(
    ZBuffer _buffer, 
    SizeType _read_size, 
    SizeType* _actual_read_size_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        IsOpen() == false,
        error_code::kZFileErrorCode_NoFileOpened,
        "No file opened!"
    );

    if (_buffer.GetSize() == 0ULL) {
        return ret_val;
    }

    link_code = FlushP();
    if (link_code != kOK) {
        ret_val = error_code::kZFileErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZFile::FlushP() link error!"
        );
        return ret_val;
    }

    if (_read_size > _buffer.GetSize()) {
        _read_size = _buffer.GetSize();
    }

    try {
        SizeType actual_read_size = file_.read(_buffer.GetDataPtr<Void>(), _read_size);
        if (_actual_read_size_ptr != nullptr) {
            *_actual_read_size_ptr = actual_read_size;
        }
    }
    catch (const std::system_error& error) {
        ret_val = error_code::kZFileErrorCode_ReadFailed;
        Z_LOG_ERROR(
            ret_val, 0,
            "Flush failed, {}! error_code: {}",
            error.what(), error.code().value()
        );
    }
    
    return ret_val;
}

NODISCARD ReturnType ZFile::Write(ZConstBuffer _buffer) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        IsOpen() == false,
        error_code::kZFileErrorCode_NoFileOpened,
        "No file opened!"
    );

    if (_buffer.GetSize() == 0ULL) {
        return ret_val;
    }

    if (_buffer.GetSize() + output_buffer_data_size_ >= output_buffer_.GetSize()) {
        link_code = FlushP();
        if (link_code != kOK) {
            ret_val = error_code::kZFileErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZFile::FlushP() link error!"
            );
            return ret_val;
        }
    }

    if (_buffer.GetSize() >= output_buffer_.GetSize()) {
        link_code = WriteP(_buffer);
        if (link_code != kOK) {
            ret_val = error_code::kZFileErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZFile::WriteP() link error!"
            );
            return ret_val;
        }
    }
    else {
        WriteBufferP(_buffer);
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Write(ZStringView _format, fmt::format_args _args) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        IsOpen() == false,
        error_code::kZFileErrorCode_NoFileOpened,
        "No file opened!"
    );

    SizeType size;
    auto result = fmt::vformat_to_n(&size, 0, _format.STDStringView(), _args);
    size = result.size;

    if (size + output_buffer_data_size_ >= output_buffer_.GetSize()) {
        link_code = FlushP();
        if (link_code != kOK) {
            ret_val = error_code::kZFileErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZFile::FlushP() link error!"
            );
            return ret_val;
        }
    }

    if (size >= output_buffer_.GetSize()) {
        ZMemory buffer(size);
        fmt::vformat_to(
            buffer.GetDataPtr<Char>(),
            _format.STDStringView(),
            _args
        );
        link_code = WriteP(ZConstBuffer(buffer.GetDataPtr<Void>(), size));
        if (link_code != kOK) {
            ret_val = error_code::kZFileErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code,
                "ZFile::WriteP() link error!"
            );
            return ret_val;
        }
    }
    else {
        WriteBufferP(_format, _args, size);
    }

    return ret_val;
}

NODISCARD ReturnType ZFile::Flush() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        IsOpen() == false,
        error_code::kZFileErrorCode_NoFileOpened,
        "No file opened!"
    );

    link_code = FlushP();
    if (link_code != kOK) {
        ret_val = error_code::kZFileErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZFile::FlushP() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD Int64 ZFile::Seek(Int64 _offset, SeekType_ _seek_type) noexcept {
    ReturnType link_code = kOK;
    link_code = FlushP();
    if (link_code != kOK) {
        Z_LOG_ERROR(
            error_code::kZFileErrorCode_LinkError, link_code,
            "ZFile::WriteP() link error!"
        );
        return kFailed;
    }
#ifdef _WIN32
    return _lseeki64(file_.descriptor(), _offset, static_cast<Int32>(_seek_type));
#else
    return lseek(file_.descriptor(), _offset, static_cast<Int32>(_seek_type));
#endif
}

NODISCARD Int64 ZFile::Tell() noexcept {
#ifdef _WIN32
    Int64 pos = _lseeki64(file_.descriptor(), 0LL, SEEK_CUR);
#else
    Int64 pos = lseek(file_.descriptor(), 0LL, SEEK_CUR);
#endif
    if (pos != kFailed) {
        pos += static_cast<Int64>(output_buffer_data_size_);
    }
    return pos;
}

NODISCARD ReturnType ZFile::FlushP() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (output_buffer_data_size_ == 0ULL || IsOpen() == false) {
        return ret_val;
    }

    link_code = WriteP(ZConstBuffer(output_buffer_.GetDataPtr<Void>(), output_buffer_data_size_));
    if (link_code != kOK) {
        ret_val = error_code::kZFileErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZFile::WriteP() link error!"
        );
        return ret_val;
    }

    output_buffer_data_size_ = 0ULL;

    return ret_val;
}

NODISCARD ReturnType ZFile::WriteP(ZConstBuffer _buffer) noexcept {
    ReturnType ret_val = kOK;

    try {
        file_.write(_buffer.GetDataPtr<Void>(), _buffer.GetSize());
    }
    catch (const std::system_error& error) {
        ret_val = error_code::kZFileErrorCode_WriteFailed;
        Z_LOG_ERROR(
            ret_val, 0,
            "Write failed, {}! error_code: {}",
            error.what(), error.code().value()
        );
        return ret_val;
    }

    return ret_val;
}

Void ZFile::WriteBufferP(ZConstBuffer _buffer) noexcept {
    memcpy(&output_buffer_[output_buffer_data_size_], _buffer.GetDataPtr<Void>(), _buffer.GetSize());
    output_buffer_data_size_ += _buffer.GetSize();
}

Void ZFile::WriteBufferP(ZStringView _format, fmt::format_args _args, SizeType _data_size) noexcept {
    fmt::vformat_to(
        &output_buffer_[output_buffer_data_size_],
        _format.STDStringView(),
        _args
    );
    output_buffer_data_size_ += _data_size;
}

}//zengine