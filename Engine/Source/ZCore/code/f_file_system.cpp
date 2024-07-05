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

#include <filesystem>


#include "m_log.h"

namespace zengine {
namespace file_system {

CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    try {
        if (std::filesystem::remove(_path_dir)) {
            Z_LOG_SUCCESS(L"File deleted successfully! path: %ls", string::String2WString(_path_dir).String());
        }
        else {
            ret_val = error_code::kFFileSystemErrorCodeFileNotFound;
            Z_LOG_ERROR(ret_val, 0, L"File not found! path: %ls", string::String2WString(_path_dir).String());
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
        Z_LOG_ERROR(
            ret_val, 0, L"System error! path: %ls error msg: %ls", 
            string::String2WString(_path_dir).String(), string::String2WString(exception.what()).String());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const WChar* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    try {
        if (std::filesystem::remove(_path_dir)) {
            Z_LOG_SUCCESS(L"File deleted successfully! path: %ls", _path_dir);
        }
        else {
            ret_val = error_code::kFFileSystemErrorCodeFileNotFound;
            Z_LOG_ERROR(ret_val, 0, L"File not found! path: %ls", _path_dir);
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
    ReturnType link_code = kOK;
    try {
        if (std::filesystem::exists(_path_dir) && std::filesystem::is_directory(_path_dir)) {
            for (const auto& file : std::filesystem::directory_iterator(_path_dir)) {
                if (std::filesystem::is_regular_file(file.path())) {
                    std::cout << "File: " << file.path().filename().string() << std::endl;
                }
                else if (std::filesystem::is_directory(file.path())) {
                    std::cout << "Directory: " << file.path().filename().string() << std::endl;
                }
            }
        }
        else {
            std::cerr << "The path specified is not a directory or does not exist." << std::endl;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        std::cerr << "Filesystem error: " << exception.what() << std::endl;
    }
    catch (const std::exception& exception) {
        std::cerr << "Error: " << exception.what() << std::endl;
    }
    return ret_val;
}

}//file_system
}//zengine
