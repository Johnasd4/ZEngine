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
#ifndef Z_CORE_MEMORY_POOL_T_MEMORY_POOL_THREAD_SAFE_BASE_H_
#define Z_CORE_MEMORY_POOL_T_MEMORY_POOL_THREAD_SAFE_BASE_H_

#include "internal/z_drive.h"
#include "z_cs_mutex.h"

namespace zengine{
namespace memory_pool {

/*
    Extend this class instead of using a mutex member. You can lock and unlock 
    it as the same as mutex. It will only work when kIsThreadSafe is true and
    do nothing when it's false.
    Template Parameters:
    - kIsThreadSafe: thread safe or not.  
*/
template<Bool kIsThreadSafe>
class TMemoryPoolThreadSafeBase {
protected:
    FORCEINLINE Void Lock() noexcept { if constexpr (kIsThreadSafe) { mutex_.Lock(); } }
    FORCEINLINE Void Unlock() noexcept { if constexpr (kIsThreadSafe) { mutex_.Unlock(); } }

private:
    ZCSMutex mutex_;
};
template<>
class TMemoryPoolThreadSafeBase<false> {
protected:
    FORCEINLINE Void Lock() noexcept {}
    FORCEINLINE Void Unlock() noexcept {}
};

}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_T_MEMORY_POOL_THREAD_SAFE_BASE_H_
