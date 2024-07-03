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

/*
    Delete files by the given path.
*/
CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const Char* _path) noexcept {
    ReturnType ret_val = kOK;
    try {
        if (std::filesystem::remove(_path)) {
            Z_LOG_MESSAGE(L"File deleted successfully! path: %ls", _path);
        }
        else {
            ret_val = error_code::kFFileSystemErrorCodeFileNotFound;
            Z_LOG_ERROR(ret_val, 0, "File not found! offset: %d, seek_type: %s", _path);
            std::wcout << L"File not found." << std::endl;
            return ret_val;
        }
    }
    catch (const std::filesystem::filesystem_error& exception) {
        std::wcerr << L"Error deleting file: " << exception.what() << std::endl;
    }

    return ret_val;
}

///*
//    Delete files by the given path.
//*/
//CORE_DLLAPI NODISCARD ReturnType DeleteFileByPath(const WChar* _path) noexcept {
//
//}

}//file_system
}//zengine
