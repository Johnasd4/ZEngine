/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#pragma warning(disable: 4251)  //ignore dllexport warning.
#pragma warning(disable: 4275)  //ignore dllexport warning.
#pragma warning(disable: 6011)
#pragma warning(disable: 26813)
#pragma warning(disable : 26800)

#define NOMINMAX

//#ifdef _WIN32
////sets the _WIN32_WINNT and WINVER to Windows 10
//#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
//#endif
#define WINVER 0x0A00
#define _STL_WIN32_WINNT 0x0A00
//#endif

#include <iostream>

#include "d_macro.h"

//Define if at the start of the z_core cpp file.
#ifdef CORE_DLLFILE
#define CORE_DLLAPI DLLEXPORT
#define PROJECT_NAME "ZCore"
#else
#define CORE_DLLAPI DLLIMPORT
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"Z_Core_x64_Debug.lib")
#else
#pragma comment(lib,"Z_Core_x64_Release.lib")
#endif//_DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib,"Z_Core_Win32_Debug.lib")
#else
#pragma comment(lib,"Z_Core_Win32_Release.lib")
#endif//_DEBUG
#endif//_WIN64
#endif//CORE_DLLFILE