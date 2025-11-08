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

#include "drive/d_function.h"

#include <thread>

#include "drive/d_constant.h"
#include "m_log.h"

namespace zengine {

CORE_DLLAPI NODISCARD Void SleepSec(TimeType _time) noexcept {
    std::this_thread::sleep_for(std::chrono::seconds(_time));
}

CORE_DLLAPI NODISCARD Void SleepMs(TimeType _time) noexcept {
    std::this_thread::sleep_for(std::chrono::milliseconds(_time));
}

CORE_DLLAPI NODISCARD Void Exit(ReturnType _ret_val) noexcept {
    Z_LOG_FINISH(L"Program exited with code %d", _ret_val);
    SleepMs(kTimeBeforeProgramExit);
    exit(_ret_val);
}

}//zengine
