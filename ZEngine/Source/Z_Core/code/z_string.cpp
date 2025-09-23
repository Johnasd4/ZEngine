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

#include "z_string.h"

#include <codecvt>

#include "t_array.h"
#include "t_vector.h"

namespace zengine {
namespace string {

CORE_DLLAPI NODISCARD ZWString String2WString(const Char* _str) noexcept {
    ReturnType ret_val = kOK;
    TVector<WChar> temp_str;
    //calculate length
    SizeType str_len = std::mbstowcs(nullptr, _str, 0);
    //invalid input string returns empty string
    if (str_len == -1) {
        return ZWString(L"");
    }
    temp_str.Reserve(str_len + 1);
    std::mbstowcs(temp_str.DataPtr(), _str, str_len + 1);
    return ZWString(temp_str.DataPtr());
}

CORE_DLLAPI NODISCARD ZString WString2String(const WChar* _str) noexcept {
    ReturnType ret_val = kOK;
    TVector<Char> temp_str;
    //calculate length
    SizeType str_len = std::wcstombs(nullptr, _str, 0);
    //invalid input string returns empty string
    if (str_len == -1) {
        return ZString("");
    }
    temp_str.Reserve(str_len + 1);
    std::wcstombs(temp_str.DataPtr(), _str, str_len + 1);
    return ZString(temp_str.DataPtr());
}

}//string
}//zengine