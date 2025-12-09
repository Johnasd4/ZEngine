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

#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#pragma warning(disable: 6011)
#pragma warning(disable: 26813)
#pragma warning(disable : 26800)

//Ignores the safety io function warning.
#define _CRT_SECURE_NO_WARNINGS
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#define _CRT_NON_CONFORMING_SWPRINTFS

//Removes the part that conflicts with Winsock.h and Winsock2.h.
#define WIN32_LEAN_AND_MEAN

#define NOMINMAX

#include <iostream>
#include <windows.h>

#include "d_macro.h"

//Define if at the start of the z_core cpp file.
#ifdef CORE_DLLFILE
#define CORE_DLLAPI DLLEXPORT
#define PROJECT_NAME "ZCore"
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"L_WhereAmI_x64_Debug.lib")
#pragma comment(lib,"L_Simdutf_x64_Debug.lib")
#else
#pragma comment(lib,"L_WhereAmI_x64_Release.lib")
#pragma comment(lib,"L_Simdutf_x64_Release.lib")
#endif//_DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib,"L_WhereAmI_Win32_Debug.lib")
#pragma comment(lib,"L_Simdutf_Win32_Debug.lib")
#else
#pragma comment(lib,"L_WhereAmI_Win32_Release.lib")
#pragma comment(lib,"L_Simdutf_Win32_Release.lib")
#endif//_DEBUG
#endif//_WIN64
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