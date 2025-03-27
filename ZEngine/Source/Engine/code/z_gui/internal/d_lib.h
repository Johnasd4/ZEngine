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
#ifndef Z_GUI_INTERNAL_D_LIB_H_
#define Z_GUI_INTERNAL_D_LIB_H_

#include "../../z_core/internal/z_drive.h"

//Define if at the start of the z_core cpp file.
#ifdef GUI_DLLFILE

#include "glfw/glfw3.h" 
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#define GUI_DLLAPI DLLEXPORT
#define PROJECT_NAME L"ZGui"
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glfw3dll.lib")

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"L_ImGui_1_90_9_Opengl3_x64_Debug.lib")
#else
#pragma comment(lib,"L_ImGui_1_90_9_Opengl3_x64_Release.lib")
#endif//_DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib,"L_ImGui_1_90_9_Opengl3_Win32_Debug.lib")
#else
#pragma comment(lib,"L_ImGui_1_90_9_Opengl3_Win32_Release.lib")
#endif//_DEBUG
#endif//_WIN64
#else
#define GUI_DLLAPI DLLIMPORT
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"Z_Gui_x64_Debug.lib")
#else
#pragma comment(lib,"Z_Gui_x64_Release.lib")
#endif//_DEBUG
#else
#ifdef _DEBUG
#pragma comment(lib,"Z_Gui_Win32_Debug.lib")
#else
#pragma comment(lib,"Z_Gui_Win32_Release.lib")
#endif//_DEBUG
#endif//_WIN64
#endif//GUI_DLLFILE

#endif // !Z_GUI_INTERNAL_D_LIB_H_