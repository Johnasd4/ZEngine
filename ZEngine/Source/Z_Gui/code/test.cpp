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

#include "test.h"

#include <thread>
#include <mutex>
#include "../z_core/m_log.h"
#include "../z_core/z_thread.h"


namespace zengine {
namespace gui {

    // 全局变量
    GLFWwindow* window1;
    GLFWwindow* window2;

    // 渲染函数
    void renderLoop(GLFWwindow*& window) {
        window = glfwCreateWindow(1000, 1000, "title", NULL, NULL);
        if (!window) {
            std::cerr << "Failed to create window" << std::endl;
            return;
        }
        glfwMakeContextCurrent(window);
        //glEnable(GL_DEPTH_TEST);
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        auto context = ImGui::CreateContext();
        //ImGui::SetCurrentContext(context);
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        while (!glfwWindowShouldClose(window)) {
            glfwMakeContextCurrent(window);

            //Imgui frame start
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            //window_ptr->Tick(delta_time);

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glfwPollEvents();
        }
    }

    // 创建窗口和线程
    void createWindowAndThread(GLFWwindow*& window, const char* title, int width, int height) {
        // 启动渲染线程

    }

    GUI_DLLAPI Int32 Test_000() noexcept {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return -1;
        }
        ZThread thread_1(renderLoop, Ref(window1));
        Sleep(100);
        ZThread thread_2(renderLoop, Ref(window2));
        thread_1.Join();
        thread_2.Join();
        // 创建两个窗口
        createWindowAndThread(window1, "Window 1", 800, 600);
        createWindowAndThread(window2, "Window 2", 800, 600);
        // 主线程循环
        while (!glfwWindowShouldClose(window1) && !glfwWindowShouldClose(window2)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        glfwDestroyWindow(window1);
        glfwDestroyWindow(window2);
        glfwTerminate();

        return 0;
    }

}//gui
}//zengine