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

#include "t_array.h"
#include "t_list.h"
#include "t_optional.h"
#include "z_string.h"
#include "z_string_view.h"

namespace zengine {
namespace error_code {

/** @brief Enumeration for file system related error codes. */
enum FFileSystemErrorCodeEnum : ReturnType::ValueType {
    /** @brief Base error code for file system linking errors. */
    kFFileSystemErrorCode_LinkError = kErrorCodeBase_FFileSystem,
    /** @brief System level error (e.g., from underlying OS or library). */
    kFFileSystemErrorCode_SystemOrLibraryError,
    /** @brief Error indicating a null pointer parameter was passed. */
    kFFileSystemErrorCode_NullptrParam,
    /** @brief Error indicating a parameter value is out of the valid range. */
    kFFileSystemErrorCode_ParamOutOfRange,
};

} // error_code
} // zengine

namespace zengine {
namespace file_system {

/**
 * @brief The info of the files, contains: path, name, extension, directory.
 * @details Implementation Principle: This class encapsulates file metadata. It stores the full path string deeply
 *          and maintains lightweight string views pointing to the name, extension, and directory components
 *          within that path to minimize memory allocation and copying.
 */
class CORE_DLLAPI ZFileInfo : public ZObject {
public:
    /** @brief Default constructor. */
    ZFileInfo() noexcept;
    /** @brief Copy constructor. */
    ZFileInfo(const ZFileInfo& _file_info) noexcept;
    /** @brief Move constructor. */
    ZFileInfo(ZFileInfo&& _file_info) noexcept;

    /** @brief Copy assignment operator. */
    ZFileInfo& operator=(const ZFileInfo& _file_info) noexcept;
    /** @brief Move assignment operator. */
    ZFileInfo& operator=(ZFileInfo&& _file_info) noexcept;

    /**
     * @brief Get the full path of the file.
     * @return A string view of the full path.
     */
    NODISCARD FORCEINLINE ZStringView GetPath() const noexcept { return path_; }
    /**
     * @brief Get the file name (including extension).
     * @return A string view of the file name.
     */
    NODISCARD FORCEINLINE ZStringView GetName() const noexcept { return name_; }
    /**
     * @brief Get the file extension.
     * @return A string view of the file extension (including the dot).
     */
    NODISCARD FORCEINLINE ZStringView GetExtension() const noexcept { return extension_; }
    /**
     * @brief Get the directory path containing the file.
     * @return A string view of the directory path.
     */
    NODISCARD FORCEINLINE ZStringView GetDirectory() const noexcept { return directory_; }

protected:
    using SuperType_ = ZObject;

    friend CORE_DLLAPI ZFileInfo GetFileInfoByPath(ZStringView _file_path) noexcept;

private:
    /** @brief The full path of the file. */
    ZString path_;
    /** @brief The file name (including extension). */
    ZStringView name_;
    /** @brief The file extension (including the dot). */
    ZStringView extension_;
    /** @brief The directory path containing the file. */
    ZStringView directory_;
};

/**
 * @brief The file filter struct used for file dialogs or filtering.
 * @details Implementation Principle: A simple aggregate structure holding string views for the filter display
 *          name and the wildcard pattern.
 * @example { "Text Files", "*.txt" }, { "All Files", "*.*" }
 */
struct ZFileFilter {
    /** @brief The display name of the filter (e.g., "Text Files"). */
    ZStringView hint_;
    /** @brief The extension pattern (e.g., "*.txt"). */
    ZStringView extension_;
};

} // file_system
} // zengine

