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

#include "internal/drive.h"

#include "z_mutex.h"
#include "z_object.h"

namespace zengine {

/*
    File class.
*/
class ZFile : public ZObject {
public:
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

    FORCEINLINE ZFile() noexcept : SuperType(), file_ptr(nullptr) {}
    FORCEINLINE ~ZFile() noexcept { if (file_ptr != nullptr) { fclose(file_ptr); } }

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
