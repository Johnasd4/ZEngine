/*
    Copyright (c) YuLin Zhu (ÖìÓêÁÖ)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (ÖìÓêÁÖ)
    Contact: 1152325286@qq.com
*/
#ifndef Z_CORE_Z_LOCK_GUARD_H_
#define Z_CORE_Z_LOCK_GUARD_H_

#include "internal/drive.h"

#include "z_mutex.h"
#include "z_object.h"

namespace zengine {

/*
    Locks the mutex when constructed and release when destructed.
*/
class ZLockGuard : public ZObject {
public:
    FORCEINLINE ZLockGuard(ZMutex* mutex_ptr) noexcept : SuperType(), mutex_ptr_(mutex_ptr) { mutex_ptr_->Lock(); }
    FORCEINLINE ~ZLockGuard() noexcept { mutex_ptr_->Unlock(); }

protected:
    using SuperType = ZObject;

private:
    ZLockGuard() = delete;
    ZLockGuard(const ZLockGuard&) = delete;
    ZLockGuard(ZLockGuard&&) = delete;

    ZLockGuard& operator=(const ZLockGuard&) = delete;
    ZLockGuard& operator=(ZLockGuard&&) = delete;

    ZMutex* mutex_ptr_;
};

}//zengine

#endif // !Z_CORE_Z_LOCK_GUARD_H_
