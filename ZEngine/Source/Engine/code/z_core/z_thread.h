/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "internal/z_drive.h"

#include <process.h>

#include "t_tuple.h"
#include "z_object.h"

namespace zengine {

/*
    Thread class.
*/
class CORE_DLLAPI ZThread : public ZObject {
public:
    using ThreadIDType_ = UInt32;

    NODISCARD FORCEINLINE static ThreadIDType_ GetCurrnetThreadID() noexcept { return GetCurrentThreadId(); }

    ZThread() noexcept;
    ZThread(ZThread&& _thread) noexcept;

    template <typename _Function, typename... _ArgsType>
    ZThread(_Function&& _func, _ArgsType&&... _args) noexcept : SuperType_() {
        using ParamsType = TTuple<_Function, TTuple<_ArgsType...>>;
        ParamsType* params_ptr = new ParamsType(
            std::forward<_Function>(_func), tuple::MakeTuple(std::forward<_ArgsType>(_args)...));
        auto thread_func = [](Void* _params_ptr) -> UInt32 {
            tuple::Apply(((ParamsType*)_params_ptr)->Get<0>(), std::move(((ParamsType*)_params_ptr)->Get<1>()));
            delete (ParamsType*)_params_ptr;
            return 0;
        };
        handle_ = (Handle)_beginthreadex(
            NULL,
            0,
            thread_func,
            (Void*)params_ptr,
            0,
            &id_);
    }

    ~ZThread() noexcept;

    ZThread& operator=(ZThread&& _thread) noexcept;

    NODISCARD FORCEINLINE constexpr Bool operator==(const ZThread& _thread) noexcept {
        return id_ == _thread.id_;
    }
    NODISCARD FORCEINLINE constexpr Bool operator!=(const ZThread& _thread) noexcept {
        return id_ != _thread.id_;
    }
    NODISCARD FORCEINLINE constexpr Bool operator>(const ZThread& _thread) noexcept {
        return id_ > _thread.id_;
    }
    NODISCARD FORCEINLINE constexpr Bool operator>=(const ZThread& _thread) noexcept {
        return id_ >= _thread.id_;
    }
    NODISCARD FORCEINLINE constexpr Bool operator<(const ZThread& _thread) noexcept {
        return id_ < _thread.id_;
    }
    NODISCARD FORCEINLINE constexpr Bool operator<=(const ZThread& _thread) noexcept {
        return id_ <= _thread.id_;
    }

    NODISCARD FORCEINLINE ThreadIDType_ ID() const noexcept { return id_; }
    NODISCARD FORCEINLINE Bool Joinable() noexcept { return WaitForSingleObject(handle_, 0) == WAIT_TIMEOUT; }

    FORCEINLINE Void Join() noexcept { WaitForSingleObject(handle_, INFINITE); }
    Void Detach() noexcept;
    Void Swap(ZThread& _thread) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZThread(const ZThread&) = delete;
    ZThread& operator=(const ZThread&) = delete;

    Void MoveP(ZThread&& _thread) noexcept;

    ThreadIDType_ id_;
    Handle handle_;
};

}//zengine