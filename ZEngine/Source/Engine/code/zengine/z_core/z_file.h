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

#include "internal/l_fmt.h"

#include "t_smart_pointer.h"
#include "z_buffer.h"
#include "z_memory.h"
#include "z_object.h"
#include "z_string_view.h"

namespace zengine {
namespace error_code {

/** @brief Enumeration of file-related error codes. */
enum ZFileErrorCodeEnum : ReturnType {
    /** @brief Base error code for file operations. */
    kZFileErrorCode_LinkError = kErrorCodeBase_ZFile,
    /** @brief System level error occurred. */
    kZFileErrorCode_SystemOrLibraryError,
    /** @brief A nullptr parameter was passed. */
    kZFileErrorCode_NullptrParam,
    /** @brief A parameter was out of range. */
    kZFileErrorCode_ParamOutOfRange,
    /** @brief Failed to open the file. */
    kZFileErrorCode_OpenFileFailed,
    /** @brief No file is currently opened. */
    kZFileErrorCode_NoFileOpened,
    /** @brief Another file is already opened by this object. */
    kZFileErrorCode_OtherFileOpened,
    /** @brief Failed to read from the file. */
    kZFileErrorCode_ReadFailed,
    /** @brief Failed to write to the file. */
    kZFileErrorCode_WriteFailed,
    /** @brief Failed to retrieve the file size. */
    kZFileErrorCode_GetSizeFailed
};
}//error_code
}//zengine

namespace zengine {
namespace internal {

/** @brief Forward declaration of internal file data structure. */
struct ZFileData;

}//internal
}//zengine

namespace zengine {

/** @brief File class for handling file I/O operations. */
class CORE_DLLAPI ZFile : public ZObject<> {
public:
    /** @brief Constant representing a failed seek operation. */
    static constexpr Int64 kFailed = -1LL;

    /** @brief Enumeration of file open flags. */
    enum OpenFlag_ {
        /** @brief Open for reading only. */
        kReadOnly = fmt::file::RDONLY,
        /** @brief Open for writing only. */
        kWriteOnly = fmt::file::WRONLY,
        /** @brief Open for reading and writing. */
        kReadWrite = fmt::file::RDWR,
        /** @brief Create if the file doesn't exist. */
        kCreate = fmt::file::CREATE,
        /** @brief Open in append mode. */
        kAppend = fmt::file::APPEND,
        /** @brief Truncate the content of the file. */
        kTruncate = fmt::file::TRUNC
    };

    /** @brief Enumeration of seek types. */
    enum class SeekType_ : Int32 {
        /** @brief Seek from the beginning of the file. */
        kFileHead = SEEK_SET,
        /** @brief Seek from the current file position. */
        kCurrntPtr = SEEK_CUR,
        /** @brief Seek from the end of the file. */
        kFileEnd = SEEK_END
    };

    /** @brief Default constructor. */
    ZFile() noexcept;

    /** @brief Move constructor. */
    ZFile(ZFile&& _file) noexcept;

    /** @brief Destructor. */
    ~ZFile() noexcept;

    /** @brief Move assignment operator. */
    ZFile& operator=(ZFile&& _file) noexcept;

    /** 
     * @brief Checks if a file is currently open.
     * @return True if the file is open, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsOpen() noexcept { return file_.descriptor() != -1; }

    /**
     * @brief Computes the total size of the file.
     * @return The size of the file in bytes.
     */
    NODISCARD Int64 ComputeSize() noexcept;

    /** 
     * @brief Opens a file with the specified path and flags.
     * @param _file_dir The path to the file.
     * @param _open_flag The flags to use when opening the file (see OpenFlag_).
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType Open(const Char* _file_dir, Int32 _open_flag) noexcept;

    /** 
     * @brief Opens a file safely, ensuring path validity and access rights.
     * @param _file_dir The path to the file.
     * @param _open_flag The flags to use when opening the file (see OpenFlag_).
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType OpenSafe(const Char* _file_dir, Int32 _open_flag) noexcept;

    /** @brief Closes the currently open file. */
    Void Close() noexcept;

    /** 
     * @brief Sets the size of the output buffer.
     * @param _buffer_size The new size of the buffer in bytes.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType SetOutputBufferSize(
        SizeType _buffer_size
    ) noexcept;

    /** 
     * @brief Reads data from the file into a buffer.
     * @param _buffer The buffer to read data into.
     * @param _read_size The number of bytes to read.
     * @param _actual_read_size_ptr Pointer to store the actual number of bytes read (optional).
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType Read(
        ZBuffer _buffer, 
        SizeType _read_size, 
        SizeType* _actual_read_size_ptr = nullptr
    ) noexcept;

    /** 
     * @brief Writes data from a buffer to the file.
     * @param _buffer The buffer containing data to write.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType Write(ZConstBuffer _buffer) noexcept;

    /** 
     * @brief Writes formatted data to the file using a format string and arguments.
     * @param _format The format string.
     * @param _args The arguments for formatting.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType Write(ZStringView _format, fmt::format_args _args) noexcept;

    /** 
     * @brief Prints formatted data to the file.
     * @tparam _ArgsType The types of the arguments.
     * @param _format The format string.
     * @param _args The arguments to format.
     * @return kOK if successful, otherwise an error code.
     */
    template<typename... _ArgsType>
    NODISCARD FORCEINLINE ReturnType Print(ZStringView _format, _ArgsType... _args) noexcept {
        return Write(_format, fmt::make_format_args(_args...));
    }

    /** 
     * @brief Flushes the output buffer to the file.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType Flush() noexcept;

    /** 
     * @brief Moves the file pointer to a specific location.
     * @param _offset The offset to move the pointer by.
     * @param _seek_type The reference point for the offset (see SeekType_).
     * @return The new file position, or kFailed on error.
     */
    NODISCARD Int64 Seek(Int64 _offset, SeekType_ _seek_type) noexcept;

    /** 
     * @brief Gets the current file pointer position.
     * @return The current file position.
     */
    NODISCARD Int64 Tell() noexcept;

protected:
    /** @brief Type alias for the base class. */
    using SuperType_ = ZObject;

private:
    /** @brief Deleted copy constructor to prevent copying. */
    ZFile(const ZFile&) = delete;

    /** @brief Deleted copy assignment operator to prevent copying. */
    ZFile& operator=(const ZFile&) = delete;

    /** 
     * @brief Internal method to flush the buffer.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType FlushP() noexcept;

    /** 
     * @brief Internal method to write data directly to the file.
     * @param _buffer The buffer to write.
     * @return kOK if successful, otherwise an error code.
     */
    NODISCARD ReturnType WriteP(ZConstBuffer _buffer) noexcept;

    /** 
     * @brief Internal method to write data to the output buffer.
     * @param _buffer The buffer to copy data from.
     */
    Void WriteBufferP(ZConstBuffer _buffer) noexcept;

    /** 
     * @brief Internal method to write formatted data to the output buffer.
     * @param _format The format string.
     * @param _args The arguments for formatting.
     * @param _data_size The size of the formatted data.
     */
    Void WriteBufferP(ZStringView _format, fmt::format_args _args, SizeType _data_size) noexcept;

    /** @brief The underlying file object. */
    fmt::file file_;
    /** @brief Current size of data in the output buffer. */
    SizeType output_buffer_data_size_;
    /** @brief Buffer for output operations. */
    ZMemory output_buffer_;
};

}//zengine