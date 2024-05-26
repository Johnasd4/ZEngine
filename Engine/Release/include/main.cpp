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
    CChar string[20];
    string[19] = '\0';
    system_time::GetTimeString(string);
    cout << string << endl;

    Sleep(2000);

    system_time::GetTimeString(string);
    cout << string << endl;

    return 0;
}
