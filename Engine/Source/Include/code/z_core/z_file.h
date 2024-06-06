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
    kZFileErrorCodeLinkError = kErrorCodeBasePCore,
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
class ZFile : public ZObject {
public:

    enum SeekType : Int32 {
        kZFileSeekTypeFileHead = SEEK_SET,
        kZFileSeekTypeCurrntPtr = SEEK_CUR,
        kZFileSeekTypeFileEnd = SEEK_END,

    };

    /*Read only.*/
    static constexpr CChar kOpenTypeRead[] = "r";
    /*Write only, will clear the file.*/
    static constexpr CChar kOpenTypeWrite[] = "w";
    /*Append at the end of the file.*/
    static constexpr CChar kOpenTypeAppend[] = "a";
    /*Read and write.*/
    static constexpr CChar kOpenTypeReadPlus[] = "r+";
    /*Read and write, will clear the file.*/
    static constexpr CChar kOpenTypeWritePlus[] = "w+";
    /*Read and write, starts at the end of the file.*/
    static constexpr CChar kOpenTypeAppendPlus[] = "a+";
    /*Read only.(binary file)*/
    static constexpr CChar kOpenTypeReadBin[] = "rb";
    /*Write only, will clear the file.(binary file)*/
    static constexpr CChar kOpenTypeWriteBin[] = "wb";
    /*Append at the end of the file.(binary file)*/
    static constexpr CChar kOpenTypeAppendBin[] = "ab";
    /*Read and write.(binary file)*/
    static constexpr CChar kOpenTypeReadPlusBin[] = "rb+";
    /*Read and write, will clear the file.(binary file)*/
    static constexpr CChar kOpenTypeWritePlusBin[] = "wb+";
    /*Read and write, starts at the end of the file.(binary file)*/
    static constexpr CChar kOpenTypeAppendPlusBin[] = "ab+";

    FORCEINLINE ZFile() noexcept : SuperType(), file_ptr_(nullptr) {}
    FORCEINLINE ~ZFile() noexcept { if (file_ptr_ != nullptr) { fclose(file_ptr_); } }

    /*
        Read binary out of the file.
    */
    CORE_DLLAPI NODISCARD ReturnType Read(Void* data_ptr, SizeType data_size) noexcept;
    /*
        Write binary in to the file.
    */
    CORE_DLLAPI NODISCARD ReturnType Write(Void* data_ptr, SizeType data_size) noexcept;
    /*
        Scans from the file.
    */
    CORE_DLLAPI NODISCARD ReturnType Scan(const CChar* format, ArgListType args) noexcept;
    /*
        Scans from the file.
    */
    CORE_DLLAPI NODISCARD ReturnType Scan(const CChar* format, ...) noexcept;
    /*
        Scans from the file.
    */
    CORE_DLLAPI NODISCARD ReturnType Scan(const TChar* format, ArgListType args) noexcept;
    /*
        Scans from the file.
    */
    CORE_DLLAPI NODISCARD ReturnType Scan(const TChar* format, ...) noexcept;
    /*
        Prints in the file.
    */
    CORE_DLLAPI NODISCARD ReturnType Print(const CChar* format, ArgListType args) noexcept;
    /*
        Prints in the file.
    */
    CORE_DLLAPI NODISCARD ReturnType Print(const CChar* format, ...) noexcept;
    /*
        Prints in the file.
    */
    CORE_DLLAPI NODISCARD ReturnType Print(const TChar* format, ArgListType args) noexcept;
    /*
        Prints in the file.
    */
    CORE_DLLAPI NODISCARD ReturnType Print(const TChar* format, ...) noexcept;

    /*
        If a file is opened.
    */
    FORCEINLINE NODISCARD Bool IfOpen() noexcept { return file_ptr_ != nullptr; }
    /*
        If the path exists.
    */
    CORE_DLLAPI NODISCARD Bool PathExist(const CChar* path_dir) noexcept;
    /*
        If the path exists.
    */
    CORE_DLLAPI NODISCARD Bool PathExist(const TChar* path_dir) noexcept;

    /*
        Creates the path if the path doesn't exist.
    */
    CORE_DLLAPI NODISCARD ReturnType CreatePath(const CChar* path_dir) noexcept;
    /*
        Creates the path if the path doesn't exist.
    */
    CORE_DLLAPI NODISCARD ReturnType CreatePath(const TChar* path_dir) noexcept;

    /*
        Opens the file, needs the path exist.
    */
    CORE_DLLAPI NODISCARD ReturnType Open(const CChar* file_dir, const CChar* open_type) noexcept;
    /*
        Opens the file, needs the path exist.
    */
    CORE_DLLAPI NODISCARD ReturnType Open(const TChar* file_dir,const TChar* open_type) noexcept;
    /*
        Opens the file safe, will create the path if the path doesn't exist.
    */
    CORE_DLLAPI NODISCARD ReturnType OpenSafe(const CChar* path_dir, const CChar* file_dir,
                                              const CChar* open_type) noexcept;
    /*
        Opens the file safe, will create the path if the path doesn't exist.
    */
    CORE_DLLAPI NODISCARD ReturnType OpenSafe(const TChar* path_dir, const TChar* file_dir,
                                              const TChar* open_type) noexcept;
    /*
        Close the current file.
    */
    CORE_DLLAPI NODISCARD ReturnType Close() noexcept;

    /*
        Sets the ptr where to read and write.
        Parameters:
        offset: The offset to the start place.
        seek_type: Where to start to seek,
    */
    CORE_DLLAPI NODISCARD ReturnType Seek(Int32 offset, SeekType seek_type) noexcept;
    /*
        Gets the ptr where to read and write.
        Parameters:
        pos_ptr: returns the pos of the current ptr.
    */
    CORE_DLLAPI NODISCARD ReturnType Tell(Int32* pos_ptr) noexcept;



protected:
    using SuperType = ZObject;

private:
    ZFile(const ZFile&) = delete;
    ZFile(ZFile&&) = delete;

    ZFile& operator=(const ZFile&) = delete;
    ZFile& operator=(ZFile&&) = delete;

    FILE* file_ptr_;
};

}//zengine

#endif // !Z_CORE_Z_FILE_H_
