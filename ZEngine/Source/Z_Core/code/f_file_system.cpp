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
#include "drive/d_pch.h"

#include "f_file_system.h"

#include "f_string.h"
#include "m_log.h"
#include "t_array.h"
#include "t_fixed_memory.h"

namespace zengine {
namespace file_system {

CORE_DLLAPI NODISCARD ZStringView ExecutePath() noexcept {
    static ZString path = std::invoke([]() {
        std::error_code error_code;
        auto path = std::filesystem::current_path(error_code);
        if (error_code) {
            return ZString();
        }

        auto path_char_string = path.c_str();
        if constexpr (kSameType<decltype(path_char_string), const Char*>) {
            return ZString(path_char_string);
        }
        else {
            return string::WStringToString(path_char_string);
        }
    });
    return path;
}

CORE_DLLAPI NODISCARD ZStringView ExecutablePath() noexcept {
    static ZString path = std::invoke([]() {
        //get length
        Int32 length = wai_getExecutablePath(NULL, 0, NULL);
        if (length <= 0) {
            return ZString();
        }

        //get path
        TFixedMemory<kMaxFileDirLength> buffer;
        wai_getExecutablePath(buffer.DataPtr<Char>(), length, NULL);

        return ZString(buffer.DataPtr<Char>(), length);
    });
    return path;
}

CORE_DLLAPI NODISCARD ZStringView ExecutableDirectoryPath() noexcept {
    static ZStringView s_dir = std::invoke([]() {
        //get length
        Int32 length;
        wai_getExecutablePath(NULL, 0, &length);

        return ZStringView(ExecutablePath().DataPtr(), length);
    });
    return s_dir;
}

CORE_DLLAPI NODISCARD ReturnType ProgramInfo(ZFileInfo* _file_info_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = GetFileInfoByPath(
        ZString((std::filesystem::current_path() / std::filesystem::path(__argv[0]).filename()).string().c_str()),
        _file_info_ptr
    );
    if (link_code != kOK) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "file_system::GetFileInfoByPath() link error!");
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType CreateFileByPath(const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    try {
        if (std::filesystem::exists(_path_dir)) {
            Z_DEBUG_LOG_MESSAGE("File already exist! path: {}", _path_dir);
        }
        else {
            std::filesystem::path path(_path_dir);

            //create directory if not exist
            link_code = CreateDirectoryByPath(_path_dir);
            if (link_code != kOK) {
                ret_val = error_code::kFFileSystemErrorCode_LinkError;
                Z_DEBUG_LOG_FAILURE("Create file failed! path: {}", _path_dir);
                Z_LOG_ERROR(ret_val, link_code, "file_system::CreateDirectoryByPath() link error!");
                return ret_val;
            }

            //create file
            std::ofstream file(_path_dir);
            if (file.is_open()) {
                file.close();
                Z_DEBUG_LOG_SUCCESS("Create file succeed! path: {}", _path_dir);
            }
            else {
                Z_DEBUG_LOG_FAILURE("Create file failed! path: {}", _path_dir);
                ret_val = error_code::kFFileSystemErrorCode_FileCreateFailed;
                Z_LOG_ERROR(ret_val, 0, "Create file failed! path: {}", _path_dir);
                return ret_val;
            }
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(ret_val, 0, "Create file failed! path: {}", _path_dir);
        Z_LOG_ERROR(
            ret_val, 0, "System error! path: {} error msg: {}", 
            _path_dir, exception.what()
        );
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    try {
        if (!std::filesystem::exists(_path_dir)) {
            Z_DEBUG_LOG_MESSAGE("File does not exist! path: {}", _path_dir);
        }
        else if (std::filesystem::remove(_path_dir)) {
            Z_DEBUG_LOG_SUCCESS("Deleted file succeed! path: {}", _path_dir);
        }
        else {

            ret_val = error_code::kFFileSystemErrorCode_FileDeleteFailed;
            Z_LOG_ERROR(ret_val, 0, "Deleted file failed! path: {}", _path_dir);
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_DEBUG_LOG_FAILURE("Deleted file failed! path: {}", _path_dir);
        Z_LOG_ERROR(
            ret_val, 0, "System error! path: {} error msg: {}", 
            _path_dir, exception.what());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType RenameFileByPath(const Char* _old_path_dir, const Char* _new_path_dir) noexcept {
    ReturnType ret_val = kOK;
    try {
        if (!std::filesystem::exists(_old_path_dir)) {
            Z_DEBUG_LOG_MESSAGE("File does not exist! path: {}", _old_path_dir);
            return ret_val;
        }
        std::filesystem::rename(_old_path_dir, _new_path_dir);
        Z_DEBUG_LOG_SUCCESS("Rename file succeed! path: {} -> {}", _old_path_dir, _new_path_dir);
        return ret_val;
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_DEBUG_LOG_FAILURE("Rename file failed! old_path: {} new_path: {}", _old_path_dir, _new_path_dir);
        Z_LOG_ERROR(
            ret_val, 0, "System error! old path: {} new path: {} error msg: {}",
            _old_path_dir, _new_path_dir, exception.what());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType CopyFileByPath(
    const Char* _source_path_dir,
    const Char* _target_path_dir,
    Bool overwrite_exist
) noexcept {
    ReturnType ret_val = kOK;
    try {

        //check source file exist
        if (!std::filesystem::exists(_source_path_dir)) {
            ret_val = error_code::kFFileSystemErrorCode_PathNotExist;
            Z_DEBUG_LOG_FAILURE(
                "Copy file failed! source_path: {} target_path: {}",
                _target_path_dir, _source_path_dir
            );
            Z_LOG_ERROR(ret_val, 0, "Copy file failed! Source path not exist! path: {}", _source_path_dir);
            return ret_val;
        }

        //generate target directory if not exist
        std::filesystem::path path(_target_path_dir);
        if (!std::filesystem::exists(path.parent_path().c_str())) {
            if (!std::filesystem::create_directories(path.parent_path().c_str())) {
                ret_val = error_code::kFFileSystemErrorCode_PathNotDirectory;
                Z_DEBUG_LOG_FAILURE(
                    "Copy file failed! source_path: {} target_path: {}",
                    _target_path_dir, _source_path_dir
                );
                Z_LOG_ERROR(
                    ret_val, 0,
                    "Copy file failed! Target directory not valid! path: {}",
                    _target_path_dir
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
                "Copy file failed! source_path: {} target_path: {}",
                _target_path_dir, _source_path_dir
            );
            Z_LOG_ERROR(
                ret_val, 0, 
                "Copy file failed! source_path: {} target_path: {}", 
                _target_path_dir, _source_path_dir
            );
            return ret_val;
        }
        
        Z_DEBUG_LOG_SUCCESS(
            "Copy file succeed! source_path: {} target_path: {}",
            _target_path_dir, _source_path_dir
        );

        return ret_val;
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_DEBUG_LOG_FAILURE(
            "Copy file failed! source_path: {} target_path: {}",
            _target_path_dir, _source_path_dir
        );
        Z_LOG_ERROR(
            ret_val, 0, "System error! source_path: {} target_path: {}",
            _target_path_dir, _source_path_dir, exception.what());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType CreateDirectoryByPath(ZStringView _path_dir) noexcept {
    ReturnType ret_val = kOK;
    try {
        std::filesystem::path path_dir(_path_dir.STDStringView());
        if (std::filesystem::exists(path_dir)) {
            Z_DEBUG_LOG_MESSAGE("Directory already exist! path: {}", _path_dir);
        }
        else if (std::filesystem::create_directories(path_dir)) {
            Z_DEBUG_LOG_SUCCESS("Create directory succeed! path: {}", _path_dir);
        }
        else {
            ret_val = error_code::kFFileSystemErrorCode_CreateDirectoryFailed;
            Z_DEBUG_LOG_FAILURE("Create directory failed! path: {}", _path_dir);
            Z_LOG_ERROR(ret_val, 0, "Create directory failed! path: {}", _path_dir);
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_DEBUG_LOG_FAILURE("Create directory failed! path: {}", _path_dir);
        Z_LOG_ERROR(
            ret_val, 0, "System error! path: {} error msg: {}",
            _path_dir, exception.what());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_DEBUG_LOG_FAILURE("Create directory failed! path: {}", _path_dir);
        Z_LOG_ERROR(
            ret_val, 0, "System error! path: {} error msg: {}",
            _path_dir, exception.what());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType DeleteDirectoryByPath(ZStringView _path_dir) noexcept {
    ReturnType ret_val = kOK;
    try {
        std::filesystem::path path_dir(_path_dir.STDStringView());
        if (!std::filesystem::exists(path_dir)) {
            Z_DEBUG_LOG_MESSAGE("Directory not exist! path: {}", _path_dir);
        }
        else if (std::filesystem::remove_all(path_dir)) {
            Z_DEBUG_LOG_SUCCESS("Deleted directory succeed! path: {}", _path_dir);
        }
        else {
            ret_val = error_code::kFFileSystemErrorCode_DirectoryDeleteFailed;
            Z_DEBUG_LOG_FAILURE("Deleted directory failed! path: {}", _path_dir);
            Z_LOG_ERROR(ret_val, 0, "Deleted directory failed! path: {}", _path_dir);
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_DEBUG_LOG_FAILURE("Deleted directory failed! path: {}", _path_dir);
        Z_LOG_ERROR(
            ret_val, 0, "System error! path: {} error msg: {}",
            _path_dir, exception.what());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFilesByPath(const Char* _path_dir, TList<ZString>* _file_list_ptr) noexcept {
    ReturnType ret_val = kOK;
    try {
        Z_CHECK(
            !std::filesystem::exists(_path_dir), error_code::kFFileSystemErrorCode_PathNotExist,
            "Path not exist! path: {}", _path_dir);
        Z_CHECK(
            !std::filesystem::is_directory(_path_dir), error_code::kFFileSystemErrorCode_PathNotDirectory,
            "Path not directory! path: {}", _path_dir);
        for (const auto& entry : std::filesystem::directory_iterator(_path_dir)) {
            if (std::filesystem::is_regular_file(entry.path())) {
                _file_list_ptr->PushBack(entry.path().string().c_str());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, "System error! path: {} error msg: {}",
            _path_dir, exception.what());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, "System error! path: {} error msg: {}",
            _path_dir, exception.what());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD Bool PathExist(const Char* _path_dir) noexcept {
    return std::filesystem::exists(_path_dir);
}

CORE_DLLAPI NODISCARD Bool PathValid(const Char* _path_dir) noexcept {
    //nullptr check
    if (!_path_dir) {
        return false;
    }

    ZString path_dir(_path_dir);

    //empty check
    if (path_dir.Empty()) {
        return false;
    }

    //long file dir length check
    if (path_dir.Size() > 32767) {
        return false;
    }

    //normal file dir length check
    if (path_dir.Size() > 260 && path_dir.Find("\\\\?\\") != 0 && path_dir.Find("\\\\.\\") != 0) {
        return false;
    }

    //illegal char check
    static constexpr Char illegal_chars[] = "*?\"<>|";
    for (SizeType index = 0; index < sizeof(illegal_chars) - 1; ++index) {
        if (path_dir.Find(illegal_chars[index]) != ZString::kEnd) {
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

CORE_DLLAPI NODISCARD ReturnType GetDirectoriesByPath(ZStringView _path_dir, TList<ZString>* _file_list_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    try {
        std::filesystem::path path_dir(_path_dir.STDStringView());
        Z_CHECK(
            !std::filesystem::exists(path_dir), error_code::kFFileSystemErrorCode_PathNotExist,
            "Path not exist! path: {}", _path_dir);
        Z_CHECK(
            !std::filesystem::is_directory(path_dir), error_code::kFFileSystemErrorCode_PathNotDirectory,
            "Path not directory! path: {}", _path_dir);
        for (const auto& file : std::filesystem::directory_iterator(path_dir)) {
            if (std::filesystem::is_directory(file.path())) {
                auto str = file.path().string();
                _file_list_ptr->PushBack(ZString(str.c_str(), str.size()));
            }
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, "System error! path: {} error msg: {}",
            _path_dir, exception.what());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, "System error! path: {} error msg: {}",
            _path_dir, exception.what());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFilesAndDirectoriesByPath(
    const Char* _path_dir, TList<ZString>* _file_list_ptr
) noexcept {
    ReturnType ret_val = kOK;
    try {
        Z_CHECK(
            !std::filesystem::exists(_path_dir), error_code::kFFileSystemErrorCode_PathNotExist,
            "Path not exist! path: {}", _path_dir);
        Z_CHECK(
            !std::filesystem::is_directory(_path_dir), error_code::kFFileSystemErrorCode_PathNotDirectory,
            "Path not directory! path: {}", _path_dir);
        for (const auto& entry : std::filesystem::directory_iterator(_path_dir)) {
            if (std::filesystem::is_regular_file(entry.path()) || std::filesystem::is_directory(entry.path())) {
                _file_list_ptr->PushBack(entry.path().string().c_str());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, "System error! path: {} error msg: {}",
            _path_dir, exception.what());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, "System error! path: {} error msg: {}",
            _path_dir, exception.what());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFileTreeByPath(const Char* _path_dir, TList<ZString>* _file_list_ptr) noexcept {
    ReturnType ret_val = kOK;
    try {
        Z_CHECK(
            !std::filesystem::exists(_path_dir), error_code::kFFileSystemErrorCode_PathNotExist,
            "Path not exist! path: {}", _path_dir);
        Z_CHECK(
            !std::filesystem::is_directory(_path_dir), error_code::kFFileSystemErrorCode_PathNotDirectory,
            "Path not directory! path: {}", _path_dir);
        for (const auto& entry : std::filesystem::directory_iterator(_path_dir)) {
            if (std::filesystem::is_regular_file(entry.path())) {
                _file_list_ptr->PushBack(entry.path().string().c_str());
            }
            else if (std::filesystem::is_directory(entry.path())) {
                GetFileTreeByPath(entry.path().string().c_str(), _file_list_ptr);
            }
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, "System error! path: {} error msg: {}",
            _path_dir, exception.what());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, 0, "System error! path: {} error msg: {}",
            _path_dir, exception.what());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFileInfoByPath(
    const ZString& _file, ZFileInfo* _file_info_ptr
) noexcept {
    ReturnType ret_val = kOK;
    std::filesystem::path path(_file.DataPtr());
    _file_info_ptr->path_ = _file.DataPtr();
    _file_info_ptr->name_ = path.filename().string().c_str();
    _file_info_ptr->extension_ = path.extension().string().c_str();
    _file_info_ptr->directory_ = path.parent_path().string().c_str();
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFileInfoListByPathList(
    const TList<ZString>& _file_list, TList<ZFileInfo>* _file_info_list_ptr
) noexcept {
    ReturnType ret_val = kOK;
    for (auto file_path = _file_list.Begin(); file_path != _file_list.End(); ++file_path) {
        std::filesystem::path path(file_path->DataPtr());
        _file_info_list_ptr->PushBack(
            ZFileInfo(
                file_path->DataPtr(), 
                path.filename().string().c_str(), 
                path.extension().string().c_str(),
                path.parent_path().string().c_str()
            )
        );
    }
    return ret_val;
}
  
CORE_DLLAPI NODISCARD ReturnType GetFileByFileSelector(
    const TArray<ZFileFilter>& _file_filter_array, ZString* _file_ptr
) noexcept {
    ReturnType ret_val = kOK;

    //Init COM lib
    HRESULT link_code = CoInitialize(nullptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to init COM library!");
        return ret_val;
    }

    IFileOpenDialog* file_open_dialog = nullptr;

    //create dialog instance
    link_code = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&file_open_dialog));
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to create file open dialog instance!");
        CoUninitialize();
        return ret_val;
    }

    //set filter
    file_open_dialog->SetFileTypes(
        static_cast<UInt32>(_file_filter_array.Size()), 
        reinterpret_cast<const COMDLG_FILTERSPEC*>(_file_filter_array.DataPtr())
    );

    //shows the file selector, returns neg value if no file selected
    link_code = file_open_dialog->Show(nullptr);
    if (FAILED(link_code)) {
        Z_LOG_MESSAGE("No file selected!");
        CoUninitialize();
        return ret_val;
    }

    //get the selected file
    IShellItem* file_item_ptr;
    link_code = file_open_dialog->GetResult(&file_item_ptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to get the shell items!");
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    //get file path
    WChar* file_path = nullptr;
    link_code = file_item_ptr->GetDisplayName(SIGDN_FILESYSPATH, &file_path);
    if (!SUCCEEDED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to get the file path!");
        file_item_ptr->Release();
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    //save the value.
    *_file_ptr = string::WStringToString(file_path);

    //release resourse
    file_item_ptr->Release();
    file_open_dialog->Release();
    CoUninitialize();

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFilesByFileSelector(
    const TArray<ZFileFilter>& _file_filter_array, TList<ZString>* _file_list_ptr
) noexcept {
    ReturnType ret_val = kOK;

    //Init COM lib
    HRESULT link_code = CoInitialize(nullptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to init COM library!");
        return ret_val;
    }

    IFileOpenDialog* file_open_dialog = nullptr;

    //create dialog instance
    link_code = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&file_open_dialog));
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to create file open dialog instance!");
        CoUninitialize();
        return ret_val;
    }

    //set options to multiple files
    file_open_dialog->SetOptions(FOS_ALLOWMULTISELECT | FOS_FILEMUSTEXIST);

    //set filter
    file_open_dialog->SetFileTypes(
        static_cast<UInt32>(_file_filter_array.Size()), 
        reinterpret_cast<const COMDLG_FILTERSPEC*>(_file_filter_array.DataPtr())
    );

    //shows the file selector, returns neg value if no file selected
    link_code = file_open_dialog->Show(nullptr);
    if (FAILED(link_code)) {
        Z_LOG_MESSAGE("No file selected!");
        CoUninitialize();
        return ret_val;
    }

    //get the selected file
    IShellItemArray* file_items_ptr;
    link_code = file_open_dialog->GetResults(&file_items_ptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to get the shell items!");
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    DWORD file_num = 0;
    link_code = file_items_ptr->GetCount(&file_num);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Get file num failed!");
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
            Z_LOG_ERROR(ret_val, link_code, "Link error! Get file item failed!");
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
            Z_LOG_ERROR(ret_val, link_code, "Link error! Get file item failed!");
            file_item->Release();
            file_items_ptr->Release();
            file_open_dialog->Release();
            CoUninitialize();
            return ret_val;
        }

        _file_list_ptr->PushBack(string::WStringToString(file_path));

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

CORE_DLLAPI NODISCARD ReturnType GetFolderByFileSelector(ZString* _folder_ptr) noexcept {
    ReturnType ret_val = kOK;

    //Init COM lib
    HRESULT link_code = CoInitialize(nullptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to init COM library!");
        return ret_val;
    }

    IFileOpenDialog* file_open_dialog = nullptr;

    //create dialog instance
    link_code = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&file_open_dialog));
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to create file open dialog instance!");
        CoUninitialize();
        return ret_val;
    }

    //set options to folder
    file_open_dialog->SetOptions(FOS_PICKFOLDERS | FOS_FILEMUSTEXIST);

    //shows the file selector, returns neg value if no file selected
    link_code = file_open_dialog->Show(nullptr);
    if (FAILED(link_code)) {
        Z_LOG_MESSAGE("No file selected!");
        CoUninitialize();
        return ret_val;
    }

    //get the selected file
    IShellItem* file_item_ptr;
    link_code = file_open_dialog->GetResult(&file_item_ptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to get the shell items!");
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    //get folder path
    WChar* folder_path = nullptr;
    link_code = file_item_ptr->GetDisplayName(SIGDN_FILESYSPATH, &folder_path);
    if (!SUCCEEDED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to get the folder path!");
        file_item_ptr->Release();
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    //save the value.
    *_folder_ptr = string::WStringToString(folder_path);

    //release resourse
    file_item_ptr->Release();
    file_open_dialog->Release();
    CoUninitialize();

    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFoldersByFileSelector(TList<ZString>* _folder_list_ptr) noexcept {
    ReturnType ret_val = kOK;

    //Init COM lib
    HRESULT link_code = CoInitialize(nullptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to init COM library!");
        return ret_val;
    }

    IFileOpenDialog* file_open_dialog = nullptr;

    //create dialog instance
    link_code = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&file_open_dialog));
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to create file open dialog instance!");
        CoUninitialize();
        return ret_val;
    }

    //set options to multiple files
    file_open_dialog->SetOptions(FOS_ALLOWMULTISELECT | FOS_PICKFOLDERS | FOS_FILEMUSTEXIST);

    //shows the file selector, returns neg value if no file selected
    link_code = file_open_dialog->Show(nullptr);
    if (FAILED(link_code)) {
        Z_LOG_MESSAGE("No file selected!");
        CoUninitialize();
        return ret_val;
    }

    //get the selected file
    IShellItemArray* file_items_ptr;
    link_code = file_open_dialog->GetResults(&file_items_ptr);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Failed to get the shell items!");
        file_open_dialog->Release();
        CoUninitialize();
        return ret_val;
    }

    DWORD file_num = 0;
    link_code = file_items_ptr->GetCount(&file_num);
    if (FAILED(link_code)) {
        ret_val = error_code::kFFileSystemErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "Link error! Get file num failed!");
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
            Z_LOG_ERROR(ret_val, link_code, "Link error! Get file item failed!");
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
            Z_LOG_ERROR(ret_val, link_code, "Link error! Get file item failed!");
            file_item->Release();
            file_items_ptr->Release();
            file_open_dialog->Release();
            CoUninitialize();
            return ret_val;
        }

        _folder_list_ptr->PushBack(string::WStringToString(folder_path));

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
