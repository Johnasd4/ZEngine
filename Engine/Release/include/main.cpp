/*
    Copyright (c) YuLin Zhu (朱雨林)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (朱雨林)
    Contact: 1152325286@qq.com
*/
#define PROJECT_NAME_C_STRING "Include"
#define PROJECT_NAME_T_STRING L"Include"

#include "z_engine.h"

#include <list>
#include <condition_variable>

using namespace zengine;
using namespace std;



int main() {
    Z_LOG_ERROR(1, 2, "TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    Z_LOG_ERROR(1, 2, "TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    Z_LOG_TRACE(L"TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    Z_LOG_MESSAGE(L"Message...");
    Z_LOG_START(L"Start...");
    Z_LOG_PROCESS(L"Process 1...");
    Z_LOG_PROCESS(L"Process 2...");
    Z_LOG_PROCESS(L"Process 3...");
    Z_LOG_FINISH(L"Finish...");
    Z_LOG_SUCCESS(L"Success...");
    Z_LOG_FAILURE(L"Failure...");
    ZMutex mutex;
    TUniqueLock<ZMutex> lock(mutex);
    //std::thread prod(producer);
    //std::thread cons(consumer);
    //prod.join();
    //cons.join();
    Sleep(100);
    TList<Int32> forward_list;
    for (Int32 i = 0; i < 10; ++i) {
        forward_list.PushBack(i);
    }
    while (!forward_list.Empty()) {
        cout << forward_list.Front() << endl;
        forward_list.PopFront();
    }
    bool qqq = forward_list == forward_list;
    return 0;
}
