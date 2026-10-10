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

#include "f_file_system.h"

#include "f_string.h"
#include "m_log.h"
#include "t_array.h"
#include "t_fixed_memory.h"

namespace zengine {
namespace file_system {
namespace internal {

//FORCEINLINE static uv_fs_t* RequestHandleInstancePtr() noexcept {
//    thread_local uv_fs_t request_handle;
//    return &request_handle;
//}

FORCEINLINE static Bool IsSeparator(Char _char) noexcept {
    return _char == '/' || _char == '\\';
}

} // internal
} // file_system
} // zengine

namespace zengine {
namespace file_system {

ZFileInfo::ZFileInfo() noexcept
    : SuperType_()
    , path_()
    , name_()
    , extension_()
    , directory_()
{}

ZFileInfo::ZFileInfo(const ZFileInfo& _file_info) noexcept
    : SuperType_()
    , path_(_file_info.path_)
{
    SizeType name_start_index = _file_info.name_.GetDataPtr() - _file_info.path_.GetDataPtr();
    name_ = ZStringView(path_.GetDataPtr() + name_start_index, _file_info.name_.GetSize());
    SizeType extension_start_index = _file_info.extension_.GetDataPtr() - _file_info.path_.GetDataPtr();
    extension_ = ZStringView(path_.GetDataPtr() + extension_start_index, _file_info.extension_.GetSize());
    SizeType directory_start_index = _file_info.directory_.GetDataPtr() - _file_info.path_.GetDataPtr();
    directory_ = ZStringView(path_.GetDataPtr() + directory_start_index, _file_info.directory_.GetSize());
}

ZFileInfo::ZFileInfo(ZFileInfo&& _file_info) noexcept 
    : SuperType_()
{
    SizeType name_start_index = _file_info.name_.GetDataPtr() - _file_info.path_.GetDataPtr();
    SizeType extension_start_index = _file_info.extension_.GetDataPtr() - _file_info.path_.GetDataPtr();
    SizeType directory_start_index = _file_info.directory_.GetDataPtr() - _file_info.path_.GetDataPtr();
    path_ = std::move(_file_info.path_);
    name_ = ZStringView(path_.GetDataPtr() + name_start_index, _file_info.name_.GetSize());
    extension_ = ZStringView(path_.GetDataPtr() + extension_start_index, _file_info.extension_.GetSize());
    directory_ = ZStringView(path_.GetDataPtr() + directory_start_index, _file_info.directory_.GetSize());
}

ZFileInfo& ZFileInfo::operator=(const ZFileInfo& _file_info) noexcept {
    if (this == &_file_info) {
        return *this;
    }
    path_ = _file_info.path_;
    SizeType name_start_index = _file_info.name_.GetDataPtr() - _file_info.path_.GetDataPtr();
    name_ = ZStringView(path_.GetDataPtr() + name_start_index, _file_info.name_.GetSize());
    SizeType extension_start_index = _file_info.extension_.GetDataPtr() - _file_info.path_.GetDataPtr();
    extension_ = ZStringView(path_.GetDataPtr() + extension_start_index, _file_info.extension_.GetSize());
    SizeType directory_start_index = _file_info.directory_.GetDataPtr() - _file_info.path_.GetDataPtr();
    directory_ = ZStringView(path_.GetDataPtr() + directory_start_index, _file_info.directory_.GetSize());
    return *this;
}

ZFileInfo& ZFileInfo::operator=(ZFileInfo&& _file_info) noexcept {
    if (this == &_file_info) {
        return *this;
    }
    SizeType name_start_index = _file_info.name_.GetDataPtr() - _file_info.path_.GetDataPtr();
    SizeType extension_start_index = _file_info.extension_.GetDataPtr() - _file_info.path_.GetDataPtr();
    SizeType directory_start_index = _file_info.directory_.GetDataPtr() - _file_info.path_.GetDataPtr();
    path_ = std::move(_file_info.path_);
    name_ = ZStringView(path_.GetDataPtr() + name_start_index, _file_info.name_.GetSize());
    extension_ = ZStringView(path_.GetDataPtr() + extension_start_index, _file_info.extension_.GetSize());
    directory_ = ZStringView(path_.GetDataPtr() + directory_start_index, _file_info.directory_.GetSize());
    return *this;
}

} // file_system
} // zengine

