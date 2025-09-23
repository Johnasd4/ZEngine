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

#include "z_core/internal/z_drive.h"

//Define if at the start of the z_socket cpp file.
#ifdef SOCKET_DLLFILE
#define SOCKET_DLLAPI DLLEXPORT
#define PROJECT_NAME L"ZSocket"
#else
#define SOCKET_DLLAPI DLLIMPORT
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"Z_Socket_x64_Debug.lib")
#else
#pragma comment(lib,"Z_Socket_x64_Release.lib")
#endif//_DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib,"Z_Socket_Win32_Debug.lib")
#else
#pragma comment(lib,"Z_Socket_Win32_Release.lib")
#endif//_DEBUG
#endif//_WIN64
#endif//SOCKET_DLLFILE