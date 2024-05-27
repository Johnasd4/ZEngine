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

#include "z_engine.h"

using namespace zengine;
using namespace std;

int main() {
    // 获取当前时间
    auto now = std::chrono::system_clock::now();

    // 获取当前时间点的时间戳
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    // 获取当前本地时间
    std::tm local_tm = *std::localtime(&now_time_t);

    // 获取当前UTC时间
    std::tm utc_tm = *std::gmtime(&now_time_t);

    // 计算本地时间和UTC时间之间的差异
    int local_hour = local_tm.tm_hour;
    int utc_hour = utc_tm.tm_hour;

    // 计算时区偏移（以小时为单位）
    int timezone_offset = local_hour - utc_hour;

    // 处理跨天情况
    if (local_tm.tm_yday > utc_tm.tm_yday) {
        timezone_offset += 24;
    }
    else if (local_tm.tm_yday < utc_tm.tm_yday) {
        timezone_offset -= 24;
    }

    std::cout << "Local time offset (hours): " << timezone_offset << std::endl;

    time_t t;
    time(&t);
    int sec = t % 60;
    cout << "sec:" << sec << endl;
    t = t - sec;
    t = t / 60;

    int min = t % 60;
    cout << "min:" << min << endl;
    t = t - min;
    t = t / 60;

    int hour = t % 24;
    t = t - hour;
    t = t / 24;
    hour = (hour + 8) % 24;
    cout << "hour:" << hour << endl;
    
    t = t - 30 * 365 - 6;//2000

    int year = 2000;
    year += 400 * (t / ((365 * 100 + 24) * 4 + 1));    //400
    t -= year / 400 * ((365 * 100 + 24) * 4 + 1);
    year += 100 * (t / (365 * 100 + 24));    //100
    t -= year / 100 * (365 * 100 + 24);
    year += 4 * (t / (365 * 4 + 1));    //4
    t -= year / 4 * (365 * 4 + 1);
    year += 4 * (t / 365);    //1
    t -= year * 365;
    cout << "year:" << year << endl;

    cout << t << endl;

    return 0;
}
