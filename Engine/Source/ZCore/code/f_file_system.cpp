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

CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const WChar* _path) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    try {
        if (std::filesystem::remove(_path)) {
            Z_LOG_SUCCESS(L"File deleted successfully! path: %ls", _path);
        }
        else {
            TVector<Char> str_path;
            link_code = string::WString2String(_path, &str_path);
            if (link_code != kOK) {
                ret_val = error_code::kFFileSystemErrorCodeLinkError;
                Z_LOG_ERROR(ret_val, link_code, "string::WString2String() link error!");
                return ret_val;
            }
            ret_val = error_code::kFFileSystemErrorCodeFileNotFound;
            Z_LOG_ERROR(ret_val, 0, "File not found! path: %s", str_path.DataPtr());
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        TVector<Char> str_path;
        link_code = string::WString2String(_path, &str_path);
        if (link_code != kOK) {
            ret_val = error_code::kFFileSystemErrorCodeLinkError;
            Z_LOG_ERROR(ret_val, link_code, "string::WString2String() link error!");
            return ret_val;
        }
        ret_val = error_code::kFFileSystemErrorCodeSystemError;
        Z_LOG_ERROR(ret_val, 0, "System error! path: %s error msg: %s", str_path.DataPtr(), exception.what());
        return ret_val;
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType GetFilesByPath(const WChar* _path, TList<ZWString>* file_list_ptr) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    try {
        if (std::filesystem::exists(_path) && std::filesystem::is_directory(_path)) {
            for (const auto& entry : fs::directory_iterator(directory)) {
                if (fs::is_regular_file(entry.path())) {
                    std::cout << "File: " << entry.path().filename().string() << std::endl;
                }
                else if (fs::is_directory(entry.path())) {
                    std::cout << "Directory: " << entry.path().filename().string() << std::endl;
                }
            }
        }
        else {
            std::cerr << "The path specified is not a directory or does not exist." << std::endl;
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return ret_val;
}

}//file_system
}//zengine
