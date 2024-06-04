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

#include "z_file.h"
#include "m_log.h"

namespace zengine {

CORE_DLLAPI NODISCARD ReturnType ZFile::Open(const CChar* file_dir, const CChar* open_type) noexcept {
    ReturnType ret_val = kOK;

    file_ptr = fopen(file_dir, open_type);
    if (file_ptr == nullptr) {
        ret_val = kErrCodeZFileOpenFileFailed;
        Z_LOG_ERR(ret_val, 0, "Open file failed! file_dir: %s, open_type: %s", file_dir, open_type);
        return ret_val;
    }
    
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType ZFile::Close() noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(file_ptr == nullptr, kErrCodeZFileFilePtrNull, 0, "No file opened!");
    
    if (fclose(file_ptr) != 0) {
        ret_val = kErrCodeZFileCloseFileFailed;
        Z_LOG_ERR(ret_val, 0, "Close file failed!");
        return ret_val;
    }
    
    file_ptr == nullptr;

    return ret_val;
}

}//zengine
