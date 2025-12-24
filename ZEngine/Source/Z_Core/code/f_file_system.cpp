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

FORCEINLINE static uv_fs_t* RequestHandleInstancePtr() noexcept {
    thread_local uv_fs_t request_handle;
    return &request_handle;
}

FORCEINLINE static Bool IsSeparator(Char _char) noexcept {
    return _char == '/' || _char == '\\';
}

}//internal
}//file_system
}//zengine

namespace zengine {
namespace file_system {

CORE_DLLAPI NODISCARD Bool IsPathExist(const Char* _path_dir) noexcept {
    Int32 ret_val = uv_fs_access(NULL, internal::RequestHandleInstancePtr(), _path_dir, F_OK, NULL);
    uv_fs_req_cleanup(internal::RequestHandleInstancePtr());
    return ret_val == 0;
}

CORE_DLLAPI NODISCARD ZString GetExecuteDirectoryPath() noexcept {
    ZString path_str;
    Char buffer[kMaxFileDirLength];
    SizeType size = sizeof(buffer);

    if (uv_cwd(buffer, &size) == 0) {
        path_str.Assign(buffer, size);
    }
    return path_str;
}

CORE_DLLAPI NODISCARD ZStringView GetExecutablePath() noexcept {
    static ZStringView path = std::invoke([]() -> ZStringView {
        static ZString path_str;
        //get length
        Int32 length = wai_getExecutablePath(NULL, 0, NULL);
        if (length <= 0) {
            return path_str;
        }

        //get path
        TFixedMemory<kMaxFileDirLength> buffer;
        wai_getExecutablePath(buffer.GetDataPtr<Char>(), length, NULL);

        path_str.Assign(buffer.GetDataPtr<Char>(), length);

        return path_str;
    });
    return path;
}

CORE_DLLAPI NODISCARD ZStringView GetExecutableDirectoryPath() noexcept {
    static ZStringView directory_dir = std::invoke([]() -> ZStringView {
        //get length
        Int32 length;
        wai_getExecutablePath(NULL, 0, &length);

        return ZStringView(GetExecutablePath().GetDataPtr(), length);
    });
    return directory_dir;
}

CORE_DLLAPI NODISCARD ZFileInfo GetProgramInfo() noexcept {
    return GetFileInfoByPath(GetExecutablePath());
}

CORE_DLLAPI NODISCARD ZFileInfo GetFileInfoByPath(ZStringView _file_dir) noexcept {
    ZFileInfo file_info;

    //Assign the full path to the owning string
    file_info.path_ = _file_dir;

    //Empty path check
    if (_file_dir.IsEmpty()) {
        return file_info;
    }

    //Parse Offsets
    SizeType name_start_index = 0ULL;
    SizeType extension_start_index = _file_dir.GetSize();
    Bool if_separator_found = false;

    //Find last separator to determine name start
    for (SizeType index = _file_dir.GetSize(); index > 0; --index) {
        if (internal::IsSeparator(_file_dir[index - 1])) {
            name_start_index = index;
            if_separator_found = true;
            break;
        }
    }

    //Set Directory View
    if (if_separator_found) {
        SizeType dir_len = name_start_index - 1;
        
        //Edge Case: Root Directory (e.g., "/file.txt" or "C:/file.txt")
        //If the separator is at index 0 (Unix root) or follows a drive letter, 
        //we shouldn't strip it if it results in an empty string implies current dir.
        //For "/file.txt": separator at 0, name_start at 1. dir_len becomes 0.
        //We force it to 1 to keep the "/"
        if (dir_len == 0 && internal::IsSeparator(_file_dir[0])) {
            dir_len = 1; 
        }
        
        file_info.directory_ = ZStringView(_file_dir.GetDataPtr(), dir_len);
    } 
    else {
        //No separator found, meaning the file is in the current working directory.
        //Directory view remains empty (or you can return "./" if you prefer).
        file_info.directory_ = ZStringView();
    }

    //Set Filename View
    SizeType name_len = _file_dir.GetSize() - name_start_index;
    file_info.name_ = ZStringView(&_file_dir[name_start_index], name_len);

    //Set Extension View
    //Logic: Find the LAST dot in the filename.
    //Rule: The dot cannot be the FIRST character of the filename (e.g., ".gitignore" has no extension).
    if (name_len > 0) {
        SizeType dot_index = 0;
        Bool if_dot_found = false;

        // Scan the filename part backwards
        for (SizeType index = _file_dir.GetSize(); index > name_start_index; --index) {
            if (_file_dir[index - 1] == '.') {
                dot_index = index - 1;
                if_dot_found = true;
                break;
            }
        }

        //Apply Extension Rules:
        //A dot must be found.
        if (if_dot_found) {
            file_info.extension_ = ZStringView(
                &_file_dir[dot_index],
                _file_dir.GetSize() - dot_index
            );
        }
        else {
            file_info.extension_ = ZStringView();
        }
    }

    return file_info;
}

CORE_DLLAPI NODISCARD ReturnType CreateFileByPath(const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    //Check if exist
    if (IsPathExist(_path_dir)) {
        Z_DEBUG_LOG_MESSAGE("File already exist! path: {}", _path_dir);
        return ret_val;
    }

    //Create directory if not exist
    link_code = CreateDirectoryByPath(_path_dir);
    if (link_code != kOK) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_DEBUG_LOG_FAILURE("Create file failed! path: {}", _path_dir);
        Z_LOG_ERROR(ret_val, link_code, "file_system::CreateDirectoryByPath() link error!");
        return ret_val;
    }