namespace zengine {
namespace file_system {

/**
 * @brief Checks if the path exists.
 * @param _path The path directory string to check.
 * @return True if the path exists, false otherwise.
 */
CORE_DLLAPI NODISCARD Bool IsPathExist(const Char* _path) noexcept;

/**
 * @brief Returns the current working directory path.
 * @param _str_buf Pointer to the buffer to store the directory path.
 * @param _io_size_ptr Pointer to the size of the buffer. On return, it contains the actual size used.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType GetExecuteDirectoryPath(Char* _str_buf, SizeType* _io_size_ptr) noexcept;

/**
 * @brief Returns the program's executable file path.
 * @return The absolute path of the executable file.
 */
CORE_DLLAPI NODISCARD ZStringView GetExecutablePath() noexcept;

/**
 * @brief Returns the program's executable directory path.
 * @return The directory path where the executable is located.
 */
CORE_DLLAPI NODISCARD ZStringView GetExecutableDirectoryPath() noexcept;

/**
 * @brief Get the file information of the current running program.
 * @return ZFileInfo structure containing details about the program's executable.
 */
CORE_DLLAPI NODISCARD ZFileInfo GetProgramInfo() noexcept;

/**
 * @brief Get the file info by the given path.
 * @param _file_path The file path to parse.
 * @return ZFileInfo structure containing parsed path components.
 */
CORE_DLLAPI NODISCARD ZFileInfo GetFileInfoByPath(ZStringView _file_path) noexcept;

/**
 * @brief Get all the files under the given path.
 * @param _dir_path The directory path to scan.
 * @return An optional list containing the file paths. kOK if successful (wrapped in optional), or error otherwise.
 */
CORE_DLLAPI NODISCARD TOptional<TList<ZString>> GetFilesByPath(ZStringView _dir_path) noexcept;

/**
 * @brief Get all the directories under the given path.
 * @param _dir_path The directory path to scan.
 * @return An optional list containing the directory paths. kOK if successful (wrapped in optional), or error otherwise.
 */
CORE_DLLAPI NODISCARD TOptional<TList<ZString>> GetDirectoriesByPath(ZStringView _dir_path) noexcept;

/**
 * @brief Get all the files and directories under the given path.
 * @param _dir_path The directory path to scan.
 * @return An optional list containing the paths. kOK if successful (wrapped in optional), or error otherwise.
 */
CORE_DLLAPI NODISCARD TOptional<TList<ZString>> GetFilesAndDirectoriesByPath(ZStringView _dir_path) noexcept;

/**
 * @brief Get all the files under the given path, even the file is in a deeper folder.
 * @param _dir_path The root directory path to scan.
 * @return An optional list containing the file paths. kOK if successful (wrapped in optional), or error otherwise.
 */
CORE_DLLAPI NODISCARD TOptional<TList<ZString>> GetFileTreeByPath(ZStringView _dir_path) noexcept;

/**
 * @brief Get the files info by the given path list.
 * @param _file_list A list of file paths.
 * @return An optional list containing the ZFileInfo structures. kOK if successful (wrapped in optional), or error.
 */
CORE_DLLAPI NODISCARD TOptional<TList<ZFileInfo>> GetFileInfoListByPathList(const TList<ZString>& _file_list) noexcept;

/**
 * @brief Create a file by the given path.
 * @param _file_path The full path where the file should be created.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType CreateFileByPath(const Char* _file_path) noexcept;

/**
 * @brief Delete a file by the given path.
 * @param _file_path The full path of the file to delete.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const Char* _file_path) noexcept;

/**
 * @brief Rename file by the given path.
 * @param _old_file_path The current path of the file.
 * @param _new_file_path The new path (name) for the file.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType RenameFileByPath(
    const Char* _old_file_path,
    const Char* _new_file_path
) noexcept;

/**
 * @brief Copy file by the given path.
 * @param _src_file_path The source file path.
 * @param _dst_file_path The destination file path.
 * @param overwrite_exist Whether to overwrite the destination file if it exists.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType CopyFileByPath(
    ZStringView _src_file_path,
    ZStringView _dst_file_path,
    Bool overwrite_exist = true
) noexcept;

/**
 * @brief Create directory by the given path (recursively).
 * @param _dir_path The directory path to create.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType CreateDirectoryByPath(ZStringView _dir_path) noexcept;

/**
 * @brief Delete directory by the given path (recursively).
 * @param _dir_path The directory path to delete.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType DeleteDirectoryByPath(ZStringView _dir_path) noexcept;

} // file_system
} // zengine