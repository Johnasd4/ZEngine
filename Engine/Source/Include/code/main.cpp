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

#include <condition_variable>
#include <mutex>
#include "z_engine.h"

#include <queue>

using namespace zengine;
using namespace std;

int main() {
    Z_LOG_ERROR(1, 2, "TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    Z_LOG_ERROR(1, 2, "TEST%d%d%d%d%d", 3, 4, 5, 6, 7);
    console::PrintMessage("Message...\n");
    console::PrintStart("Start...\n");
    console::PrintProcess("Process 1...\n");
    console::PrintProcess("Process 2...\n");
    console::PrintProcess("Process 3...\n");
    console::PrintFinish("Finish...\n");
    console::PrintSuccess("Success...\n");
    console::PrintFailure("Failure...\n");
    console::PrintError("Error...\n");
    Sleep(1);
    std::unique_lock<std::mutex> b;
    std::condition_variable cv;

    return 0;
}
