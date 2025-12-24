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
#include "z_string.h"
#include "z_string_view.h"

namespace zengine {
namespace error_code {

/**
 * @brief Enumeration for file system related error codes.
 */
enum FFileSystemErrorCodeEnum : ReturnType {
    /** @brief Base error code for file system linking errors. */
    kFFileSystemErrorCode_LinkError = kErrorCodeBase_FFileSystem,
    /** @brief System level error (e.g., from underlying OS or library). */
    kFFileSystemErrorCode_SystemOrLibraryError,
    /** @brief Error indicating a null pointer parameter was passed. */
    kFFileSystemErrorCode_NullptrParam,
    /** @brief Error indicating a parameter value is out of the valid range. */
    kFFileSystemErrorCode_ParamOutOfRange,
};

}//error_code
}//zengine
namespace zengine {
namespace file_system {

/**
 * @brief The info of the files, contains: path, name, extension, directory.
 */
struct ZFileInfo {
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
 * @example { "Text Files", "*.txt" }, { "All Files", "*.*" }
 */
struct ZFileFilter {
    /** @brief The display name of the filter (e.g., "Text Files"). */
    ZStringView hint_;
    /** @brief The extension pattern (e.g., "*.txt"). */
    ZStringView extension_;
};

/**
 * @brief Checks if the path exists.
 * @param _path_dir The path directory string to check.
 * @return True if the path exists, false otherwise.
 */
CORE_DLLAPI NODISCARD Bool IsPathExist(const Char* _path_dir) noexcept;

/**
 * @brief Returns the program's current working directory path.
 * @return The current working directory path string.
 */
CORE_DLLAPI NODISCARD ZString GetExecuteDirectoryPath() noexcept;

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
 * @param _file_dir The file path to parse.
 * @return ZFileInfo structure containing parsed path components.
 */
CORE_DLLAPI NODISCARD ZFileInfo GetFileInfoByPath(ZStringView _file_dir) noexcept;

/**
 * @brief Create a file by the given path.
 * @param _path_dir The full path where the file should be created.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType CreateFileByPath(const Char* _path_dir) noexcept;

/**
 * @brief Delete a file by the given path.
 * @param _path_dir The full path of the file to delete.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const Char* _path_dir) noexcept;

/**
 * @brief Rename file by the given path.
 * @param _old_path_dir The current path of the file.
 * @param _new_path_dir The new path (name) for the file.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType RenameFileByPath(
    const Char* _old_path_dir,
    const Char* _new_path_dir
) noexcept;

/**
 * @brief Copy file by the given path.
 * @param _source_path_dir The source file path.
 * @param _target_path_dir The destination file path.
 * @param overwrite_exist Whether to overwrite the destination file if it exists.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType CopyFileByPath(
    ZStringView _source_path_dir,
    ZStringView _target_path_dir,
    Bool overwrite_exist = true
) noexcept;

/**
 * @brief Create directory by the given path (recursively).
 * @param _path_dir The directory path to create.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType CreateDirectoryByPath(ZStringView _path_dir) noexcept;

/**
 * @brief Delete directory by the given path (recursively).
 * @param _path_dir The directory path to delete.
 */
CORE_DLLAPI Void DeleteDirectoryByPath(ZStringView _path_dir) noexcept;

/**
 * @brief Get all the files under the given path.
 * @details Pushes the file names into the given list.
 * @param _path_dir The directory path to scan.
 * @param _file_list_ptr Pointer to the list to store file paths.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType GetFilesByPath(
    ZStringView _path_dir,
    TList<ZString>* _file_list_ptr
) noexcept;

/**
 * @brief Get all the directories under the given path.
 * @details Pushes the directory names into the given list.
 * @param _path_dir The directory path to scan.
 * @param _file_list_ptr Pointer to the list to store directory paths.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType GetDirectoriesByPath(
    ZStringView _path_dir,
    TList<ZString>* _file_list_ptr
) noexcept;

/**
 * @brief Get all the files and directories under the given path.
 * @details Pushes the names into the given list.
 * @param _path_dir The directory path to scan.
 * @param _file_list_ptr Pointer to the list to store paths.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType GetFilesAndDirectoriesByPath(
    ZStringView _path_dir,
    TList<ZString>* _file_list_ptr
) noexcept;

/**
 * @brief Get all the files under the given path, even the file is in a deeper folder.
 * @details Recursively scans the directory tree and pushes file paths into the given list.
 * @param _path_dir The root directory path to scan.
 * @param _file_list_ptr Pointer to the list to store file paths.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType GetFileTreeByPath(
    ZStringView _path_dir,
    TList<ZString>* _file_list_ptr
) noexcept;

/**
 * @brief Get the files info by the given path list.
 * @param _file_list A list of file paths.
 * @param _file_info_list_ptr Pointer to the list to store the resulting ZFileInfo structures.
 * @return kOK if successful, or an error code if failed.
 */
CORE_DLLAPI NODISCARD ReturnType GetFileInfoListByPathList(
    const TList<ZString>& _file_list,
    TList<ZFileInfo>* _file_info_list_ptr
) noexcept;

}//file_system
}//zengine