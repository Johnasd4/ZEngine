#ifndef Z_CORE_M_TIME_TEST_H_
#define Z_CORE_M_TIME_TEST_H_

#include "internal/drive.h"

#include "f_console_output.h"

#define TIME_TEST_ONE_TIME(code) {\
		Int32 start_time,end_time;\
		start_time = clock();\
		code;\
		end_time = clock();\
		zengine::console::Print(\
			zengine::console::ConsoleTextColour::kConsoleTextColourLightYellow,\
			zengine::console::ConsoleBackgroundColour::kConsoleBackgroundColourDarkBlack,\
			"The code used %d ms\n",\
			end_time - start_time);\
	}


#endif // !Z_CORE_M_TIME_TEST_H_
