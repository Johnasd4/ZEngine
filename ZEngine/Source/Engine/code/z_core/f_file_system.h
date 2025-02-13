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
#ifndef Z_CORE_F_FILE_SYSTEM_H_
#define Z_CORE_F_FILE_SYSTEM_H_

#include "internal/z_drive.h"

#include <filesystem>

#include "t_list.h"
#include "z_string.h"

namespace zengine {

namespace error_code {

enum FFileSystemErrorCode : ReturnType {
    kFFileSystemErrorCodeLinkError = kErrorCodeBaseFFileSystem,
    kFFileSystemErrorCodeSystemError,
    kFFileSystemErrorCodeFileDeleteFailed,
    kFFileSystemErrorCodePathNotExist,
    kFFileSystemErrorCodePathNotDirectory,
    kFFileSystemErrorCodeDirectoryDeleteFailed,
    kFFileSystemErrorCodeCreateDirectoryFailed
};

}//error_code

/*
    The namespace contains the function that controls files.
*/
namespace file_system {

/*
    The info of the files, contains: path, name, extension, directory.
*/
struct ZFileInfo {
    ZWString path_;
    ZWString name_;
    ZWString extension_;
    ZWString directory_;
};

/*
    Delete file by the given path.
*/
CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const WChar* _path_dir) noexcept;

/*
    Rename file by the given path.
*/
CORE_DLLAPI NODISCARD ReturnType RenameFileByPath(const WChar* _old_path_dir, const WChar* _new_path_dir) noexcept;

/*
    Create directory by the given path.
*/
CORE_DLLAPI NODISCARD ReturnType CreateDirectoryByPath(const WChar* _path_dir) noexcept;

/*
    Delete directory by the given path.
*/
CORE_DLLAPI NODISCARD ReturnType DeleteDirectoryByPath(const WChar* _path_dir) noexcept;

/*
    Checks if the path exists.
*/
CORE_DLLAPI NODISCARD Bool PathExist(const WChar* _path_dir) noexcept;

/*
    Get all the files under the given path. 
    Pushs the file names into the given list.
*/
CORE_DLLAPI NODISCARD ReturnType GetFilesByPath(const WChar* _path_dir, TList<ZWString>* file_list_ptr) noexcept;

/*
    Get all the directories under the given path. 
    Pushs the file names into the given list.
*/
CORE_DLLAPI NODISCARD ReturnType GetDirectoriesByPath(const WChar* _path_dir, TList<ZWString>* file_list_ptr) noexcept;

/*
    Get all the files and directories under the given path. 
    Pushs the file names into the given list.
*/
CORE_DLLAPI NODISCARD ReturnType GetFilesAndDirectoriesByPath(
    const WChar* _path_dir, TList<ZWString>* file_list_ptr
) noexcept;

/*
    Get all the files under the given path, even the file is in a deeper folder. 
    Pushs the file names into the given list.
*/
CORE_DLLAPI NODISCARD ReturnType GetFileTreeByPath(const WChar* _path_dir, TList<ZWString>* file_list_ptr) noexcept;

/*
    Get the files info by the given path list.
*/
CORE_DLLAPI NODISCARD ReturnType GetFileInfoListByPathList(
    const TList<ZWString>* file_list_ptr, TList<ZFileInfo>* file_info_list_ptr
) noexcept;

}//file_system
}//zengine

#endif // !Z_CORE_F_FILE_SYSTEM_H_
