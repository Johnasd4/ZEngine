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

#include "m_error_message.h"

#include "f_console_output.h"

namespace zengine {
namespace internal {

CORE_DLLAPI Void ConsolePrintErrorMessage(const CChar* error_title, 
                                          const CChar* error_file, 
                                          const CChar* error_funcion,
                                          Int32 error_line, 
                                          const CChar* error_message) noexcept {
    console::Print(
        console::ConsoleOutputTextColour::kConsoleTextColourLightRed, 
        console::ConsoleOutputBackgroundColour::kConsoleBackgroundColourDarkBlack,
        "\n***** %s *****\nFile£∫%s\nFunction£∫%s\nLine£∫%d\nMessage£∫%s\n***** %s *****\n", 
        error_title, error_file, error_funcion, error_line, error_message, error_title);
}

CORE_DLLAPI Void LogErrorMessage(const CChar* error_title,
                                 const CChar* error_file,
                                 const CChar* error_funcion,
                                 Int32 error_line,
                                 const CChar* error_message) noexcept {
    static ZMutex log_mutex;
    log_mutex.Lock();
    //TODO(Johnasd4):The log output.

    log_mutex.Unlock();
}

}//internal
}//zengine