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
#define CORE_DLLFILE

#include "f_console.h"

namespace zengine {
namespace console {

CORE_DLLAPI Void SetConsoleOutputColour(ConsoleOutputTextColourType test_colour,
                                        ConsoleOutputBackgroundColourType background_colour) noexcept {
    static internal::ZConsoleOutputSettings& output_settings = internal::ZConsoleOutputSettings::InstanceP();
    output_settings.SetTextColour(test_colour);
    output_settings.SetBackgroundColour(background_colour);
    //Changes the console output colour.
    SetConsoleTextAttribute(
        GetStdHandle(STD_OUTPUT_HANDLE), 
        (ConsoleOutputColourType)test_colour | (ConsoleOutputColourType)background_colour);    
}

}//console
}//zengine
