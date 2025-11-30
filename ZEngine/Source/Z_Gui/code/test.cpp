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
#define GUI_DLLFILE
#include "drive/d_pch.h"

#include "test.h"

#include "z_window.h"

namespace zengine {
namespace gui {

class MessageViewer {
private:
    std::vector<std::string> messages;  // 所有消息
    int visibleStart = 0;               // 当前可见区域的起始索引
    int visibleCount = 20;              // 最大可见消息数

public:
    // 添加消息到数组（不会被清空）
    void AddMessage(const std::string& message) {
        messages.push_back(message);
    }

    // 渲染消息列表
    void Render() {
        // 计算可见范围
        int totalMessages = static_cast<int>(messages.size());
        visibleStart = std::max(0, std::min(visibleStart, totalMessages - visibleCount));

        ImGui::Begin("消息列表");
        ImGui::SetWindowFontScale(1);
        std::cout << ImGui::GetScrollMaxY() << std::endl;
        // 显示滚动条和消息计数
        static Bool check_box_bool = true;
        static Int32 radio_buttion_int = true;
        ImGui::Checkbox("勾选框", &check_box_bool);
        ImGui::RadioButton("勾选框1", &radio_buttion_int, 1);
        ImGui::Text("消息总数: %f", ImGui::GetFrameHeightWithSpacing());
        //ImGui::SetWindowFontScale(2);

        std::cout << ImGui::GetScrollMaxY() << std::endl;
        ImGui::SetScrollY(ImGui::GetScrollMaxY() + 100);

        ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiCond_Always);

        // 创建可滚动区域
        //ImGui::BeginChild("滚动区域", ImVec2(0, 0), false);
        //// 显示可见消息
        //int endIndex = std::min(visibleStart + visibleCount, totalMessages);
        //for (int i = visibleStart; i < endIndex; ++i) {
        //    ImGui::Text("[%d] %s", i, messages[i].c_str());
        //    ImGui::Button("Help");

        //    // 添加分隔线（可选）
        //    if (i < endIndex - 1) {
        //        ImGui::Separator();
        //    }
        //}

        //ImGui::EndChild();

        // 滚动控制
        if (ImGui::Button("上翻")) {
            visibleStart = std::max(0, visibleStart - 1);
        }
        ImGui::SameLine();
        if (ImGui::Button("下翻")) {
            visibleStart = std::min(totalMessages - visibleCount, visibleStart + 1);
        }
        ImGui::SameLine();
        ImGui::Text("显示: %d-%d/%d",
            visibleStart + 1,
            std::min(visibleStart + visibleCount, totalMessages),
            totalMessages);

        ImGui::End();
    }

    // 获取消息数量
    size_t GetMessageCount() const {
        return messages.size();
    }

    // 清空所有消息（如果需要）
    void ClearMessages() {
        messages.clear();
        visibleStart = 0;
    }
};


void renderLoop(GLFWwindow*& window) {



}

GUI_DLLAPI Void Test_000() noexcept {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }
    GLFWwindow* window;
    // 在应用中使用
    MessageViewer messageViewer;

    // 添加一些测试消息
    for (int i = 0; i < 100; ++i) {
        messageViewer.AddMessage("消息 " + std::to_string(i) + " - 这是测试内容");
    }

    window = glfwCreateWindow(1000, 1000, "标题", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        return;
    }
    glfwMakeContextCurrent(window);
    //glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);
    IMGUI_CHECKVERSION();
    auto context = ImGui::CreateContext();
    ImGui::SetCurrentContext(context);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    ImGuiIO& io = ImGui::GetIO();

    // 清除默认字体，只使用中文字体
    io.Fonts->Clear();

    // 加载TTF字体文件
    ImFont* font1 = io.Fonts->AddFontFromFileTTF("F:/Font/Sarasa/Sarasa-TTF-1.0.33/SarasaGothicSC-Regular.ttf", 40.0f,
        nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

    if (!font1) {
        std::cerr << "Failed to load Chinese font! Using default font." << std::endl;
        // 如果中文字体加载失败，回退到默认字体
        io.Fonts->AddFontDefault();
    }
    else {
        io.FontDefault = font1; // 设置为默认字体
        std::cout << "Chinese font loaded successfully." << std::endl;
    }

    // 构建字体图谱
    io.Fonts->Build();

    // 关键：创建字体纹理
    ImGui_ImplOpenGL3_CreateFontsTexture();

    while (!glfwWindowShouldClose(window)) {
        glfwMakeContextCurrent(window);

        // 先清除缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Imgui frame start
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static char numberBuffer[128] = "";  // 存储输入的数字
        static std::string long_text = "这是第一行文本\n这是第二行中文测试\nHello World!";

        ImGui::Begin("Hello, world!");

        messageViewer.Render();

        // 使用中文字体
        if (font1) {
            ImGui::PushFont(font1);
        }

        ImGui::SetWindowFontScale(1.0f); // 降低缩放，因为40px已经很大了

        // 创建一个带滚动条的子窗口
        ImGui::BeginChild("ScrollingRegion",
            ImVec2(0, ImGui::GetTextLineHeight() * 10), // 高度为10行
            true, // 显示边框
            ImGuiWindowFlags_HorizontalScrollbar); // 可选的横向滚动条

        if (font1) {
            ImGui::PushFont(font1);
        }
        ImGui::TextUnformatted(long_text.c_str());
        if (font1) {
            ImGui::PopFont();
        }

        ImGui::EndChild();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.8f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.6f, 0.0f, 1.0f));

        if (ImGui::Button("测试按钮")) {
            long_text += "按钮被点击了!\n";
        }
        if (ImGui::Button("测试按钮1")) {
            long_text += "第二个按钮被点击了!\n";
        }

        ImGui::PopStyleColor(4);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
        ImGui::InputText("输入框", numberBuffer, sizeof(numberBuffer), ImGuiInputTextFlags_CharsDecimal);
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.8f, 1.0f));
        ImGui::Text("你输入的是: %s", numberBuffer);
        ImGui::PopStyleColor();

        ImGui::Text("这是一些文本。");

        // 弹出字体
        if (font1) {
            ImGui::PopFont();
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }




    glfwDestroyWindow(window);
    glfwTerminate();

    return;
}

GUI_DLLAPI Void Test_ImguiDemo() noexcept {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }
    GLFWwindow* window;

    //create window
    window = glfwCreateWindow(1000, 1000, "Title", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    IMGUI_CHECKVERSION();
    auto context = ImGui::CreateContext();
    ImGui::SetCurrentContext(context);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    ImGuiIO& io = ImGui::GetIO();

    while (!glfwWindowShouldClose(window)) {
        glfwMakeContextCurrent(window);

        // 先清除缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 调用Demo窗口
        ImGui::ShowDemoWindow();

        // 渲染
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwSwapBuffers(window);
    glfwTerminate();

    return;
}

}//gui
}//zengine