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
#ifndef Z_CORE_Z_FILE_H_
#define Z_CORE_Z_FILE_H_

#include "internal/z_drive.h"

#include "z_mutex.h"
#include "z_object.h"

namespace zengine {

namespace error_code {

enum ZFileErrorCode : ReturnType {
    kZFileErrorCodeLinkError = kErrorCodeBaseZFile,
    kZFileErrorCodeOpenFileFailed,
    kZFileErrorCodeCloseFileFailed,
    kZFileErrorCodeNoFileOpened,
    kZFileErrorCodeOtherFileOpened,
    kZFileErrorCodeCreatePathFailed,
    kZFileErrorCodeReadFailed,
    kZFileErrorCodeWriteFailed
};

}//error_code

/*
    File class.
*/
class CORE_DLLAPI ZFile : public ZObject {
public:

    enum SeekType_ : Int32 {
        kZFileSeekTypeFileHead = SEEK_SET,
        kZFileSeekTypeCurrntPtr = SEEK_CUR,
        kZFileSeekTypeFileEnd = SEEK_END,

    };

    /*Read only.*/
    static constexpr Char kOpenTypeRead[] = "r";
    /*Write only, will clear the file.*/
    static constexpr Char kOpenTypeWrite[] = "w";
    /*Append at the end of the file.*/
    static constexpr Char kOpenTypeAppend[] = "a";
    /*Read and write.*/
    static constexpr Char kOpenTypeReadPlus[] = "r+";
    /*Read and write, will clear the file.*/
    static constexpr Char kOpenTypeWritePlus[] = "w+";
    /*Read and write, starts at the end of the file.*/
    static constexpr Char kOpenTypeAppendPlus[] = "a+";
    /*Read only.(binary file)*/
    static constexpr Char kOpenTypeReadBin[] = "rb";
    /*Write only, will clear the file.(binary file)*/
    static constexpr Char kOpenTypeWriteBin[] = "wb";
    /*Append at the end of the file.(binary file)*/
    static constexpr Char kOpenTypeAppendBin[] = "ab";
    /*Read and write.(binary file)*/
    static constexpr Char kOpenTypeReadPlusBin[] = "rb+";
    /*Read and write, will clear the file.(binary file)*/
    static constexpr Char kOpenTypeWritePlusBin[] = "wb+";
    /*Read and write, starts at the end of the file.(binary file)*/
    static constexpr Char kOpenTypeAppendPlusBin[] = "ab+";

    /*Read only.*/
    static constexpr WChar kOpenTypeReadW[] = L"r";
    /*Write only, will clear the file.*/
    static constexpr WChar kOpenTypeWriteW[] = L"w";
    /*Append at the end of the file.*/
    static constexpr WChar kOpenTypeAppendW[] = L"a";
    /*Read and write.*/
    static constexpr WChar kOpenTypeReadPlusW[] = L"r+";
    /*Read and write, will clear the file.*/
    static constexpr WChar kOpenTypeWritePlusW[] = L"w+";
    /*Read and write, starts at the end of the file.*/
    static constexpr WChar kOpenTypeAppendPlusW[] = L"a+";
    /*Read only.(binary file)*/
    static constexpr WChar kOpenTypeReadBinW[] = L"rb";
    /*Write only, will clear the file.(binary file)*/
    static constexpr WChar kOpenTypeWriteBinW[] = L"wb";
    /*Append at the end of the file.(binary file)*/
    static constexpr WChar kOpenTypeAppendBinW[] = L"ab";
    /*Read and write.(binary file)*/
    static constexpr WChar kOpenTypeReadPlusBinW[] = L"rb+";
    /*Read and write, will clear the file.(binary file)*/
    static constexpr WChar kOpenTypeWritePlusBinW[] = L"wb+";
    /*Read and write, starts at the end of the file.(binary file)*/
    static constexpr WChar kOpenTypeAppendPlusBinW[] = L"ab+";
    /*The max length of the file name.*/
    static constexpr IndexType kFileNameLength = 512;

    FORCEINLINE ZFile() noexcept : SuperType_(), file_ptr_(nullptr) {}
    FORCEINLINE ZFile(ZFile&& _file) noexcept : SuperType_(), file_ptr_(_file.file_ptr_) {}

    FORCEINLINE ~ZFile() noexcept { if (file_ptr_ != nullptr) { fclose(file_ptr_); } }

    FORCEINLINE ZFile& operator=(ZFile&& _file) noexcept {
        file_ptr_ = _file.file_ptr_;
        return *this;
    }

    /*
        Read binary out of the file.
    */
    NODISCARD ReturnType Read(Void* _data_ptr, SizeType _data_size) noexcept;
    /*
        Write binary in to the file.
    */
    NODISCARD ReturnType Write(Void* _data_ptr, SizeType _data_size) noexcept;
    /*
        Scans from the file.
    */
    NODISCARD ReturnType Scan(const Char* _format, ArgListType _args) noexcept;
    /*
        Scans from the file.
    */
    NODISCARD ReturnType Scan(const Char* _format, ...) noexcept;
    /*
        Scans from the file.
    */
    NODISCARD ReturnType Scan(const WChar* _format, ArgListType _args) noexcept;
    /*
        Scans from the file.
    */
    NODISCARD ReturnType Scan(const WChar* _format, ...) noexcept;
    /*
        Prints in the file.
    */
    NODISCARD ReturnType Print(const Char* _format, ArgListType _args) noexcept;
    /*
        Prints in the file.
    */
    NODISCARD ReturnType Print(const Char* _format, ...) noexcept;
    /*
        Prints in the file.
    */
    NODISCARD ReturnType Print(const WChar* _format, ArgListType _args) noexcept;
    /*
        Prints in the file.
    */
    NODISCARD ReturnType Print(const WChar* _format, ...) noexcept;

    /*
        If a file is opened.
    */
    FORCEINLINE NODISCARD Bool IfOpen() noexcept { return file_ptr_ != nullptr; }
    /*
        If the path exists.
    */
    NODISCARD Bool PathExist(const Char* _path_dir) noexcept;
    /*
        If the path exists.
    */
    NODISCARD Bool PathExist(const WChar* _path_dir) noexcept;

    /*
        Creates the path if the path doesn't exist.
    */
    NODISCARD ReturnType CreatePath(const Char* _path_dir) noexcept;
    /*
        Creates the path if the path doesn't exist.
    */
    NODISCARD ReturnType CreatePath(const WChar* _path_dir) noexcept;

    /*
        Opens the file, needs the path exist.
    */
    NODISCARD ReturnType Open(const Char* _file_dir, const Char* _open_type) noexcept;
    /*
        Opens the file, needs the path exist.
    */
    NODISCARD ReturnType Open(const WChar* _file_dir,const WChar* _open_type) noexcept;
    /*
        Opens the file safe, will create the path if the path doesn't exist.
    */
    NODISCARD ReturnType OpenSafe(const Char* _path_dir, const Char* _file_dir, const Char* _open_type) noexcept;
    /*
        Opens the file safe, will create the path if the path doesn't exist.
    */
    NODISCARD ReturnType OpenSafe(const WChar* _path_dir, const WChar* _file_dir, const WChar* _open_type) noexcept;
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



protected:
    using SuperType_ = ZObject;

private:
    ZFile(const ZFile&) = delete;

    ZFile& operator=(const ZFile&) = delete;

    FILE* file_ptr_;
};

}//zengine

#endif // !Z_CORE_Z_FILE_H_
