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
#define CORE_DLLFILE

#include <thread>

#include "z_thread.h"

namespace zengine {

ZThread::ZThread() noexcept : SuperType_(), id_(NULL), handle_(nullptr) {}

ZThread::ZThread(ZThread&& _thread) noexcept : SuperType_(std::forward<ZThread>(_thread)) {
    MoveP(std::forward<ZThread>(_thread));
}

ZThread::~ZThread() noexcept {}

ZThread& ZThread::operator=(ZThread&& _thread) noexcept {
    SuperType_::operator=(std::forward<ZThread>(_thread));
    MoveP(std::forward<ZThread>(_thread));
    return *this;
}

Void ZThread::Detach() noexcept { 
    CloseHandle(handle_); 
    id_ = NULL;
    handle_ = nullptr;
}

Void ZThread::Swap(ZThread& _thread) noexcept { 
    UInt32 temp_id = id_;
    Handle temp_handle_ = handle_;
    id_ = _thread.id_;
    handle_ = _thread.handle_;
    _thread.id_ = id_;
    _thread.handle_ = handle_;
}

Void ZThread::MoveP(ZThread&& _thread) noexcept {
    id_ = _thread.id_;
    handle_ = _thread.handle_;
    _thread.id_ = NULL;
    _thread.handle_ = nullptr;
}

CORE_DLLAPI NODISCARD Void SleepSec(TimeType _time) noexcept {
    std::this_thread::sleep_for(std::chrono::seconds(_time));
}

CORE_DLLAPI NODISCARD Void SleepMs(TimeType _time) noexcept {
    std::this_thread::sleep_for(std::chrono::milliseconds(_time));
}

}//zengine
