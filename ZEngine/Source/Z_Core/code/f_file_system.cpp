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

}//file_system
}//zengine
