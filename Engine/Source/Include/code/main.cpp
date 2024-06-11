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

#include <mutex>
#include <condition_variable>
#include <queue>
#include <tuple>
#include <thread>
using namespace zengine;
using namespace std;


std::mutex mtx;
std::condition_variable cv;
std::queue<int> data_queue;
bool finished = false;

void producer() {
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::lock_guard<std::mutex> lock(mtx);
        data_queue.push(i);
        cv.notify_one();
    }
    {
        std::lock_guard<std::mutex> lock(mtx);
        finished = true;
        cv.notify_all();
    }
}

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !data_queue.empty() || finished; });
        while (!data_queue.empty()) {
            int data = data_queue.front();
            data_queue.pop();
            std::cout << "Consumed: " << data << std::endl;
        }
        if (finished) break;
    }
}

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
    Sleep(100);
    //std::thread prod(producer);
    //std::thread cons(consumer);
    //prod.join();
    //cons.join();

    return 0;
}
