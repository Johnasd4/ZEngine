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

#include <functional>

using namespace zengine;
using namespace std;

Int32 test_func(Int32 &test) {
    test = test + 1;
    cout << test << endl;
    return test;
}



int main() {
    //Z_LOG_ERROR(1, 2, "TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    //Z_LOG_ERROR(1, 2, "TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    //Z_LOG_TRACE(L"TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    //Z_LOG_MESSAGE(L"Message...");
    //Z_LOG_START(L"Start...");
    //Z_LOG_PROCESS(L"Process 1...");
    //Z_LOG_PROCESS(L"Process 2...");
    //Z_LOG_PROCESS(L"Process 3...");
    //Z_LOG_FINISH(L"Finish...");
    //Z_LOG_SUCCESS(L"Success...");
    //Z_LOG_FAILURE(L"Failure...");
    //std::thread prod(producer);
    //std::thread cons(consumer);
    //prod.join();
    //cons.join();
    ZSemMutex test_mutex;
    test_mutex.Lock();

    ZThreadPool thread_pool(1);
    {
        Int32 i = 1;
        //thread_pool.AddTask(test_func, Ref(i));
    }
    Sleep(100);
    ZTask test_task([]() {cout<<1; });
    auto test_lambda = [&](Int32 i) {cout << i + 1; };
    thread_pool.AddTask(test_lambda, 1);
    //test_task.Run();
    TArray<Int32,10>* a = nullptr;
    delete a;
    sizeof(TTuple<Int32>);
    thread_pool.LockUntilTaskDone();



    return 0;
}
