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
    static constexpr WChar kOpenTypeRead[] = L"r";
    /*Write only, will clear the file.*/
    static constexpr WChar kOpenTypeWrite[] = L"w";
    /*Append at the end of the file.*/
    static constexpr WChar kOpenTypeAppend[] = L"a";
    /*Read and write.*/
    static constexpr WChar kOpenTypeReadPlus[] = L"r+";
    /*Read and write, will clear the file.*/
    static constexpr WChar kOpenTypeWritePlus[] = L"w+";
    /*Read and write, starts at the end of the file.*/
    static constexpr WChar kOpenTypeAppendPlus[] = L"a+";
    /*Read only.(binary file)*/
    static constexpr WChar kOpenTypeReadBin[] = L"rb";
    /*Write only, will clear the file.(binary file)*/
    static constexpr WChar kOpenTypeWriteBin[] = L"wb";
    /*Append at the end of the file.(binary file)*/
    static constexpr WChar kOpenTypeAppendBin[] = L"ab";
    /*Read and write.(binary file)*/
    static constexpr WChar kOpenTypeReadPlusBin[] = L"rb+";
    /*Read and write, will clear the file.(binary file)*/
    static constexpr WChar kOpenTypeWritePlusBin[] = L"wb+";
    /*Read and write, starts at the end of the file.(binary file)*/
    static constexpr WChar kOpenTypeAppendPlusBin[] = L"ab+";
    /*The max length of the file name.*/
    static constexpr IndexType kFileNameLength = 512;

    FORCEINLINE ZFile() noexcept : SuperType_(), file_ptr_(nullptr) {}
    FORCEINLINE ZFile(ZFile&& _file) noexcept : SuperType_(std::forward<ZFile>(_file)) { 
        MoveP(std::forward<ZFile>(_file)); 
    }

    FORCEINLINE ~ZFile() noexcept { if (file_ptr_ != nullptr) { fclose(file_ptr_); } }

    FORCEINLINE ZFile& operator=(ZFile&& _file) noexcept {
        SuperType_::operator=(std::forward<ZFile>(_file));
        MoveP(std::forward<ZFile>(_file));
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
        Opens the file, needs the path exist.
    */
    NODISCARD ReturnType Open(const WChar* _file_dir,const WChar* _open_type) noexcept;
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

    FORCEINLINE Void MoveP(ZFile&& _file) noexcept {
        file_ptr_ = _file.file_ptr_;
        _file.file_ptr_ = nullptr;
    }

    FILE* file_ptr_;
};

}//zengine

#endif // !Z_CORE_Z_FILE_H_