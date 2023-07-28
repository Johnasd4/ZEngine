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
#ifndef Z_CORE_Z_MUTEX_H_
#define Z_CORE_Z_MUTEX_H_

#include "internal/drive.h"

namespace zengine {

/*
    A simple mutex.
*/
class ZMutex {
public:
    FORCEINLINE ZMutex() :handle_(CreateMutex(nullptr, FALSE, nullptr)) {}
    FORCEINLINE ~ZMutex() { CloseHandle(handle_); }

    FORCEINLINE Void Lock() { WaitForSingleObject(handle_, INFINITE); }   
    FORCEINLINE Void Unlock() { ReleaseMutex(handle_); }

private:
    ZMutex(const ZMutex&) = delete;
    ZMutex(ZMutex&&) = delete;

    ZMutex& operator=(const ZMutex&) = delete;
    ZMutex& operator=(ZMutex&&) = delete;

    Handle handle_;
};

}//zengine

#endif // !Z_CORE_Z_MUTEX_H_
