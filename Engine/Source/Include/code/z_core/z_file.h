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

/*
    File class.
*/
class ZFile : public ZObject {
public:

    /*
        File error code.
    */
    enum ErrorCode : ReturnType {
        kErrCodeZFileOpenFileFailed = kErrCodeBasePCore,
        kErrCodeZFileCloseFileFailed,
        kErrCodeZFileFilePtrNull,
    };

    //Read only.
    static constexpr CChar kOpenTypeRead[] = "r";
    //Write only, will clear the file.
    static constexpr CChar kOpenTypeWrite[] = "w";
    //Append at the end of the file.
    static constexpr CChar kOpenTypeAppend[] = "a";
    //Read and write.
    static constexpr CChar kOpenTypeReadPlus[] = "r+";
    //Read and write, will clear the file.
    static constexpr CChar kOpenTypeWritePlus[] = "w+";
    //Read and write, starts at the end of the file.
    static constexpr CChar kOpenTypeAppendPlus[] = "a+";
    //Read only.(binary file)
    static constexpr CChar kOpenTypeReadBin[] = "rb";
    //Write only, will clear the file.(binary file)
    static constexpr CChar kOpenTypeWriteBin[] = "wb";
    //Append at the end of the file.(binary file)
    static constexpr CChar kOpenTypeAppendBin[] = "ab";
    //Read and write.(binary file)
    static constexpr CChar kOpenTypeReadPlusBin[] = "rb+";
    //Read and write, will clear the file.(binary file)
    static constexpr CChar kOpenTypeWritePlusBin[] = "wb+";
    //Read and write, starts at the end of the file.(binary file)
    static constexpr CChar kOpenTypeAppendPlusBin[] = "ab+";

    FORCEINLINE ZFile() noexcept : SuperType(), file_ptr(nullptr) {}
    FORCEINLINE ~ZFile() noexcept { if (file_ptr != nullptr) { fclose(file_ptr); } }

    CORE_DLLAPI ZFile& operator<<(const CChar* path_dir) noexcept;
    CORE_DLLAPI ZFile& operator>>(const CChar* path_dir) noexcept;

    CORE_DLLAPI NODISCARD Bool Read() noexcept;
    CORE_DLLAPI NODISCARD Bool ReadBin() noexcept;
    CORE_DLLAPI NODISCARD Bool Write() noexcept;
    CORE_DLLAPI NODISCARD Bool WriteBin() noexcept;

    CORE_DLLAPI NODISCARD Bool SetPtr() noexcept;
    CORE_DLLAPI NODISCARD Bool GetPtr() noexcept;

    CORE_DLLAPI NODISCARD Bool CheckPath(const CChar* path_dir) noexcept;
    CORE_DLLAPI NODISCARD Bool CreatePath(const CChar* path_dir) noexcept;
    CORE_DLLAPI NODISCARD ReturnType Open(const CChar* file_dir,const CChar* open_type) noexcept;
    CORE_DLLAPI NODISCARD Bool OpenSafe(const CChar* path_dir, const CChar* file_dir, const CChar* open_type) noexcept;
    CORE_DLLAPI NODISCARD ReturnType Close() noexcept;

    FORCEINLINE NODISCARD Bool IfOpen() noexcept { return file_ptr != nullptr; }

protected:
    using SuperType = ZObject;

private:
    ZFile(const ZFile&) = delete;
    ZFile(ZFile&&) = delete;

    ZFile& operator=(const ZFile&) = delete;
    ZFile& operator=(ZFile&&) = delete;

    FILE* file_ptr;
};

}//zengine

#endif // !Z_CORE_Z_FILE_H_
