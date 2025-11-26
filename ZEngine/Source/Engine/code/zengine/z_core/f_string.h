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
#pragma once

#include "drive.h"

#include "t_list.h"
#include "z_string.h"
#include "z_string_view.h"

namespace zengine {
namespace error_code {
enum FStringErrorCode : ReturnType {
    kFStringErrorCode_LinkError = kErrorCodeBase_ZString,
    kFStringErrorCode_SystemError,
    kFStringErrorCode_NullptrParam,
    kFStringErrorCode_ParamOutOfRange,
    kFStringErrorCode_InvalidString,
    kFStringErrorCode_StringToNumberCanNotTransform,
    kFStringErrorCode_StringToNumberOutOfRange,
};
}//error_code
}//zengine

namespace zengine {
namespace string {

/*
    Translate narrow string to wide string, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ZWString String2WString(const Char* _str) noexcept;

/*
    Translate wide string to narrow string, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ZString WString2String(const WChar* _str) noexcept;

/*
    Translate narrow string to Int32, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ReturnType String2Int32(const Char* _str, Int32* _ans_ptr) noexcept;
/*
    Translate narrow string to Int64, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ReturnType String2Int64(const Char* _str, Int64* _ans_ptr) noexcept;
/*
    Translate narrow string to UInt32, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ReturnType String2UInt32(const Char* _str, UInt32* _ans_ptr) noexcept;
/*
    Translate narrow string to UInt64, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ReturnType String2UInt64(const Char* _str, UInt64* _ans_ptr) noexcept;
/*
    Translate narrow string to Float32, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ReturnType String2Float32(const Char* _str, Float32* _ans_ptr) noexcept;
/*
    Translate narrow string to Float64, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ReturnType String2Float64(const Char* _str, Float64* _ans_ptr) noexcept;

/*
    Translate wide string to Int32, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ReturnType WString2Int32(const WChar* _str, Int32* _ans_ptr) noexcept;
/*
    Translate wide string to Int64, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ReturnType WString2Int64(const WChar* _str, Int64* _ans_ptr) noexcept;
/*
    Translate wide string to UInt32, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ReturnType WString2UInt32(const WChar* _str, UInt32* _ans_ptr) noexcept;
/*
    Translate wide string to UInt64, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ReturnType WString2UInt64(const WChar* _str, UInt64* _ans_ptr) noexcept;
/*
    Translate wide string to Float32, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ReturnType WString2Float32(const WChar* _str, Float32* _ans_ptr) noexcept;
/*
    Translate wide string to Float64, if the string is invalid, will return "".
*/
CORE_DLLAPI NODISCARD ReturnType WString2Float64(const WChar* _str, Float64* _ans_ptr) noexcept;

/*
    Generate narrow string.
*/
CORE_DLLAPI NODISCARD ZString GenerateString(const Char* _format, ...) noexcept;

/*
    Generate wide string.
*/
CORE_DLLAPI NODISCARD ZWString GenerateWString(const WChar* _format, ...) noexcept;

/*
    Splits the given string view to string.
*/
CORE_DLLAPI NODISCARD TList<ZString> SplitToString(ZStringView _str, const Char _token) noexcept;

/*
    Splits the given string view to string.
*/
CORE_DLLAPI NODISCARD TList<ZWString> SplitToString(ZWStringView _str, const WChar _token) noexcept;

/*
    Splits the given string view to string view.
*/
CORE_DLLAPI NODISCARD TList<ZStringView> SplitToStringView(ZStringView _str, const Char _token) noexcept;

/*
    Splits the given string view to string view.
*/
CORE_DLLAPI NODISCARD TList<ZWStringView> SplitToStringView(ZWStringView _str, const WChar _token) noexcept;

}//string
}//zengine
