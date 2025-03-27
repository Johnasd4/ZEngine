/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/

#include "$safeitemname$.h"

using namespace zengine;
using namespace zengine::gui;

/*
    Change ButtonName to change the button name, add ## before the ButtonName to hide the button name.
*/
$safeitemname$::$safeitemname$() noexcept
    : ZButton("ButtonName")
{
    //Add code here.
}

$safeitemname$::~$safeitemname$() noexcept {}

Void $safeitemname$::Begin() noexcept {
    SuperType_::Begin();
    //Add code here.
}

Void $safeitemname$::Tick(Float32 _delta_sec) noexcept {
    SuperType_::Tick(_delta_sec);
    //Add code here.
}

Void $safeitemname$::OnButtonClicked() noexcept {
    //Add code here.
}