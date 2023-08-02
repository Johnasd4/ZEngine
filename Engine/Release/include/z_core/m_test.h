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
#ifndef Z_CORE_M_TEST_H_
#define Z_CORE_M_TEST_H_

#include "internal/drive.h"

#include "f_console_output.h"

#define TIME_TEST_ONE_TIME(code) {\
		Int32 start_time,end_time;\
		start_time = clock();\
		code;\
		end_time = clock();\
		zengine::console::Print(\
			zengine::console::ConsoleOutputTextColour::kConsoleTextColourLightYellow,\
			zengine::console::ConsoleOutputBackgroundColour::kConsoleBackgroundColourDarkBlack,\
			"The code used %d ms\n",\
			end_time - start_time);\
	}


#endif // !Z_CORE_M_TEST_H_
