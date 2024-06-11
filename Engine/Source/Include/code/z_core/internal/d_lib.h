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
#ifndef Z_CORE_INTERNAL_D_LIB_H_
#define Z_CORE_INTERNAL_D_LIB_H_

//Ignores the safety io function warning.
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

//Removes the part that conflicts with Winsock.h and Winsock2.h.
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <ctime>
#include <stdarg.h>
#include <process.h>

#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#endif
#ifndef DLLIMPORT
#define DLLIMPORT __declspec(dllimport)
#endif

#ifndef NODISCARD
#define NODISCARD [[nodiscard]]
#endif

//Define if at the start of the z_core cpp file.
#ifdef CORE_DLLFILE
#define CORE_DLLAPI DLLEXPORT
#define PROJECT_NAME_C_STRING "ZCore"
#define PROJECT_NAME_T_STRING L"ZCore"
#else
#define CORE_DLLAPI DLLIMPORT
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"ZCore_x64_Debug.lib")
#else
#pragma comment(lib,"ZCore_x64_Release.lib")
#endif//_DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib,"ZCore_Win32_Debug.lib")
#else
#pragma comment(lib,"ZCore_Win32_Release.lib")
#endif//_DEBUG
#endif//_WIN64
#endif//CORE_DLLFILE

#endif // !Z_CORE_INTERNAL_D_LIB_H_