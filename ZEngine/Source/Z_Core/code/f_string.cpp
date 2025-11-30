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

#include "f_string.h"

#include "m_log.h"
#include "t_array.h"
#include "z_memory.h"

namespace zengine {
namespace string {

CORE_DLLAPI NODISCARD ZWString String2WString(const Char* _str) noexcept {
    ReturnType ret_val = kOK;
    TArray<WChar> temp_str;
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
    TArray<Char> temp_str;
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

CORE_DLLAPI NODISCARD ReturnType String2Int32(const Char* _str, Int32* _ans_ptr) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    Char* err_str;
    *_ans_ptr = std::strtol(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType String2Int64(const Char* _str, Int64* _ans_ptr) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    Char* err_str;
    *_ans_ptr = std::strtoll(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType String2UInt32(const Char* _str, UInt32* _ans_ptr) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    Char* err_str;
    *_ans_ptr = std::strtoul(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType String2UInt64(const Char* _str, UInt64* _ans_ptr) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    Char* err_str;
    *_ans_ptr = std::strtoull(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType String2Float32(const Char* _str, Float32* _ans_ptr) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    Char* err_str;
    *_ans_ptr = std::strtof(_str, &err_str);
    if (_str == err_str) {
        ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType String2Float64(const Char* _str, Float64* _ans_ptr) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    Char* err_str;
    *_ans_ptr = std::strtod(_str, &err_str);
    if (_str == err_str) {
        ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType WString2Int32(const WChar* _str, Int32* _ans_ptr) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    WChar* err_str;
    *_ans_ptr = std::wcstol(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType WString2Int64(const WChar* _str, Int64* _ans_ptr) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    WChar* err_str;
    *_ans_ptr = std::wcstoll(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType WString2UInt32(const WChar* _str, UInt32* _ans_ptr) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    WChar* err_str;
    *_ans_ptr = std::wcstoul(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType WString2UInt64(const WChar* _str, UInt64* _ans_ptr) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    WChar* err_str;
    *_ans_ptr = std::wcstoull(_str, &err_str, 10);
    if (_str == err_str) {
        ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType WString2Float32(const WChar* _str, Float32* _ans_ptr) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    WChar* err_str;
    *_ans_ptr = std::wcstof(_str, &err_str);
    if (_str == err_str) {
        ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ReturnType WString2Float64(const WChar* _str, Float64* _ans_ptr) noexcept {
    ReturnType ret_val = kOK;
    Int32& err_ref = errno;
    err_ref = 0;
    WChar* err_str;
    *_ans_ptr = std::wcstod(_str, &err_str);
    if (_str == err_str) {
        ret_val = error_code::kFStringErrorCode_StringToNumberCanNotTransform;
        Z_LOG_ERROR(ret_val, 0, L"Can not transform to number!");
    }
    else if (err_ref == ERANGE) {
        ret_val = error_code::kFStringErrorCode_StringToNumberOutOfRange;
        Z_LOG_ERROR(ret_val, 0, L"Number out of range!");
    }
    return ret_val;
}

CORE_DLLAPI NODISCARD ZString GenerateString(const Char* _format, ...) noexcept {
    ArgListType args;
    va_start(args, _format);
    Int32 buffer_size = vsnprintf(nullptr, 0, _format, args) + 1;
    ZMemory memory(buffer_size * sizeof(Char));
    Int32 string_size = vsprintf(memory.DataPtr<Char>(), _format, args);
    ZString ans_str(memory.DataPtr<Char>(), string_size);
    va_end(args);
    return ans_str;
}

CORE_DLLAPI NODISCARD ZWString GenerateWString(const WChar* _format, ...) noexcept {
    ArgListType args;
    va_start(args, _format);
    Int32 buffer_size = vswprintf(nullptr, 0, _format, args) + 1;
    ZMemory memory(buffer_size * sizeof(WChar));
    Int32 string_size = vswprintf(memory.DataPtr<WChar>(), _format, args);
    ZWString ans_str(memory.DataPtr<WChar>(), string_size);
    va_end(args);
    return ans_str;
}

CORE_DLLAPI NODISCARD TList<ZString> SplitToString(ZStringView _str, const Char _token) noexcept {
    ReturnType ret_val = kOK;
    SizeType start_index = 0;
    SizeType end_index = 0;
    SizeType str_len = 0;
    TList<ZString> result_list;
    while (end_index != _str.Size()) {
        if (_str[end_index] != _token) {
            ++end_index;
            continue;
        }
        else if (start_index == end_index) {
            ++end_index;
            start_index = end_index;
            continue;
        }
        str_len = end_index - start_index;
        result_list.EmplaceBack(_str, start_index, str_len);
        ++end_index;
        start_index = end_index;
    };
    if (start_index != end_index) {
        str_len = end_index - start_index;
        result_list.EmplaceBack(_str, start_index, str_len);
    }
    return result_list;
};

CORE_DLLAPI NODISCARD TList<ZWString> SplitToString(ZWStringView _str, const WChar _token) noexcept {
    ReturnType ret_val = kOK;
    SizeType start_index = 0;
    SizeType end_index = 0;
    SizeType str_len = 0;
    TList<ZWString> result_list;
    while (end_index != _str.Size()) {
        if (_str[end_index] != _token) {
            ++end_index;
            continue;
        }
        else if (start_index == end_index) {
            ++end_index;
            start_index = end_index;
            continue;
        }
        str_len = end_index - start_index;
        result_list.EmplaceBack(_str, start_index, str_len);
        ++end_index;
        start_index = end_index;
    };
    if (start_index != end_index) {
        str_len = end_index - start_index;
        result_list.EmplaceBack(_str, start_index, str_len);
    }
    return result_list;
};

CORE_DLLAPI NODISCARD TList<ZStringView> SplitToStringView(ZStringView _str, const Char _token) noexcept {
    ReturnType ret_val = kOK;
    SizeType start_index = 0;
    SizeType end_index = 0;
    SizeType str_len = 0;
    TList<ZStringView> result_list;
    while (end_index != _str.Size()) {
        if (_str[end_index] != _token) {
            ++end_index;
            continue;
        }
        else if (start_index == end_index) {
            ++end_index;
            start_index = end_index;
            continue;
        }
        str_len = end_index - start_index;
        result_list.EmplaceBack(_str, start_index, str_len);
        ++end_index;
        start_index = end_index;
    };
    if (start_index != end_index) {
        str_len = end_index - start_index;
        result_list.EmplaceBack(_str, start_index, str_len);
    }
    return result_list;
};

CORE_DLLAPI NODISCARD TList<ZWStringView> SplitToStringView(ZWStringView _str, const WChar _token) noexcept {
    ReturnType ret_val = kOK;
    SizeType start_index = 0;
    SizeType end_index = 0;
    SizeType str_len = 0;
    TList<ZWStringView> result_list;
    while (end_index != _str.Size()) {
        if (_str[end_index] != _token) {
            ++end_index;
            continue;
        }
        else if (start_index == end_index) {
            ++end_index;
            start_index = end_index;
            continue;
        }
        str_len = end_index - start_index;
        result_list.EmplaceBack(_str, start_index, str_len);
        ++end_index;
        start_index = end_index;
    };
    if (start_index != end_index) {
        str_len = end_index - start_index;
        result_list.EmplaceBack(_str, start_index, str_len);
    }
    return result_list;
};

}//string
}//zengine