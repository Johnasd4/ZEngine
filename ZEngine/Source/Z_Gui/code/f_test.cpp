/*
    Copyright (c) YuLin Zhu (朱雨林)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (朱雨林)
    Contact: 1152325286@qq.com
*/
#define GUI_DLLFILE

#include "glfw/glfw3.h" 

#include "f_test.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

namespace zengine {
namespace gui {

void setupImGui(GLFWwindow* window) {
    // 初始化 ImGui 上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // 设置 ImGui 样式
    ImGui::StyleColorsDark();

    // 初始化 ImGui 的 GLFW 实现
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void renderImGui() {
    // 启动新的 ImGui 帧
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 显示示例窗口
    bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);

    // 渲染 ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void cleanupImGui() {
    // 清理 ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

/*
    Test.
*/
GUI_DLLAPI NODISCARD Int32 Test() noexcept {
    // 初始化 GLFW
    if (!glfwInit())
        return -1;

    // 创建一个窗口
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui Example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // 启用垂直同步

    // 初始化 OpenGL 加载器（例如 GLAD）
    // gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // 设置 ImGui
    setupImGui(window);

    // 创建一个窗口
    GLFWwindow* window2 = glfwCreateWindow(1280, 720, "ImGui Example2", NULL, window);
    if (!window2) {
        glfwTerminate();
        return -1;
    }


    // 主循环
    while (!glfwWindowShouldClose(window)) {
        // 清除屏幕
        glClear(GL_COLOR_BUFFER_BIT);

        // 渲染 ImGui 界面
        renderImGui();

        // 交换前后缓冲区
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理 ImGui
    cleanupImGui();

    // 清理 GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}

}//gui
}//zengine