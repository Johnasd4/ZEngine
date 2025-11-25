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

#include "f_file_system.h"

#include <fstream>
#include <filesystem>
#include <shobjidl.h>
#include <regex>

#include "f_string.h"
#include "m_log.h"
#include "t_vector.h"

namespace zengine {
namespace file_system {

CORE_DLLAPI NODISCARD ZWString ProgramPath() noexcept {
    return (std::filesystem::current_path() / std::filesystem::path(__argv[0]).filename()).c_str();
}

CORE_DLLAPI NODISCARD ZWString ProgramDirectory() noexcept {
    return std::filesystem::current_path().c_str();
}

CORE_DLLAPI NODISCARD ReturnType ProgramInfo(ZFileInfo* _file_info_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = GetFileInfoByPath(
        ZWString((std::filesystem::current_path() / std::filesystem::path(__argv[0]).filename()).c_str()),
        _file_info_ptr
    );
    if (link_code != kOK) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"file_system::GetFileInfoByPath() link error!");
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType CreateFileByPath(const WChar* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    try {
        if (std::filesystem::exists(_path_dir)) {
            Z_DEBUG_LOG_MESSAGE(L"File already exist! path: %ls", _path_dir);
        }
        else {
            std::filesystem::path path(_path_dir);

            //create directory if not exist
            link_code = CreateDirectoryByPath(path.parent_path().c_str());
            if (link_code != kOK) {
                ret_val = error_code::kFFileSystemErrorCode_LinkError;
                Z_DEBUG_LOG_FAILURE(L"Create file failed! path: %ls", _path_dir);
                Z_LOG_ERROR(ret_val, link_code, L"file_system::CreateDirectoryByPath() link error!");
                return ret_val;
            }

            //create file
            std::ofstream file(_path_dir);
            if (file.is_open()) {
                file.close();
                Z_DEBUG_LOG_SUCCESS(L"Create file succeed! path: %ls", _path_dir);
            }
            else {
                Z_DEBUG_LOG_FAILURE(L"Create file failed! path: %ls", _path_dir);
                ret_val = error_code::kFFileSystemErrorCode_FileCreateFailed;
                Z_LOG_ERROR(ret_val, 0, L"Create file failed! path: %ls", _path_dir);
                return ret_val;
            }
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(ret_val, 0, L"Create file failed! path: %ls", _path_dir);
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls", 
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const WChar* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    try {
        if (!std::filesystem::exists(_path_dir)) {
            Z_DEBUG_LOG_MESSAGE(L"File does not exist! path: %ls", _path_dir);
        }
        else if (std::filesystem::remove(_path_dir)) {
            Z_DEBUG_LOG_SUCCESS(L"Deleted file succeed! path: %ls", _path_dir);
        }
        else {

            ret_val = error_code::kFFileSystemErrorCode_FileDeleteFailed;
            Z_LOG_ERROR(ret_val, 0, L"Deleted file failed! path: %ls", _path_dir);
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_DEBUG_LOG_FAILURE(L"Deleted file failed! path: %ls", _path_dir);
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls", 
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType RenameFileByPath(const WChar* _old_path_dir, const WChar* _new_path_dir) noexcept {
    ReturnType ret_val = kOK;
    try {
        if (!std::filesystem::exists(_old_path_dir)) {
            Z_DEBUG_LOG_MESSAGE(L"File does not exist! path: %ls", _old_path_dir);
            return ret_val;
        }
        std::filesystem::rename(_old_path_dir, _new_path_dir);
        Z_DEBUG_LOG_SUCCESS(L"Rename file succeed! path: %ls -> %ls", _old_path_dir, _new_path_dir);
        return ret_val;
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_DEBUG_LOG_FAILURE(L"Rename file failed! old_path: %ls new_path: %ls", _old_path_dir, _new_path_dir);
        Z_LOG_ERROR(
            ret_val, 0, L"System error! old path: %ls new path: %ls error msg: %ls",
            _old_path_dir, _new_path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType CopyFileByPath(
    const WChar* _source_path_dir,
    const WChar* _target_path_dir,
    Bool overwrite_exist
) noexcept {
    ReturnType ret_val = kOK;
    try {

        //check source file exist
        if (!std::filesystem::exists(_source_path_dir)) {
            ret_val = error_code::kFFileSystemErrorCode_PathNotExist;
            Z_DEBUG_LOG_FAILURE(
                L"Copy file failed! source_path: %ls target_path: %ls",
                _target_path_dir, _source_path_dir
            );
            Z_LOG_ERROR(ret_val, 0, L"Copy file failed! Source path not exist! path: %ls", _source_path_dir);
            return ret_val;
        }

        //generate target directory if not exist
        std::filesystem::path path(_target_path_dir);
        if (!std::filesystem::exists(path.parent_path().c_str())) {
            if (!std::filesystem::create_directories(path.parent_path().c_str())) {
                ret_val = error_code::kFFileSystemErrorCode_PathNotDirectory;
                Z_DEBUG_LOG_FAILURE(
                    L"Copy file failed! source_path: %ls target_path: %ls",
                    _target_path_dir, _source_path_dir
                );
                Z_LOG_ERROR(
                    ret_val, 0,
                    L"Copy file failed! Target directory not valid! path: %ls",
                    path.parent_path().c_str()
                );
                return ret_val;
            }
        }
        //copy file
        auto copy_options = std::filesystem::copy_options::none;
        if (overwrite_exist) {
            copy_options |= std::filesystem::copy_options::overwrite_existing;
        }
        if (!std::filesystem::copy_file(_source_path_dir, _target_path_dir, copy_options)) {
            ret_val = error_code::kFFileSystemErrorCode_CopyFileFailed;
            Z_DEBUG_LOG_FAILURE(
                L"Copy file failed! source_path: %ls target_path: %ls",
                _target_path_dir, _source_path_dir
            );
            Z_LOG_ERROR(
                ret_val, 0, 
                L"Copy file failed! source_path: %ls target_path: %ls", 
                _target_path_dir, _source_path_dir
            );
            return ret_val;
        }
        
        Z_DEBUG_LOG_SUCCESS(
            L"Copy file succeed! source_path: %ls target_path: %ls",
            _target_path_dir, _source_path_dir
        );

        return ret_val;
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_DEBUG_LOG_FAILURE(
            L"Copy file failed! source_path: %ls target_path: %ls",
            _target_path_dir, _source_path_dir
        );
        Z_LOG_ERROR(
            ret_val, 0, L"System error! source_path: %ls target_path: %ls",
            _target_path_dir, _source_path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType CreateDirectoryByPath(const WChar* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    try {
        if (std::filesystem::exists(_path_dir)) {
            Z_DEBUG_LOG_MESSAGE(L"Directory already exist! path: %ls", _path_dir);
        }
        else if (std::filesystem::create_directories(_path_dir)) {
            Z_DEBUG_LOG_SUCCESS(L"Create directory succeed! path: %ls", _path_dir);
        }
        else {
            ret_val = error_code::kFFileSystemErrorCode_CreateDirectoryFailed;
            Z_DEBUG_LOG_FAILURE(L"Create directory failed! path: %ls", _path_dir);
            Z_LOG_ERROR(ret_val, 0, L"Create directory failed! path: %ls", _path_dir);
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_DEBUG_LOG_FAILURE(L"Create directory failed! path: %ls", _path_dir);
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_DEBUG_LOG_FAILURE(L"Create directory failed! path: %ls", _path_dir);
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType DeleteDirectoryByPath(const WChar* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    try {
        if (!std::filesystem::exists(_path_dir)) {
            Z_DEBUG_LOG_MESSAGE(L"Directory not exist! path: %ls", _path_dir);
        }
        else if (std::filesystem::remove_all(_path_dir)) {
            Z_DEBUG_LOG_SUCCESS(L"Deleted directory succeed! path: %ls", _path_dir);
        }
        else {
            ret_val = error_code::kFFileSystemErrorCode_DirectoryDeleteFailed;
            Z_DEBUG_LOG_FAILURE(L"Deleted directory failed! path: %ls", _path_dir);
            Z_LOG_ERROR(ret_val, 0, L"Deleted directory failed! path: %ls", _path_dir);
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_DEBUG_LOG_FAILURE(L"Deleted directory failed! path: %ls", _path_dir);
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFilesByPath(const WChar* _path_dir, TList<ZWString>* _file_list_ptr) noexcept {
    ReturnType ret_val = kOK;
    try {
        Z_CHECK(
            !std::filesystem::exists(_path_dir), error_code::kFFileSystemErrorCode_PathNotExist,
            L"Path not exist! path: %ls", _path_dir);
        Z_CHECK(
            !std::filesystem::is_directory(_path_dir), error_code::kFFileSystemErrorCode_PathNotDirectory,
            L"Path not directory! path: %ls", _path_dir);
        for (const auto& entry : std::filesystem::directory_iterator(_path_dir)) {
            if (std::filesystem::is_regular_file(entry.path())) {
                _file_list_ptr->PushBack(entry.path().c_str());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD Bool PathExist(const WChar* _path_dir) noexcept {
    return std::filesystem::exists(_path_dir);
}

CORE_DLLAPI NODISCARD Bool PathValid(const WChar* _path_dir) noexcept {
    //nullptr check
    if (!_path_dir) {
        return false;
    }

    ZWString path_dir(_path_dir);

    //empty check
    if (path_dir.Empty()) {
        return false;
    }

    //long file dir length check
    if (path_dir.Size() > 32767) {
        return false;
    }

    //normal file dir length check
    if (path_dir.Size() > 260 && path_dir.Find(L"\\\\?\\") != 0 && path_dir.Find(L"\\\\.\\") != 0) {
        return false;
    }

    //illegal char check
    static constexpr WChar illegal_chars[] = L"*?\"<>|";
    for (SizeType index = 0; index < sizeof(illegal_chars) - 1; ++index) {
        if (path_dir.Find(illegal_chars[index]) != ZWString::kFindEnd) {
            return false;
        }
    }

    //format check

    //fullpath start
    if (path_dir.Size() >= 2 && path_dir[1] == L':') {
        if (
            !((path_dir[0] >= L'A' && path_dir[0] <= L'Z') ||
            (path_dir[0] >= L'a' && path_dir[0] <= L'z'))
        ) {
            return false;
        }

        if (path_dir.Size() > 2 && path_dir[2] != L'\\' && path_dir[2] != L'/') {
            return false;
        }
    }

    //path body
    for (SizeType index = 1; index < path_dir.Size(); ++index) {
        if (
            (path_dir[index] == L'\\' && path_dir[index - 1] == L'\\') ||
            (path_dir[index] == L'\\' && path_dir[index - 1] == L'/') ||
            (path_dir[index] == L'/' && path_dir[index - 1] == L'\\') ||
            (path_dir[index] == L'/' && path_dir[index - 1] == L'/')
        ) {
            return false;
        }
    }

    return true;
}

CORE_DLLAPI NODISCARD ReturnType GetDirectoriesByPath(const WChar* _path_dir, TList<ZWString>* _file_list_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    try {
        Z_CHECK(
            !std::filesystem::exists(_path_dir), error_code::kFFileSystemErrorCode_PathNotExist,
            L"Path not exist! path: %ls", _path_dir);
        Z_CHECK(
            !std::filesystem::is_directory(_path_dir), error_code::kFFileSystemErrorCode_PathNotDirectory,
            L"Path not directory! path: %ls", _path_dir);
        for (const auto& file : std::filesystem::directory_iterator(_path_dir)) {
            if (std::filesystem::is_directory(file.path())) {
                _file_list_ptr->PushBack(file.path().c_str());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFilesAndDirectoriesByPath(
    const WChar* _path_dir, TList<ZWString>* _file_list_ptr
) noexcept {
    ReturnType ret_val = kOK;
    try {
        Z_CHECK(
            !std::filesystem::exists(_path_dir), error_code::kFFileSystemErrorCode_PathNotExist,
            L"Path not exist! path: %ls", _path_dir);
        Z_CHECK(
            !std::filesystem::is_directory(_path_dir), error_code::kFFileSystemErrorCode_PathNotDirectory,
            L"Path not directory! path: %ls", _path_dir);
        for (const auto& entry : std::filesystem::directory_iterator(_path_dir)) {
            if (std::filesystem::is_regular_file(entry.path()) || std::filesystem::is_directory(entry.path())) {
                _file_list_ptr->PushBack(entry.path().c_str());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFileTreeByPath(const WChar* _path_dir, TList<ZWString>* _file_list_ptr) noexcept {
    ReturnType ret_val = kOK;
    try {
        Z_CHECK(
            !std::filesystem::exists(_path_dir), error_code::kFFileSystemErrorCode_PathNotExist,
            L"Path not exist! path: %ls", _path_dir);
        Z_CHECK(
            !std::filesystem::is_directory(_path_dir), error_code::kFFileSystemErrorCode_PathNotDirectory,
            L"Path not directory! path: %ls", _path_dir);
        for (const auto& entry : std::filesystem::directory_iterator(_path_dir)) {
            if (std::filesystem::is_regular_file(entry.path())) {
                _file_list_ptr->PushBack(entry.path().c_str());
            }
            else if (std::filesystem::is_directory(entry.path())) {
                GetFileTreeByPath(entry.path().c_str(), _file_list_ptr);
            }
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFileInfoByPath(
    const ZWString& _file, ZFileInfo* _file_info_ptr
) noexcept {
    ReturnType ret_val = kOK;
    std::filesystem::path path(_file.String());
    _file_info_ptr->path_ = _file.String();
    _file_info_ptr->name_ = path.filename().c_str();
    _file_info_ptr->extension_ = path.extension().c_str();
    _file_info_ptr->directory_ = path.parent_path().c_str();
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFileInfoListByPathList(
    const TList<ZWString>& _file_list, TList<ZFileInfo>* _file_info_list_ptr
) noexcept {
    ReturnType ret_val = kOK;
    for (auto file_path = _file_list.Begin(); file_path != _file_list.End(); ++file_path) {
        std::filesystem::path path(file_path->String());
        _file_info_list_ptr->PushBack(
            ZFileInfo(file_path->String(), path.filename().c_str(), path.extension().c_str(), path.parent_path().c_str())
        );
    }
    return ret_val;
}
  
CORE_DLLAPI NODISCARD ReturnType GetFileByFileSelector(
    const TVector<ZFileFilter>& _file_filter_vector, ZWString* _file_ptr
) noexcept {
    ReturnType ret_val = kOK;

    //Init COM lib
    HRESULT link_code = CoInitialize(nullptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to init COM library!");
        return ret_val;
    }

    IFileOpenDialog* file_open_dialog = nullptr;

    //create dialog instance
    link_code = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&file_open_dialog));
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to create file open dialog instance!");
        CoUninitialize();
        return ret_val;
    }

    //set filter
    file_open_dialog->SetFileTypes(
        static_cast<UInt32>(_file_filter_vector.Size()), 
        reinterpret_cast<const COMDLG_FILTERSPEC*>(_file_filter_vector.DataPtr())
    );

    //shows the file selector, returns neg value if no file selected
    link_code = file_open_dialog->Show(nullptr);
    if (FAILED(link_code)) {
        Z_LOG_MESSAGE(L"No file selected!");
        CoUninitialize();
        return ret_val;
    }

    //get the selected file
    IShellItem* file_item_ptr;
    link_code = file_open_dialog->GetResult(&file_item_ptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to get the shell items!");
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    //get file path
    WChar* file_path = nullptr;
    link_code = file_item_ptr->GetDisplayName(SIGDN_FILESYSPATH, &file_path);
    if (!SUCCEEDED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to get the file path!");
        file_item_ptr->Release();
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    //save the value.
    *_file_ptr = file_path;

    //release resourse
    file_item_ptr->Release();
    file_open_dialog->Release();
    CoUninitialize();

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFilesByFileSelector(
    const TVector<ZFileFilter>& _file_filter_vector, TList<ZWString>* _file_list_ptr
) noexcept {
    ReturnType ret_val = kOK;

    //Init COM lib
    HRESULT link_code = CoInitialize(nullptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to init COM library!");
        return ret_val;
    }

    IFileOpenDialog* file_open_dialog = nullptr;

    //create dialog instance
    link_code = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&file_open_dialog));
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to create file open dialog instance!");
        CoUninitialize();
        return ret_val;
    }

    //set options to multiple files
    file_open_dialog->SetOptions(FOS_ALLOWMULTISELECT | FOS_FILEMUSTEXIST);

    //set filter
    file_open_dialog->SetFileTypes(
        static_cast<UInt32>(_file_filter_vector.Size()), 
        reinterpret_cast<const COMDLG_FILTERSPEC*>(_file_filter_vector.DataPtr())
    );

    //shows the file selector, returns neg value if no file selected
    link_code = file_open_dialog->Show(nullptr);
    if (FAILED(link_code)) {
        Z_LOG_MESSAGE(L"No file selected!");
        CoUninitialize();
        return ret_val;
    }

    //get the selected file
    IShellItemArray* file_items_ptr;
    link_code = file_open_dialog->GetResults(&file_items_ptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to get the shell items!");
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    DWORD file_num = 0;
    link_code = file_items_ptr->GetCount(&file_num);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Get file num failed!");
        file_items_ptr->Release();
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    for (DWORD i = 0; i < file_num; ++i) {
        IShellItem* file_item;
        link_code = file_items_ptr->GetItemAt(i, &file_item);
        if (FAILED(link_code)) {
            ret_val = error_code::kFFileSystemErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"Link error! Get file item failed!");
            file_items_ptr->Release();
            file_open_dialog->Release();
            CoUninitialize();
            return ret_val;
        }

        //get file path
        WChar* file_path = nullptr;
        link_code = file_item->GetDisplayName(SIGDN_FILESYSPATH, &file_path);
        if (FAILED(link_code)) {
            ret_val = error_code::kFFileSystemErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"Link error! Get file item failed!");
            file_item->Release();
            file_items_ptr->Release();
            file_open_dialog->Release();
            CoUninitialize();
            return ret_val;
        }

        _file_list_ptr->PushBack(file_path);

        //release resourse
        CoTaskMemFree(file_path);
        file_item->Release();
    }

    //release resourse
    file_items_ptr->Release();
    file_open_dialog->Release();
    CoUninitialize();

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFolderByFileSelector(ZWString* _folder_ptr) noexcept {
    ReturnType ret_val = kOK;

    //Init COM lib
    HRESULT link_code = CoInitialize(nullptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to init COM library!");
        return ret_val;
    }

    IFileOpenDialog* file_open_dialog = nullptr;

    //create dialog instance
    link_code = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&file_open_dialog));
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to create file open dialog instance!");
        CoUninitialize();
        return ret_val;
    }

    //set options to folder
    file_open_dialog->SetOptions(FOS_PICKFOLDERS | FOS_FILEMUSTEXIST);

    //shows the file selector, returns neg value if no file selected
    link_code = file_open_dialog->Show(nullptr);
    if (FAILED(link_code)) {
        Z_LOG_MESSAGE(L"No file selected!");
        CoUninitialize();
        return ret_val;
    }

    //get the selected file
    IShellItem* file_item_ptr;
    link_code = file_open_dialog->GetResult(&file_item_ptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to get the shell items!");
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    //get folder path
    WChar* folder_path = nullptr;
    link_code = file_item_ptr->GetDisplayName(SIGDN_FILESYSPATH, &folder_path);
    if (!SUCCEEDED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to get the folder path!");
        file_item_ptr->Release();
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    //save the value.
    *_folder_ptr = folder_path;

    //release resourse
    file_item_ptr->Release();
    file_open_dialog->Release();
    CoUninitialize();

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFoldersByFileSelector(TList<ZWString>* _folder_list_ptr) noexcept {
    ReturnType ret_val = kOK;

    //Init COM lib
    HRESULT link_code = CoInitialize(nullptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to init COM library!");
        return ret_val;
    }

    IFileOpenDialog* file_open_dialog = nullptr;

    //create dialog instance
    link_code = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&file_open_dialog));
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to create file open dialog instance!");
        CoUninitialize();
        return ret_val;
    }

    //set options to multiple files
    file_open_dialog->SetOptions(FOS_ALLOWMULTISELECT | FOS_PICKFOLDERS | FOS_FILEMUSTEXIST);

    //shows the file selector, returns neg value if no file selected
    link_code = file_open_dialog->Show(nullptr);
    if (FAILED(link_code)) {
        Z_LOG_MESSAGE(L"No file selected!");
        CoUninitialize();
        return ret_val;
    }

    //get the selected file
    IShellItemArray* file_items_ptr;
    link_code = file_open_dialog->GetResults(&file_items_ptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Failed to get the shell items!");
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    DWORD file_num = 0;
    link_code = file_items_ptr->GetCount(&file_num);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"Link error! Get file num failed!");
        file_items_ptr->Release();
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    for (DWORD i = 0; i < file_num; ++i) {
        IShellItem* file_item;
        link_code = file_items_ptr->GetItemAt(i, &file_item);
        if (FAILED(link_code)) {
            ret_val = error_code::kFFileSystemErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"Link error! Get file item failed!");
            file_items_ptr->Release();
            file_open_dialog->Release();
            CoUninitialize();
            return ret_val;
        }

        //get folder path
        WChar* folder_path = nullptr;
        link_code = file_item->GetDisplayName(SIGDN_FILESYSPATH, &folder_path);
        if (FAILED(link_code)) {
            ret_val = error_code::kFFileSystemErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"Link error! Get file item failed!");
            file_item->Release();
            file_items_ptr->Release();
            file_open_dialog->Release();
            CoUninitialize();
            return ret_val;
        }

        _folder_list_ptr->PushBack(folder_path);

        //release resourse
        CoTaskMemFree(folder_path);
        file_item->Release();
    }

    //release resourse
    file_items_ptr->Release();
    file_open_dialog->Release();
    CoUninitialize();

    return ret_val;
}

}//file_system
}//zengine
