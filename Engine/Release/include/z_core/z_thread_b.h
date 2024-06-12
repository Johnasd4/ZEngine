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
#ifndef Z_CORE_Z_THREAD_H_
#define Z_CORE_Z_THREAD_H_

#pragma warning(disable : 26439)

#include "internal/z_drive.h"

#include <thread>

#include "z_object.h"

namespace zengine {

/*
    Thread class.
*/
class ZThread : public ZObject {
public:
    using STDThread = std::thread;

    FORCEINLINE ZThread() noexcept : SuperType(), thread_() {}
    FORCEINLINE ZThread(ZThread&& thread) noexcept : thread_(std::move(thread.thread_)) {}

    template <typename Function, typename... ArgsType>
    FORCEINLINE ZThread(Function&& func, ArgsType&&... args) noexcept : 
        thread_(std::forward<Function>(func), std::forward<ArgsType>(args)...) {}

    FORCEINLINE ~ZThread() noexcept {}

    FORCEINLINE ZThread& operator=(ZThread&& thread) noexcept {
        thread_.operator=(std::move(thread.thread_));
        return *this;
    }

    NODISCARD FORCEINLINE UInt32 GetID() noexcept { 
        auto id = thread_.get_id();
        return *(UInt32*)&id; 
    }
    NODISCARD FORCEINLINE Bool Joinable() noexcept { return thread_.joinable(); }

    FORCEINLINE Void Join() noexcept { thread_.join(); }
    FORCEINLINE Void Detach() noexcept { thread_.detach(); }
    FORCEINLINE Void Swap(ZThread& thread) noexcept { thread_.swap(thread.thread_); }

protected:
    using SuperType = ZObject;

private:
    std::thread thread_;
};

}//zengine

#endif // !Z_CORE_Z_THREAD_H_
