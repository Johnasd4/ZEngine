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
    ZSystemTime t;
    cout << t.Year() << endl;
    cout << t.Month() << endl;
    cout << t.Day() << endl;
    cout << t.Hour() << endl;
    cout << t.Min() << endl;
    cout << t.Sec() << endl;
    std::array<int, 5> arr1 = {0};
    return 0;
}