namespace zengine {
namespace file_system {

CORE_DLLAPI NODISCARD Bool IsPathExist(const Char* _path) noexcept {
    uv_fs_t request_handle;
    Int32 ret_val = uv_fs_access(NULL, &request_handle, _path, F_OK, NULL);
    uv_fs_req_cleanup(&request_handle);
    return ret_val == 0;
}

CORE_DLLAPI NODISCARD ReturnType GetExecuteDirectoryPath(Char* _str_buf, SizeType* _io_size_ptr) noexcept {
    ReturnType ret_val = kOK;
    
    Int32 handle_and_error = uv_cwd(_str_buf, _io_size_ptr);
    if (handle_and_error < 0) {
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! error_msg: {}",
            uv_strerror(handle_and_error)
        );
    }

    return ret_val;
}

CORE_DLLAPI NODISCARD ZStringView GetExecutablePath() noexcept {
    static ZStringView path = std::invoke([]() -> ZStringView {
        static ZString path_str;
        // get length
        Int32 length = wai_getExecutablePath(NULL, 0, NULL);
        if (length <= 0) {
            return path_str;
        }

        // get path
        TFixedMemory<kMaxFileDirLength> buf;
        wai_getExecutablePath(buf.GetDataPtr<Char>(), length, NULL);

        path_str.Assign(buf.GetDataPtr<Char>(), length);

        return path_str;
    });
    return path;
}

CORE_DLLAPI NODISCARD ZStringView GetExecutableDirectoryPath() noexcept {
    static ZStringView directory_path = std::invoke([]() -> ZStringView {
        // get length
        Int32 length;
        wai_getExecutablePath(NULL, 0, &length);

        return ZStringView(GetExecutablePath().GetDataPtr(), length);
    });
    return directory_path;
}

CORE_DLLAPI NODISCARD ZFileInfo GetProgramInfo() noexcept {
    return GetFileInfoByPath(GetExecutablePath());
}

CORE_DLLAPI NODISCARD ZFileInfo GetFileInfoByPath(ZStringView _file_path) noexcept {
    ZFileInfo file_info;

    // Assign the full path to the owning string
    file_info.path_ = _file_path;

    // Empty path check
    if (_file_path.IsEmpty()) {
        return file_info;
    }

    // Parse Offsets
    SizeType name_start_index = 0ULL;
    SizeType extension_start_index = _file_path.GetSize();
    Bool if_separator_found = false;

    // Find last separator to determine name start
    for (SizeType index = _file_path.GetSize(); index > 0; --index) {
        if (internal::IsSeparator(_file_path[index - 1])) {
            name_start_index = index;
            if_separator_found = true;
            break;
        }
    }

    // Set Directory View
    if (if_separator_found) {
        SizeType dir_len = name_start_index - 1;
        
        // Edge Case: Root Directory (e.g., "/file.txt" or "C:/file.txt")
        // If the separator is at index 0 (Unix root) or follows a drive letter, 
        // we shouldn't strip it if it results in an empty string implies current dir.
        // For "/file.txt": separator at 0, name_start at 1. dir_len becomes 0.
        // We force it to 1 to keep the "/"
        if (dir_len == 0 && internal::IsSeparator(_file_path[0])) {
            dir_len = 1; 
        }
        
        file_info.directory_ = ZStringView(_file_path.GetDataPtr(), dir_len);
    } 
    else {
        // No separator found, meaning the file is in the current working directory.
        // Directory view remains empty (or you can return "./" if you prefer).
        file_info.directory_ = ZStringView();
    }

    // Set Filename View
    SizeType name_len = _file_path.GetSize() - name_start_index;
    file_info.name_ = ZStringView(&_file_path[name_start_index], name_len);

    // Set Extension View
    // Logic: Find the LAST dot in the filename.
    // Rule: The dot cannot be the FIRST character of the filename (e.g., ".gitignore" has no extension).
    if (name_len > 0) {
        SizeType dot_index = 0;
        Bool if_dot_found = false;

        // Scan the filename part backwards
        for (SizeType index = _file_path.GetSize(); index > name_start_index; --index) {
            if (_file_path[index - 1] == '.') {
                dot_index = index - 1;
                if_dot_found = true;
                break;
            }
        }

        // Apply Extension Rules:
        // A dot must be found.
        if (if_dot_found) {
            file_info.extension_ = ZStringView(
                &_file_path[dot_index],
                _file_path.GetSize() - dot_index
            );
        }
        else {
            file_info.extension_ = ZStringView();
        }
    }

    return file_info;
}

CORE_DLLAPI NODISCARD TOptional<TList<ZString>> GetFilesByPath(ZStringView _dir_path) noexcept {
    ReturnType ret_val = kOK;

    ZString dir_path = _dir_path;

    // Scan directory
    uv_fs_t request_handle;
    Int32 lib_link_code = uv_fs_scandir(NULL, &request_handle, _dir_path.GetDataPtr(), 0, NULL);
    if (lib_link_code < 0) {
        uv_fs_req_cleanup(&request_handle);
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! path: {} error_msg: {}",
            _dir_path, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    // Get file list
    TList<ZString> file_list;

    // Iterate entries
    uv_dirent_t directory_entry;
    // Add separator if needed
    if (!internal::IsSeparator(_dir_path[_dir_path.GetSize() - 1ULL])) {
        dir_path += '/';
    }
    while (uv_fs_scandir_next(&request_handle, &directory_entry) != UV_EOF) {
        // Only want files
        if (directory_entry.type == UV_DIRENT_FILE) {
            file_list.EmplaceBack(dir_path + directory_entry.name);
        }
    }
    uv_fs_req_cleanup(&request_handle);

    return file_list;
}

CORE_DLLAPI NODISCARD TOptional<TList<ZString>> GetDirectoriesByPath(ZStringView _dir_path) noexcept {
    ReturnType ret_val = kOK;

    ZString dir_path = _dir_path;

    // Scan directory
    uv_fs_t request_handle;
    Int32 lib_link_code = uv_fs_scandir(NULL, &request_handle, _dir_path.GetDataPtr(), 0, NULL);
    if (lib_link_code < 0) {
        uv_fs_req_cleanup(&request_handle);
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! path: {} error_msg: {}",
            _dir_path, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    // Get file list
    TList<ZString> file_list;

    // Iterate entries
    uv_dirent_t directory_entry;
    // Add separator if needed
    if (!internal::IsSeparator(_dir_path[_dir_path.GetSize() - 1ULL])) {
        dir_path += '/';
    }
    while (uv_fs_scandir_next(&request_handle, &directory_entry) != UV_EOF) {
        // Only want files
        if (directory_entry.type == UV_DIRENT_DIR) {
            file_list.EmplaceBack(dir_path + directory_entry.name);
        }
    }
    uv_fs_req_cleanup(&request_handle);

    return file_list;
}

CORE_DLLAPI NODISCARD TOptional<TList<ZString>> GetFilesAndDirectoriesByPath(ZStringView _dir_path) noexcept {
    ReturnType ret_val = kOK;

    ZString dir_path = _dir_path;

    // Scan directory
    uv_fs_t request_handle;
    Int32 lib_link_code = uv_fs_scandir(NULL, &request_handle, _dir_path.GetDataPtr(), 0, NULL);
    if (lib_link_code < 0) {
        uv_fs_req_cleanup(&request_handle);
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! path: {} error_msg: {}",
            _dir_path, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    // Get file list
    TList<ZString> file_list;

    // Iterate entries
    uv_dirent_t directory_entry;
    // Add separator if needed
    if (!internal::IsSeparator(_dir_path[_dir_path.GetSize() - 1ULL])) {
        dir_path += '/';
    }
    while (uv_fs_scandir_next(&request_handle, &directory_entry) != UV_EOF) {
        file_list.EmplaceBack(dir_path + directory_entry.name);
    }
    uv_fs_req_cleanup(&request_handle);

    return file_list;
}

NODISCARD static ReturnType GetFileTreeByPathRecursive(
    ZStringView _dir_path,
    TList<ZString>& _file_list
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    ZString dir_path = _dir_path;

    uv_fs_t scan_dir_request_handle;
    ReturnType lib_link_code = uv_fs_scandir(NULL, &scan_dir_request_handle, _dir_path.GetDataPtr(), 0, NULL);
    if (lib_link_code < 0) {
        uv_fs_req_cleanup(&scan_dir_request_handle);
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! path: {} error_msg: {}",
            _dir_path, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    // Iterate entries
    uv_dirent_t directory_entry;
    // Add separator if needed
    if (!internal::IsSeparator(_dir_path[_dir_path.GetSize() - 1ULL])) {
        dir_path += '/';
    }
    ZString full_path;
    while (uv_fs_scandir_next(&scan_dir_request_handle, &directory_entry) != UV_EOF) {
        full_path = dir_path + directory_entry.name;

        // if directory, recursive get
        if (directory_entry.type == UV_DIRENT_DIR) {
            link_code = GetFileTreeByPathRecursive(full_path, _file_list);
            if (link_code != kOK) {
                ret_val = link_code;
                break;
            }
        }
        else {
            // add to list
            _file_list.EmplaceBack(full_path);
        }
    }
    uv_fs_req_cleanup(&scan_dir_request_handle);

    return ret_val;
}

CORE_DLLAPI TOptional<TList<ZString>> GetFileTreeByPath(ZStringView _dir_path) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    // Get file list
    TList<ZString> file_list;
    link_code = GetFileTreeByPathRecursive(_dir_path, file_list);
    if (link_code != kOK) {
        return link_code;
    }
    else {
        return file_list;
    }
}

CORE_DLLAPI NODISCARD TOptional<TList<ZFileInfo>> GetFileInfoListByPathList(const TList<ZString>& _file_list) noexcept {
    ReturnType ret_val = kOK;

    TList<ZFileInfo> file_info_list;

    for (auto file_path = _file_list.Begin(); file_path != _file_list.End(); ++file_path) {
        file_info_list.EmplaceBack(GetFileInfoByPath(*file_path));
    }

    return file_info_list;
}

CORE_DLLAPI NODISCARD ReturnType CreateFileByPath(const Char* _file_path) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    // Check if exist
    if (IsPathExist(_file_path)) {
        Z_DEBUG_LOG_MESSAGE("File already exist! path: {}", _file_path);
        return ret_val;
    }

    // Create directory if not exist
    ZStringView file_path(_file_path);
    SizeType dir_len = 0;
    Bool has_dir = false;

    // Find the last separator to determine where the directory ends
    if (file_path.GetSize() > 0) {
        for (SizeType i = file_path.GetSize(); i > 0; --i) {
            if (internal::IsSeparator(file_path[i - 1])) {
                dir_len = i - 1;
                has_dir = true;

                // Edge case: Root path (e.g. "/file.txt" or "C:/file.txt")
                // If the separator is at index 0, or we are at the root drive level,
                // we want to include the separator to keep it valid (e.g. "/" or "C:/")
                if (dir_len == 0) {
                    dir_len = 1; // Keep the '/'
                }
                else if (dir_len == 2 && file_path[1] == ':') {
                    dir_len = 3; // Keep the '/' in 'C:/'
                }
                break;
            }
        }
    }

    if (has_dir) {
        ZStringView dir_path(file_path.GetDataPtr(), dir_len);
        link_code = CreateDirectoryByPath(dir_path);
        if (link_code != kOK) {
            ret_val = error_code::kFFileSystemErrorCode_LinkError;
            Z_DEBUG_LOG_FAILURE("Create file failed! path: {}", _file_path);
            Z_LOG_ERROR(ret_val, link_code, "file_system::CreateDirectoryByPath() link error!");
            return ret_val;
        }
    }

    // Create file (Open with O_CREAT)
    uv_fs_t request_handle;
    Int32 handle_and_error = uv_fs_open(
        NULL, 
        &request_handle,
        _file_path,
        O_WRONLY | O_CREAT | O_TRUNC, 
        0644, 
        NULL
    );
    uv_fs_req_cleanup(&request_handle);

    // Check open result
    if (handle_and_error < 0) {
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_DEBUG_LOG_FAILURE("Create file failed! path: {}", _file_path);
        Z_LOG_ERROR(
            ret_val, 0, 
            "System error! path: {} error_msg: {}",
            _file_path, uv_strerror(handle_and_error)
        );
        return ret_val;
    }

    // Close immediately
    uv_fs_close(NULL, &request_handle, handle_and_error, NULL);
    uv_fs_req_cleanup(&request_handle);

    Z_DEBUG_LOG_SUCCESS("Create file succeed! path: {}", _file_path);
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const Char* _file_path) noexcept {
    ReturnType ret_val = kOK;

    if (!IsPathExist(_file_path)) {
        Z_DEBUG_LOG_MESSAGE("File not exist! path: {}", _file_path);
        return ret_val;
    }

    // Delete file
    uv_fs_t request_handle;
    Int32 lib_link_code = uv_fs_unlink(NULL, &request_handle, _file_path, NULL);
    uv_fs_req_cleanup(&request_handle);
    if (lib_link_code < 0) {
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_DEBUG_LOG_FAILURE("Delete file failed! path: {}", _file_path);
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! path: {} error_msg: {}",
            _file_path, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    Z_DEBUG_LOG_SUCCESS("Deleted file succeed! path: {}", _file_path);
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType RenameFileByPath(
    const Char* _old_file_path,
    const Char* _new_file_path
) noexcept {
    ReturnType ret_val = kOK;

    if (!IsPathExist(_old_file_path)) {
        Z_DEBUG_LOG_MESSAGE("File does not exist! path: {}", _old_file_path);
        return ret_val;
    }

    // Rename file
    uv_fs_t request_handle;
    Int32 lib_link_code = uv_fs_rename(
        NULL, 
        &request_handle, 
        _old_file_path, 
        _new_file_path, 
        NULL
    );
    uv_fs_req_cleanup(&request_handle);

    if (lib_link_code < 0) {
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_DEBUG_LOG_FAILURE("Rename file failed! old_path: {} new_path: {}", _old_file_path, _new_file_path);
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! old_path: {} new_path: {} error_msg: {}",
            _old_file_path, _new_file_path, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    Z_DEBUG_LOG_SUCCESS("Rename file succeed! path: {} -> {}", _old_file_path, _new_file_path);
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType CopyFileByPath(
    const Char* _src_file_path,
    const Char* _dst_file_path,
    Bool overwrite_exist
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (!IsPathExist(_src_file_path)) {
        Z_DEBUG_LOG_FAILURE(
            "Copy file failed! source_path: {} target_path: {}",
            _dst_file_path, _src_file_path
        );
        Z_LOG_ERROR(ret_val, 0, "Copy file failed! Source path not exist! path: {}", _src_file_path);
        return ret_val;
    }

    // Create directory if not exist
    link_code = CreateDirectoryByPath(_dst_file_path);
    if (link_code != kOK) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_DEBUG_LOG_FAILURE(
            "Copy file failed! source_path: {} target_path: {}",
            _dst_file_path, _src_file_path
        );
        Z_LOG_ERROR(ret_val, link_code, "file_system::CreateDirectoryByPath() link error!");
        return ret_val;
    }

    // Copy
    uv_fs_t request_handle;
    Int32 copy_flags = overwrite_exist ? 0 : UV_FS_COPYFILE_EXCL;
    Int32 lib_link_code = uv_fs_copyfile(
        NULL, 
        &request_handle, 
        _src_file_path,
        _dst_file_path,
        copy_flags,
        NULL
    );
    uv_fs_req_cleanup(&request_handle);

    if (lib_link_code < 0) {
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_DEBUG_LOG_FAILURE(
            "Copy file failed! source_path: {} target_path: {}",
            _dst_file_path, _src_file_path
        );
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! source_path: {} target_path: {} error_msg: {}",
            _dst_file_path, _src_file_path, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    Z_DEBUG_LOG_SUCCESS("Copy file succeed! src: {} tgt: {}", _src_file_path, _dst_file_path);
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType CreateDirectoryByPath(ZStringView _dir_path) noexcept {
    ReturnType ret_val = kOK;

    // Make a mutable ref of the path, will restore after use
    ZString dir_path = _dir_path;

    // Remove trailing slash if present
    if (dir_path.GetSize() > 0ULL && internal::IsSeparator(dir_path[dir_path.GetSize() - 1ULL])) {
        dir_path.PopBack();
    }

    // Empty path check
    if (dir_path.GetSize() == 0ULL) {
        Z_DEBUG_LOG_SUCCESS("Create directory succeed! path: {}", _dir_path);
        return ret_val;
    }

    // Iterate to create recursively (mkdir -p)
    uv_fs_t request_handle;
    for (SizeType index = 0; index < dir_path.GetSize(); ++index) {
        Bool is_seperator = internal::IsSeparator(dir_path[index]);
        if (is_seperator || index == dir_path.GetSize() - 1ULL) {
            // Adjust end index for the current level
            Char temp_char = '\0';
            if (is_seperator) {
                // Skip leading
                if (index == 0ULL) {
                    continue;
                }
                // Win32: Skip drive letter (C:/)
                if (index == 2ULL && dir_path[1ULL] == ':') {
                    continue;
                }
                // Temporarily end string
                temp_char = dir_path[index];
                dir_path[index] = '\0';
            }

            // Make directory
            Int32 lib_link_code = uv_fs_mkdir(
                NULL, 
                &request_handle, 
                dir_path.GetDataPtr(),
                0755, 
                NULL
            );
            uv_fs_req_cleanup(&request_handle);

            // Check for errors other than "already exists"
            if (lib_link_code != 0 && lib_link_code != UV_EEXIST) {
                // Restore
                if (temp_char != 0ULL) {
                    dir_path[index] = temp_char;
                }
                ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
                Z_DEBUG_LOG_FAILURE("Create directory failed! path: {}", _dir_path);
                Z_LOG_ERROR(
                    ret_val, 0, 
                    "System error! path: {} error_msg: {}",
                    _dir_path, uv_strerror(lib_link_code)
                );
                return ret_val;
            }

            // Restore
            if (temp_char != 0ULL) {
                dir_path[index] = temp_char;
            }
        }
    }

    Z_DEBUG_LOG_SUCCESS("Create directory succeed! path: {}", _dir_path);
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType DeleteDirectoryByPath(ZStringView _dir_path) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    ZString dir_path = _dir_path;

    uv_fs_t scan_dir_request_handle;
    uv_dirent_t directory_entry;
    ReturnType lib_link_code = uv_fs_scandir(NULL, &scan_dir_request_handle, dir_path.GetDataPtr(), 0, NULL);
    if (lib_link_code < 0) {
        uv_fs_req_cleanup(&scan_dir_request_handle);
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! path: {} error_msg: {}",
            _dir_path, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    // iterate through directory entries
    // Add separator if needed
    if (!internal::IsSeparator(dir_path[dir_path.GetSize() - 1ULL])) {
        dir_path += '/';
    }

    ZString full_path;
    uv_fs_t request_handle;
    while (uv_fs_scandir_next(&scan_dir_request_handle, &directory_entry) != UV_EOF) {
        full_path = dir_path + directory_entry.name;

        // if directory, recursive delete
        if (directory_entry.type == UV_DIRENT_DIR) {
            link_code = DeleteDirectoryByPath(full_path);
            if (ret_val == kOK && link_code != kOK) {
                ret_val = link_code;
            }
        }
        // else delete file
        else {
            lib_link_code = uv_fs_unlink(
                NULL,
                &request_handle,
                full_path.GetDataPtr(),
                NULL
            );
            uv_fs_req_cleanup(&request_handle);
            // log error if delete failed, continue to delete other files
            if (lib_link_code < 0) {
                Z_DEBUG_LOG_FAILURE("Deleted file failed! path: {}", full_path);
                ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
                Z_LOG_ERROR(
                    ret_val, 0,
                    "System error! path: {} error_msg: {}",
                    full_path, uv_strerror(lib_link_code)
                );
            }
            else {
                Z_DEBUG_LOG_SUCCESS("Deleted file succeed! path: {}", full_path);
            }
        }
    }
    uv_fs_req_cleanup(&scan_dir_request_handle);

    // delete the now empty directory
    if (ret_val == kOK) {
        lib_link_code = uv_fs_rmdir(
            NULL,
            &request_handle,
            _dir_path.GetDataPtr(),
            NULL
        );
        uv_fs_req_cleanup(&request_handle);
        if (lib_link_code < 0) {
            Z_DEBUG_LOG_FAILURE("Deleted directory failed! path: {}", _dir_path);
            Z_LOG_ERROR(
                error_code::kFFileSystemErrorCode_SystemOrLibraryError, 0,
                "System error! path: {} error_msg: {}",
                _dir_path, uv_strerror(lib_link_code)
            );
        }
        else {
            Z_DEBUG_LOG_SUCCESS("Deleted directory succeed! path: {}", _dir_path);
        }
    }

    return ret_val;
}

} // file_system
} // zengine
