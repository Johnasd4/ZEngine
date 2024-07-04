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

#include "z_string.h"

#include "m_log.h"
#include "t_vector.h"

namespace zengine {
namespace string {

/*
    Delete files by the given path.
*/
CORE_DLLAPI NODISCARD ReturnType String2WString(const Char* _str, TVector<WChar>* _out_str_ptr) noexcept {
    ReturnType ret_val = kOK;
    //calculate length
    SizeType str_len = std::mbstowcs(nullptr, _str, 0);
    //error
    if (str_len == -1) {
        *_out_str_ptr = { L'\0' };
        ret_val = error_code::kZStringErrorCodeInvalidString;
        Z_LOG_ERROR(ret_val, 0, "std::mbstowcs() error!");
        return ret_val;
    }
    _out_str_ptr->Reserve(str_len + 1);
    std::mbstowcs(_out_str_ptr->DataPtr(), _str, str_len + 1);
    return ret_val;
}

/*
    Translate wide string to narrow string.
*/
CORE_DLLAPI NODISCARD ReturnType WString2String(const WChar* _str, TVector<Char>* _out_str_ptr) noexcept {
    ReturnType ret_val = kOK;
    //calculate length
    SizeType str_len = std::wcstombs(nullptr, _str, 0);
    //error
    if (str_len == -1) {
        *_out_str_ptr = { L'\0' };
        ret_val = error_code::kZStringErrorCodeInvalidString;
        Z_LOG_ERROR(ret_val, 0, "std::wcstombs() error!");
        return ret_val;
    }
    _out_str_ptr->Reserve(str_len + 1);
    std::wcstombs(_out_str_ptr->DataPtr(), _str, str_len + 1);
    return ret_val;
}

}//string
}//zengine