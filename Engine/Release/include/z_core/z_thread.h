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

#include "t_tuple.h"
#include "z_object.h"

namespace zengine {

/*
    Thread class.
*/
class CORE_DLLAPI ZThread : public ZObject {
public:
    ZThread() noexcept;
    ZThread(ZThread&& thread) noexcept;

    template <typename Function, typename... ArgsType>
    ZThread(Function&& func, ArgsType&&... args) noexcept {
        using ParamsType = TTuple<Function, TTuple<ArgsType...>>;
        ParamsType* params_ptr = new ParamsType(std::forward<Function>(func), 
                                                tuple::MakeTuple(std::forward<ArgsType>(args)...));
        auto thread_func = [](Void* params) -> UInt32 {
            ParamsType temp_params(*(ParamsType*)params);
            delete (ParamsType*)params;
            tuple::Apply(temp_params.Get<0>(), std::move(temp_params.Get<1>()));
            return 0; 
        };
        handle_ = (Handle)_beginthreadex(NULL,
                                         0,
                                         thread_func,
                                         (Void*)params_ptr,
                                         0,
                                         &id_);
    }

    ~ZThread() noexcept;

    ZThread& operator=(ZThread&& thread) noexcept;

    NODISCARD FORCEINLINE UInt32 ID() const noexcept { return id_; }
    NODISCARD FORCEINLINE Bool Joinable() noexcept { return WaitForSingleObject(handle_, 0) == WAIT_TIMEOUT; }

    FORCEINLINE Void Join() noexcept { WaitForSingleObject(handle_, INFINITE); }
    Void Detach() noexcept;
    Void Swap(ZThread& thread) noexcept;

protected:
    using SuperType = ZObject;

private:
    UInt32 id_;
    Handle handle_;
};

}//zengine

#endif // !Z_CORE_Z_THREAD_H_
