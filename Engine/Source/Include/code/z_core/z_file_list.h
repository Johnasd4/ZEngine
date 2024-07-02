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
#ifndef Z_CORE_Z_FILE_LIST_H_
#define Z_CORE_Z_FILE_LIST_H_

#include "internal/z_drive.h"

#include "t_list.h"
#include "z_object.h"
#include "z_string.h"

namespace zengine {

namespace error_code {

enum ZFileListErrorCode : ReturnType {
    kZFileListErrorCodeLinkError = kErrorCodeBaseZFileList
};

}//error_code

/*
    File class.
*/
class CORE_DLLAPI ZFileList : public ZObject {
public:

    FORCEINLINE ZFileList() noexcept : SuperType_(), file_ptr_(nullptr) {}
    FORCEINLINE ZFileList(ZFileList&& _file) noexcept : SuperType_(), file_ptr_(_file.file_ptr_) {}

    FORCEINLINE ~ZFileList() noexcept { if (file_ptr_ != nullptr) { fclose(file_ptr_); } }

    FORCEINLINE ZFileList& operator=(ZFileList&& _file) noexcept {
        file_ptr_ = _file.file_ptr_;
        return *this;
    }

    NODISCARD ReturnType Tell(Int32* _pos_ptr) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZFileList(const ZFileList&) = delete;

    ZFileList& operator=(const ZFileList&) = delete;

    TList<ZWString> file_list_;
};

}//zengine

#endif // !Z_CORE_Z_FILE_LIST_H_
