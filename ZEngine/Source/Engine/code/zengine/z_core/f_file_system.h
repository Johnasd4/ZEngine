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
#pragma once

#include "drive.h"

#include "t_array.h"
#include "t_list.h"
#include "z_string.h"
#include "z_string_view.h"

namespace zengine {
namespace error_code {
enum FFileSystemErrorCodeEnum : ReturnType {
    kFFileSystemErrorCode_LinkError = kErrorCodeBase_FFileSystem,
    kFFileSystemErrorCode_SystemError,
    kFFileSystemErrorCode_NullptrParam,
    kFFileSystemErrorCode_ParamOutOfRange,
    kFFileSystemErrorCode_FileCreateFailed,
    kFFileSystemErrorCode_FileDeleteFailed,
    kFFileSystemErrorCode_PathNotExist,
    kFFileSystemErrorCode_PathNotDirectory,
    kFFileSystemErrorCode_DirectoryDeleteFailed,
    kFFileSystemErrorCode_CreateDirectoryFailed,
    kFFileSystemErrorCode_CopyFileFailed
};
}//error_code
}//zengine

namespace zengine {
namespace file_system {

/*
    The info of the files, contains: path, name, extension, directory.
*/
struct ZFileInfo {
    ZString path_;
    ZString name_;
    ZString extension_;
    ZString directory_;
};

/*
    The file filter struct.
    Exanple:
        { "Text Files", "*.txt" },
        { "All Files", "*.*" }
*/
struct ZFileFilter {
    const Char* hint_;
    const Char* extension_;
};

/*
   Returns the program's path.
*/
CORE_DLLAPI NODISCARD ZStringView ExecutePath() noexcept;

/*
   Returns the program's path.
*/
CORE_DLLAPI NODISCARD ZStringView ExecutablePath() noexcept;

/*
   Returns the program's directory.
*/
CORE_DLLAPI NODISCARD ZStringView ExecutableDirectoryPath() noexcept;

/*
    Get program info.
*/
CORE_DLLAPI NODISCARD ReturnType ProgramInfo(ZFileInfo* _file_info_ptr) noexcept;

/*
    Create file by the given path.
*/
CORE_DLLAPI NODISCARD ReturnType CreateFileByPath(const Char* _path_dir) noexcept;

/*
    Delete file by the given path.
*/
CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const Char* _path_dir) noexcept;

/*
    Rename file by the given path.
*/
CORE_DLLAPI NODISCARD ReturnType RenameFileByPath(const Char* _old_path_dir, const Char* _new_path_dir) noexcept;

/*
    Copy file by the given path.
*/
CORE_DLLAPI NODISCARD ReturnType CopyFileByPath(
    const Char* _source_path_dir, 
    const Char* _target_path_dir,
    Bool overwrite_exist = true
) noexcept;

/*
    Create directory by the given path.
*/
CORE_DLLAPI NODISCARD ReturnType CreateDirectoryByPath(ZStringView _path_dir) noexcept;

/*
    Delete directory by the given path.
*/
CORE_DLLAPI NODISCARD ReturnType DeleteDirectoryByPath(ZStringView _path_dir) noexcept;

/*
    Checks if the path exists.
*/
CORE_DLLAPI NODISCARD Bool PathExist(const Char* _path_dir) noexcept;

/*
    Checks if the path valid.
*/
CORE_DLLAPI NODISCARD Bool PathValid(const Char* _path_dir) noexcept;

/*
    Get all the files under the given path. 
    Pushs the file names into the given list.
*/
CORE_DLLAPI NODISCARD ReturnType GetFilesByPath(const Char* _path_dir, TList<ZString>* _file_list_ptr) noexcept;

/*
    Get all the directories under the given path. 
    Pushs the file names into the given list.
*/
CORE_DLLAPI NODISCARD ReturnType GetDirectoriesByPath(ZStringView _path_dir, TList<ZString>* _file_list_ptr) noexcept;

/*
    Get all the files and directories under the given path. 
    Pushs the file names into the given list.
*/
CORE_DLLAPI NODISCARD ReturnType GetFilesAndDirectoriesByPath(
    const Char* _path_dir, TList<ZString>* _file_list_ptr
) noexcept;

/*
    Get all the files under the given path, even the file is in a deeper folder. 
    Pushs the file names into the given list.
*/
CORE_DLLAPI NODISCARD ReturnType GetFileTreeByPath(const Char* _path_dir, TList<ZString>* _file_list_ptr) noexcept;

/*
    Get the file info by the given path.
*/
CORE_DLLAPI NODISCARD ReturnType GetFileInfoByPath(
    const ZString& _file, ZFileInfo* _file_info_ptr
) noexcept;

/*
    Get the files info by the given path list.
*/
CORE_DLLAPI NODISCARD ReturnType GetFileInfoListByPathList(
    const TList<ZString>& _file_list, TList<ZFileInfo>* _file_info_list_ptr
) noexcept;

/*
    Get file path by the file selector.
*/
CORE_DLLAPI NODISCARD ReturnType GetFileByFileSelector(
    const TArray<ZFileFilter>& _file_filter_array, ZString* _file_ptr
) noexcept;

/*
    Get mutiple file path by the file selector.
*/
CORE_DLLAPI NODISCARD ReturnType GetFilesByFileSelector(
    const TArray<ZFileFilter>& _file_filter_array, TList<ZString>* _file_list_ptr
) noexcept;

/*
    Get folder path by the file selector.
*/
CORE_DLLAPI NODISCARD ReturnType GetFolderByFileSelector(ZString* _folder_ptr) noexcept;

/*
    Get mutiple folder path by the file selector.
*/
CORE_DLLAPI NODISCARD ReturnType GetFoldersByFileSelector(TList<ZString>* _folder_list_ptr) noexcept;

}//file_system
}//zengine