    //Create file (Open with O_CREAT)
    Int32 handle_and_error = uv_fs_open(
        NULL, 
        internal::RequestHandleInstancePtr(), 
        _path_dir,
        O_WRONLY | O_CREAT | O_TRUNC, 
        0644, 
        NULL
    );
    uv_fs_req_cleanup(internal::RequestHandleInstancePtr());

    //Check open result
    if (handle_and_error < 0) {
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_DEBUG_LOG_FAILURE("Create file failed! path: {}", _path_dir);
        Z_LOG_ERROR(
            ret_val, 0, 
            "System error! path: {} error_msg: {}",
            _path_dir, uv_strerror(handle_and_error)
        );
        return ret_val;
    }

    //Close immediately
    uv_fs_close(NULL, internal::RequestHandleInstancePtr(), handle_and_error, NULL);
    uv_fs_req_cleanup(internal::RequestHandleInstancePtr());

    Z_DEBUG_LOG_SUCCESS("Create file succeed! path: {}", _path_dir);
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;

    if (!IsPathExist(_path_dir)) {
        Z_DEBUG_LOG_MESSAGE("File not exist! path: {}", _path_dir);
        return ret_val;
    }

    //Delete file
    Int32 lib_link_code = uv_fs_unlink(NULL, internal::RequestHandleInstancePtr(), _path_dir, NULL);
    uv_fs_req_cleanup(internal::RequestHandleInstancePtr());
    if (lib_link_code < 0) {
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_DEBUG_LOG_FAILURE("Delete file failed! path: {}", _path_dir);
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! path: {} error_msg: {}",
            _path_dir, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    Z_DEBUG_LOG_SUCCESS("Deleted file succeed! path: {}", _path_dir);
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType RenameFileByPath(
    const Char* _old_path_dir,
    const Char* _new_path_dir
) noexcept {
    ReturnType ret_val = kOK;

    if (!IsPathExist(_old_path_dir)) {
        Z_DEBUG_LOG_MESSAGE("File does not exist! path: {}", _old_path_dir);
        return ret_val;
    }

    //Rename file
    Int32 lib_link_code = uv_fs_rename(
        NULL, 
        internal::RequestHandleInstancePtr(), 
        _old_path_dir, 
        _new_path_dir, 
        NULL
    );
    uv_fs_req_cleanup(internal::RequestHandleInstancePtr());

    if (lib_link_code < 0) {
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_DEBUG_LOG_FAILURE("Rename file failed! old_path: {} new_path: {}", _old_path_dir, _new_path_dir);
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! old_path: {} new_path: {} error_msg: {}",
            _old_path_dir, _new_path_dir, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    Z_DEBUG_LOG_SUCCESS("Rename file succeed! path: {} -> {}", _old_path_dir, _new_path_dir);
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType CopyFileByPath(
    const Char* _source_path_dir,
    const Char* _target_path_dir,
    Bool overwrite_exist
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    if (!IsPathExist(_source_path_dir)) {
        Z_DEBUG_LOG_FAILURE(
            "Copy file failed! source_path: {} target_path: {}",
            _target_path_dir, _source_path_dir
        );
        Z_LOG_ERROR(ret_val, 0, "Copy file failed! Source path not exist! path: {}", _source_path_dir);
        return ret_val;
    }

    //Create directory if not exist
    link_code = CreateDirectoryByPath(_target_path_dir);
    if (link_code != kOK) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_DEBUG_LOG_FAILURE(
            "Copy file failed! source_path: {} target_path: {}",
            _target_path_dir, _source_path_dir
        );
        Z_LOG_ERROR(ret_val, link_code, "file_system::CreateDirectoryByPath() link error!");
        return ret_val;
    }

    //Copy
    Int32 copy_flags = overwrite_exist ? 0 : UV_FS_COPYFILE_EXCL;
    Int32 lib_link_code = uv_fs_copyfile(
        NULL, 
        internal::RequestHandleInstancePtr(), 
        _source_path_dir,
        _target_path_dir,
        copy_flags,
        NULL
    );
    uv_fs_req_cleanup(internal::RequestHandleInstancePtr());

    if (lib_link_code < 0) {
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_DEBUG_LOG_FAILURE(
            "Copy file failed! source_path: {} target_path: {}",
            _target_path_dir, _source_path_dir
        );
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! source_path: {} target_path: {} error_msg: {}",
            _target_path_dir, _source_path_dir, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    Z_DEBUG_LOG_SUCCESS("Copy file succeed! src: {} tgt: {}", _source_path_dir, _target_path_dir);
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType CreateDirectoryByPath(ZStringView _path_dir) noexcept {
    ReturnType ret_val = kOK;

    //Make a mutable ref of the path, will restore after use
    ZString path_dir = _path_dir;

    //Remove trailing slash if present
    if (path_dir.GetSize() > 0ULL && internal::IsSeparator(path_dir[path_dir.GetSize() - 1ULL])) {
        path_dir.PopBack();
    }

    //Empty path check
    if (path_dir.GetSize() == 0ULL) {
        Z_DEBUG_LOG_SUCCESS("Create directory succeed! path: {}", _path_dir);
        return ret_val;
    }

    //Iterate to create recursively (mkdir -p)
    for (SizeType index = 0; index < path_dir.GetSize(); ++index) {
        Bool is_seperator = internal::IsSeparator(path_dir[index]);
        if (is_seperator || index == path_dir.GetSize() - 1ULL) {
            //Adjust end index for the current level
            Char temp_char = '\0';
            if (is_seperator) {
                //Skip leading
                if (index == 0ULL) {
                    continue;
                }
                //Win32: Skip drive letter (C:/)
                if (index == 2ULL && path_dir[1ULL] == ':') {
                    continue;
                }
                //Temporarily end string
                temp_char = path_dir[index];
                path_dir[index] = '\0';
            }

            //Make directory
            Int32 lib_link_code = uv_fs_mkdir(
                NULL, 
                internal::RequestHandleInstancePtr(), 
                path_dir.GetDataPtr(),
                0755, 
                NULL
            );
            uv_fs_req_cleanup(internal::RequestHandleInstancePtr());

            //Check for errors other than "already exists"
            if (lib_link_code != 0 && lib_link_code != UV_EEXIST) {
                //Restore
                if (temp_char != 0ULL) {
                    path_dir[index] = temp_char;
                }
                ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
                Z_DEBUG_LOG_FAILURE("Create directory failed! path: {}", _path_dir);
                Z_LOG_ERROR(
                    ret_val, 0, 
                    "System error! path: {} error_msg: {}",
                    _path_dir, uv_strerror(lib_link_code)
                );
                return ret_val;
            }

            //Restore
            if (temp_char != 0ULL) {
                path_dir[index] = temp_char;
            }
        }
    }

    Z_DEBUG_LOG_SUCCESS("Create directory succeed! path: {}", _path_dir);
    return ret_val;
}

CORE_DLLAPI Void DeleteDirectoryByPath(ZStringView _path_dir) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    ZString path_dir = _path_dir;

    uv_fs_t scan_dir_request_handle;
    uv_dirent_t directory_entry;
    ReturnType lib_link_code = uv_fs_scandir(NULL, &scan_dir_request_handle, path_dir.GetDataPtr(), 0, NULL);
    if (lib_link_code < 0) {
        uv_fs_req_cleanup(&scan_dir_request_handle);
        Z_LOG_ERROR(
            error_code::kFFileSystemErrorCode_SystemOrLibraryError, 0,
            "System error! path: {} error_msg: {}",
            _path_dir, uv_strerror(lib_link_code)
        );
        return;
    }

    //iterate through directory entries
    //Add separator if needed
    if (!internal::IsSeparator(path_dir[path_dir.GetSize() - 1ULL])) {
        path_dir += '/';
    }

    ZString full_path;
    while (uv_fs_scandir_next(&scan_dir_request_handle, &directory_entry) != UV_EOF) {
        full_path = path_dir + directory_entry.name;

        //if directory, recursive delete
        if (directory_entry.type == UV_DIRENT_DIR) {
            DeleteDirectoryByPath(full_path);
        }
        //else delete file
        else {
            lib_link_code = uv_fs_unlink(
                NULL,
                internal::RequestHandleInstancePtr(),
                full_path.GetDataPtr(),
                NULL
            );
            uv_fs_req_cleanup(internal::RequestHandleInstancePtr());
            //log error if delete failed, continue to delete other files
            if (lib_link_code < 0) {
                Z_DEBUG_LOG_FAILURE("Deleted file failed! path: {}", full_path);
                Z_LOG_ERROR(
                    error_code::kFFileSystemErrorCode_SystemOrLibraryError, 0,
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

    //delete the now empty directory
    if (ret_val == kOK) {
        lib_link_code = uv_fs_rmdir(
            NULL,
            internal::RequestHandleInstancePtr(),
            _path_dir.GetDataPtr(),
            NULL
        );
        uv_fs_req_cleanup(internal::RequestHandleInstancePtr());
        if (lib_link_code < 0) {
            Z_DEBUG_LOG_FAILURE("Deleted directory failed! path: {}", _path_dir);
            Z_LOG_ERROR(
                error_code::kFFileSystemErrorCode_SystemOrLibraryError, 0,
                "System error! path: {} error_msg: {}",
                _path_dir, uv_strerror(lib_link_code)
            );
        }
        else {
            Z_DEBUG_LOG_SUCCESS("Deleted directory succeed! path: {}", _path_dir);
        }
    }
}

CORE_DLLAPI NODISCARD ReturnType GetFilesByPath(
    ZStringView _path_dir,
    TList<ZString>* _file_list_ptr
) noexcept {
    ReturnType ret_val = kOK;

    ZString path_dir = _path_dir;

    //Scan directory
    Int32 lib_link_code = uv_fs_scandir(NULL, internal::RequestHandleInstancePtr(), _path_dir.GetDataPtr(), 0, NULL);
    if (lib_link_code < 0) {
        uv_fs_req_cleanup(internal::RequestHandleInstancePtr());
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, 0, 
            "System error! path: {} error_msg: {}", 
            _path_dir, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    //Iterate entries
    uv_dirent_t directory_entry;
    //Add separator if needed
    if (!internal::IsSeparator(_path_dir[_path_dir.GetSize() - 1ULL])) {
        path_dir += '/';
    }
    while (uv_fs_scandir_next(internal::RequestHandleInstancePtr(), &directory_entry) != UV_EOF) {
        //Only want files
        if (directory_entry.type == UV_DIRENT_FILE) {
            _file_list_ptr->EmplaceBack(path_dir + directory_entry.name);
        }
    }
    uv_fs_req_cleanup(internal::RequestHandleInstancePtr());

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetDirectoriesByPath(
    ZStringView _path_dir,
    TList<ZString>* _file_list_ptr
) noexcept {
    ReturnType ret_val = kOK;

    ZString path_dir = _path_dir;

    //Scan directory
    Int32 lib_link_code = uv_fs_scandir(NULL, internal::RequestHandleInstancePtr(), _path_dir.GetDataPtr(), 0, NULL);
    if (lib_link_code < 0) {
        uv_fs_req_cleanup(internal::RequestHandleInstancePtr());
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! path: {} error_msg: {}",
            _path_dir, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    //Iterate entries
    uv_dirent_t directory_entry;
    //Add separator if needed
    if (!internal::IsSeparator(_path_dir[_path_dir.GetSize() - 1ULL])) {
        path_dir += '/';
    }
    while (uv_fs_scandir_next(internal::RequestHandleInstancePtr(), &directory_entry) != UV_EOF) {
        //Only want files
        if (directory_entry.type == UV_DIRENT_DIR) {
            _file_list_ptr->EmplaceBack(path_dir + directory_entry.name);
        }
    }
    uv_fs_req_cleanup(internal::RequestHandleInstancePtr());

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFilesAndDirectoriesByPath(
    ZStringView _path_dir,
    TList<ZString>* _file_list_ptr
) noexcept {
    ReturnType ret_val = kOK;

    ZString path_dir = _path_dir;

    //Scan directory
    Int32 lib_link_code = uv_fs_scandir(NULL, internal::RequestHandleInstancePtr(), _path_dir.GetDataPtr(), 0, NULL);
    if (lib_link_code < 0) {
        uv_fs_req_cleanup(internal::RequestHandleInstancePtr());
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! path: {} error_msg: {}",
            _path_dir, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    //Iterate entries
    uv_dirent_t directory_entry;
    //Add separator if needed
    if (!internal::IsSeparator(_path_dir[_path_dir.GetSize() - 1ULL])) {
        path_dir += '/';
    }
    while (uv_fs_scandir_next(internal::RequestHandleInstancePtr(), &directory_entry) != UV_EOF) {
        _file_list_ptr->EmplaceBack(path_dir + directory_entry.name);
    }
    uv_fs_req_cleanup(internal::RequestHandleInstancePtr());

    return ret_val;
}

CORE_DLLAPI ReturnType GetFileTreeByPath(
    ZStringView _path_dir,
    TList<ZString>* _file_list_ptr
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    ZString path_dir = _path_dir;

    uv_fs_t scan_dir_request_handle;
    ReturnType lib_link_code = uv_fs_scandir(NULL, &scan_dir_request_handle, _path_dir.GetDataPtr(), 0, NULL);
    if (lib_link_code < 0) {
        uv_fs_req_cleanup(&scan_dir_request_handle);
        ret_val = error_code::kFFileSystemErrorCode_SystemOrLibraryError;
        Z_LOG_ERROR(
            ret_val, 0,
            "System error! path: {} error_msg: {}",
            _path_dir, uv_strerror(lib_link_code)
        );
        return ret_val;
    }

    //Iterate entries
    uv_dirent_t directory_entry;
    //Add separator if needed
    if (!internal::IsSeparator(_path_dir[_path_dir.GetSize() - 1ULL])) {
        path_dir += '/';
    }
    ZString full_path;
    while (uv_fs_scandir_next(&scan_dir_request_handle, &directory_entry) != UV_EOF) {
        full_path = path_dir + directory_entry.name;

        //if directory, recursive get
        if (directory_entry.type == UV_DIRENT_DIR) {
            link_code = GetFileTreeByPath(full_path, _file_list_ptr);
            if (link_code != kOK) {
                return link_code;
            }
        }
        else {
            //add to list
            _file_list_ptr->EmplaceBack(full_path);
        }
    }
    uv_fs_req_cleanup(&scan_dir_request_handle);

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFileInfoListByPathList(
    const TList<ZString>& _file_list,
    TList<ZFileInfo>* _file_info_list_ptr
) noexcept {
    ReturnType ret_val = kOK;

    for (auto file_path = _file_list.Begin(); file_path != _file_list.End(); ++file_path) {
        _file_info_list_ptr->EmplaceBack(GetFileInfoByPath(*file_path));
    }

    return ret_val;
}

}//file_system
}//zengine
