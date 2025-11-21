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
#include "z_memory.h"

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

CORE_DLLAPI NODISCARD ReturnType String2Int32(Int32* _ans_ptr, const Char* _str) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    Char* err_str;
    *_ans_ptr = std::strtol(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kZStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kZStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType String2Int64(Int64* _ans_ptr, const Char* _str) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    Char* err_str;
    *_ans_ptr = std::strtoll(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kZStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kZStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType String2UInt32(UInt32* _ans_ptr, const Char* _str) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    Char* err_str;
    *_ans_ptr = std::strtoul(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kZStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kZStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType String2UInt64(UInt64* _ans_ptr, const Char* _str) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    Char* err_str;
    *_ans_ptr = std::strtoull(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kZStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kZStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType String2Float32(Float32* _ans_ptr, const Char* _str) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    Char* err_str;
    *_ans_ptr = std::strtof(_str, &err_str);
    if (_str == err_str) {
        ret_val = error_code::kZStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kZStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType String2Float64(Float64* _ans_ptr, const Char* _str) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    Char* err_str;
    *_ans_ptr = std::strtod(_str, &err_str);
    if (_str == err_str) {
        ret_val = error_code::kZStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kZStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType WString2Int32(Int32* _ans_ptr, const WChar* _str) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    WChar* err_str;
    *_ans_ptr = std::wcstol(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kZStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kZStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType WString2Int64(Int64* _ans_ptr, const WChar* _str) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    WChar* err_str;
    *_ans_ptr = std::wcstoll(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kZStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kZStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType WString2UInt32(UInt32* _ans_ptr, const WChar* _str) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    WChar* err_str;
    *_ans_ptr = std::wcstoul(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kZStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kZStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType WString2UInt64(UInt64* _ans_ptr, const WChar* _str) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    WChar* err_str;
    *_ans_ptr = std::wcstoull(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kZStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kZStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType WString2Float32(Float32* _ans_ptr, const WChar* _str) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    WChar* err_str;
    *_ans_ptr = std::wcstof(_str, &err_str);
    if (_str == err_str) {
        ret_val = error_code::kZStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kZStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType WString2Float64(Float64* _ans_ptr, const WChar* _str) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    WChar* err_str;
    *_ans_ptr = std::wcstod(_str, &err_str);
    if (_str == err_str) {
        ret_val = error_code::kZStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kZStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ZString GenerateString(const Char* _format, ...) noexcept {
    ArgListType args;
    va_start(args, _format);
    Int32 buffer_size = vsnprintf(nullptr, 0, _format, args) + 1;
    ZMemory memory(buffer_size * sizeof(Char));
    Int32 string_size = vsprintf(memory.DataPtr<Char*>(), _format, args);
    ZString ans_str(memory.DataPtr<Char*>(), string_size);
    va_end(args);
    return ans_str;
}

CORE_DLLAPI NODISCARD ZWString GenerateWString(const WChar* _format, ...) noexcept {
    ArgListType args;
    va_start(args, _format);
    Int32 buffer_size = vswprintf(nullptr, 0, _format, args) + 1;
    ZMemory memory(buffer_size * sizeof(WChar));
    Int32 string_size = vswprintf(memory.DataPtr<WChar*>(), _format, args);
    ZWString ans_str(memory.DataPtr<WChar*>(), string_size);
    va_end(args);
    return ans_str;
}

}//string
}//zengine