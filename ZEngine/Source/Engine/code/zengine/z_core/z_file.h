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

#include "z_object.h"

namespace zengine {
namespace error_code {
enum ZFileErrorCodeEnum : ReturnType {
    kZFileErrorCode_LinkError = kErrorCodeBase_ZFile,
    kZFileErrorCode_SystemError,
    kZFileErrorCode_NullptrParam,
    kZFileErrorCode_ParamOutOfRange,
    kZFileErrorCode_OpenFileFailed,
    kZFileErrorCode_CloseFileFailed,
    kZFileErrorCode_NoFileOpened,
    kZFileErrorCode_OtherFileOpened,
    kZFileErrorCode_CreatePathFailed,
    kZFileErrorCode_ReadFailed,
    kZFileErrorCode_WriteFailed
};
}//error_code
}//zengine

namespace zengine {

/*
    File class.
*/
class CORE_DLLAPI ZFile : public ZObject {
public:

    enum class SeekType_ : Int32 {
        kFileHead = SEEK_SET,
        kCurrntPtr = SEEK_CUR,
        kFileEnd = SEEK_END
    };

    /*Read only.*/
    static inline constexpr Char kOpenTypeRead[] = "r";
    /*Write only, will clear the file.*/
    static inline constexpr Char kOpenTypeWrite[] = "w";
    /*Append at the end of the file.*/
    static inline constexpr Char kOpenTypeAppend[] = "a";
    /*Read and write.*/
    static inline constexpr Char kOpenTypeReadPlus[] = "r+";
    /*Read and write, will clear the file.*/
    static inline constexpr Char kOpenTypeWritePlus[] = "w+";
    /*Read and write, starts at the end of the file.*/
    static inline constexpr Char kOpenTypeAppendPlus[] = "a+";
    /*Read only.(binary file)*/
    static inline constexpr Char kOpenTypeReadBin[] = "rb";
    /*Write only, will clear the file.(binary file)*/
    static inline constexpr Char kOpenTypeWriteBin[] = "wb";
    /*Append at the end of the file.(binary file)*/
    static inline constexpr Char kOpenTypeAppendBin[] = "ab";
    /*Read and write.(binary file)*/
    static inline constexpr Char kOpenTypeReadPlusBin[] = "rb+";
    /*Read and write, will clear the file.(binary file)*/
    static inline constexpr Char kOpenTypeWritePlusBin[] = "wb+";
    /*Read and write, starts at the end of the file.(binary file)*/
    static inline constexpr Char kOpenTypeAppendPlusBin[] = "ab+";

    ZFile() noexcept;
    ZFile(ZFile&& _file) noexcept;

    ~ZFile() noexcept;

    ZFile& operator=(ZFile&& _file) noexcept;

    /*
        Read binary out of the file.
    */
    NODISCARD ReturnType Read(Void* _data_ptr, SizeType _data_size) noexcept;
    /*
        Write binary in to the file.
    */
    NODISCARD ReturnType Write(const Void* _data_ptr, SizeType _data_size) noexcept;
    /*
        Scans from the file.
    */
    NODISCARD ReturnType Scan(const Char* _format, ArgListType _args) noexcept;
    /*
        Scans from the file.
    */
    NODISCARD ReturnType Scan(const Char* _format, ...) noexcept;
    /*
        Prints in the file.
    */
    NODISCARD ReturnType Print(const Char* _format, ArgListType _args) noexcept;
    /*
        Prints in the file.
    */
    NODISCARD ReturnType Print(const Char* _format, ...) noexcept;

    /*
        If a file is opened.
    */
    FORCEINLINE NODISCARD Bool IfOpen() noexcept { return file_ptr_ != nullptr; }
    /*
        Opens the file, needs the path exist.
    */
    NODISCARD ReturnType Open(const Char* _file_dir, const Char* _open_type) noexcept;
    /*
        Opens the file safe, will create the path if the path doesn't exist.
    */
    NODISCARD ReturnType OpenSafe(const Char* _file_dir, const Char* _open_type) noexcept;
    /*
        Close the current file.
    */
    NODISCARD ReturnType Close() noexcept;

    /*
        Sets the ptr where to read and write.
        Parameters:
        offset: The offset to the start place.
        seek_type: Where to start to seek,
    */
    NODISCARD ReturnType Seek(Int32 _offset, SeekType_ _seek_type) noexcept;
    /*
        Gets the ptr where to read and write.
        Parameters:
        pos_ptr: returns the pos of the current ptr.
    */
    NODISCARD ReturnType Tell(Int32* _pos_ptr) noexcept;

    /*
        Returns the size of the file.
    */
    NODISCARD Int32 Size() noexcept;

    /*
        Flush the current file cache.
    */
    NODISCARD Void Flush() noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZFile(const ZFile&) = delete;
    ZFile& operator=(const ZFile&) = delete;

    Void MoveP(ZFile&& _file) noexcept;

    FILE* file_ptr_;
};

}//zengine