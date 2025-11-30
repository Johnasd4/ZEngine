/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

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
#define PROJECT_NAME L"ZCore"
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