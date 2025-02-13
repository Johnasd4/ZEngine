/*
    Copyright (c) YuLin Zhu (ÖìÓêÁÖ)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (ÖìÓêÁÖ)
    Contact: 1152325286@qq.com
*/
#define CORE_DLLFILE

#include "f_file_system.h"




#include "m_log.h"

namespace zengine {
namespace file_system {

CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const WChar* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    try {
        if (!std::filesystem::exists(_path_dir)) {
            Z_LOG_MESSAGE(L"File does not exist! path: %ls", _path_dir);
        }
        else if (std::filesystem::remove(_path_dir)) {
            Z_LOG_SUCCESS(L"Deleted file succeed! path: %ls", _path_dir);
        }
        else {
            Z_LOG_FAILURE(L"Deleted file failed! path: %ls", _path_dir);
            ret_val = error_code::kFFileSystemErrorCodeFileDeleteFailed;
            Z_LOG_ERROR(ret_val, 0, L"Deleted file failed! path: %ls", _path_dir);
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
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
            Z_LOG_MESSAGE(L"File does not exist! path: %ls", _old_path_dir);
            return ret_val;
        }
        std::filesystem::rename(_old_path_dir, _new_path_dir);
        Z_LOG_SUCCESS(L"Rename file succeed! path: %ls -> %ls", _old_path_dir, _new_path_dir);
        return ret_val;
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! old path: %ls new path: %ls error msg: %ls",
            _old_path_dir, _new_path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType CreateDirectoryByPath(const WChar* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    try {
        if (std::filesystem::exists(_path_dir)) {
            Z_LOG_MESSAGE(L"Directory already exist! path: %ls", _path_dir);
        }
        else if (std::filesystem::create_directories(_path_dir)) {
            Z_LOG_SUCCESS(L"Create directory succeed! path: %ls", _path_dir);
        }
        else {
            Z_LOG_FAILURE(L"Create directory failed! path: %ls", _path_dir);
            ret_val = error_code::kFFileSystemErrorCodeCreateDirectoryFailed;
            Z_LOG_ERROR(ret_val, 0, L"Create directory failed! path: %ls", _path_dir);
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
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
            Z_LOG_MESSAGE(L"Directory not exist! path: %ls", _path_dir);
        }
        else if (std::filesystem::remove_all(_path_dir)) {
            Z_LOG_SUCCESS(L"Deleted directory succeed! path: %ls", _path_dir);
        }
        else {
            Z_LOG_FAILURE(L"Deleted directory failed! path: %ls", _path_dir);
            ret_val = error_code::kFFileSystemErrorCodeDirectoryDeleteFailed;
            Z_LOG_ERROR(ret_val, 0, L"Deleted directory failed! path: %ls", _path_dir);
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFilesByPath(const WChar* _path_dir, TList<ZWString>* file_list_ptr) noexcept {
    ReturnType ret_val = kOK;
    try {
        Z_CHECK(
            !std::filesystem::exists(_path_dir), error_code::kFFileSystemErrorCodePathNotExist,
            L"Path not exist! path: %ls", _path_dir);
        Z_CHECK(
            !std::filesystem::is_directory(_path_dir), error_code::kFFileSystemErrorCodePathNotDirectory,
            L"Path not directory! path: %ls", _path_dir);
        for (const auto& entry : std::filesystem::directory_iterator(_path_dir)) {
            if (std::filesystem::is_regular_file(entry.path())) {
                file_list_ptr->PushBack(entry.path().c_str());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
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

CORE_DLLAPI NODISCARD ReturnType GetDirectoriesByPath(const WChar* _path_dir, TList<ZWString>* file_list_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    try {
        Z_CHECK(
            !std::filesystem::exists(_path_dir), error_code::kFFileSystemErrorCodePathNotExist,
            L"Path not exist! path: %ls", _path_dir);
        Z_CHECK(
            !std::filesystem::is_directory(_path_dir), error_code::kFFileSystemErrorCodePathNotDirectory,
            L"Path not directory! path: %ls", _path_dir);
        for (const auto& file : std::filesystem::directory_iterator(_path_dir)) {
            if (std::filesystem::is_directory(file.path())) {
                file_list_ptr->PushBack(file.path().c_str());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFilesAndDirectoriesByPath(
    const WChar* _path_dir, TList<ZWString>* file_list_ptr
) noexcept {
    ReturnType ret_val = kOK;
    try {
        Z_CHECK(
            !std::filesystem::exists(_path_dir), error_code::kFFileSystemErrorCodePathNotExist,
            L"Path not exist! path: %ls", _path_dir);
        Z_CHECK(
            !std::filesystem::is_directory(_path_dir), error_code::kFFileSystemErrorCodePathNotDirectory,
            L"Path not directory! path: %ls", _path_dir);
        for (const auto& entry : std::filesystem::directory_iterator(_path_dir)) {
            if (std::filesystem::is_regular_file(entry.path()) || std::filesystem::is_directory(entry.path())) {
                file_list_ptr->PushBack(entry.path().c_str());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFileTreeByPath(const WChar* _path_dir, TList<ZWString>* file_list_ptr) noexcept {
    ReturnType ret_val = kOK;
    try {
        Z_CHECK(
            !std::filesystem::exists(_path_dir), error_code::kFFileSystemErrorCodePathNotExist,
            L"Path not exist! path: %ls", _path_dir);
        Z_CHECK(
            !std::filesystem::is_directory(_path_dir), error_code::kFFileSystemErrorCodePathNotDirectory,
            L"Path not directory! path: %ls", _path_dir);
        for (const auto& entry : std::filesystem::directory_iterator(_path_dir)) {
            if (std::filesystem::is_regular_file(entry.path())) {
                file_list_ptr->PushBack(entry.path().c_str());
            }
            else if (std::filesystem::is_directory(entry.path())) {
                GetFileTreeByPath(entry.path().c_str(), file_list_ptr);
            }
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    catch (const std::exception& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls",
            _path_dir, string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFileInfoListByPathList(
    const TList<ZWString>* file_list_ptr, TList<ZFileInfo>* file_info_list_ptr
) noexcept {
    ReturnType ret_val = kOK;
    for (auto file_path = file_list_ptr->Begin(); file_path != file_list_ptr->End(); ++file_path) {
        std::filesystem::path path(file_path->String());
        file_info_list_ptr->PushBack(
            ZFileInfo(file_path->String(), path.filename().c_str(), path.extension().c_str(), path.parent_path().c_str())
        );
    }
    return ret_val;
}

}//file_system
}//zengine
