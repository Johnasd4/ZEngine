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
#define PROJECT_NAME "Include"

#include "zengine/z_core.h"
#include "zengine/z_math.h"
#include "zengine/z_lua.h"
#include "zengine/z_gui.h"
#include "zengine/z_socket.h"

//#include "zengine/z_engine.h"
//#include "test.h"

using namespace zengine;
using namespace zengine::file_system;
using namespace zengine::console;
using namespace zengine::lua;
using namespace zengine::gui;
//using namespace zengine::math;
//using namespace zengine::tsrpg;
using namespace zengine::socket;

//Int32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
Int32 main() {
    zengine::Initialize();

    ReturnType link_code = kOK;

    ZIOContext io_context;
    ZIOContextWorkGuard work_guard(io_context);
    ZThread thread([&io_context] () {
        io_context.Run();
    });
    auto address = ZIP4Address::MakeAddress("0.0.0.0");
    ZUDPSocket udp_socket(io_context);
    ZUDPEndpoint endpoint(*address, 10000);
    udp_socket.Open(endpoint.GetIPType());
    udp_socket.BindEndpoint(endpoint);


    Z_LOG_MESSAGE("{}", math::Rand(1, 3));
    Z_LOG_MESSAGE("Address port: {}, ip: {}", endpoint.GenerateIPString(), endpoint.GetPort());

    auto result = socket::GetUDPSocketPublicIP4AndPort(udp_socket);
    work_guard.Reset();
    if (result) {
        Z_LOG_MESSAGE(
            "GetUDPPublicIP4AndPort: port: {}, ip: {}", 
            result->GenerateIPString(), result->GetPort()
        );

    }

    zengine::socket::ZTCPSocket* socket_ptr = new zengine::socket::ZTCPSocket(io_context);
    delete socket_ptr;

    ZLuaState lua_state;
    link_code = lua_state.Initialize();

    lua_state.ExecuteScript(R"(
        Z_LOG_ERROR(0, "Error...");
        Z_LOG_TRACE("Trace...");
        Z_LOG_MESSAGE("Message...");
        Z_LOG_START("Start...");
        Z_LOG_PROCESS("Process...");
        Z_LOG_FINISH("Finish...");
        Z_LOG_FAILURE("Failure...");
        Z_LOG_SUCCESS("Success...");
    )");
    ZApplication app{ "test title", GuiSize(1000, 1000), GuiPos(0, 0) };
    app.SetPosToScreenCenter();
    ZWindow window{ "test window", GuiSize(500, 500), GuiPos(200, 200) };
    window.SetFontScale(1.0f);
    app.AddWindow(&window);
    ZFrame frame{ "test frame" };
    window.AddWidget(&frame);
    ZButton button{ "test button" };
    ZButton button2{ "test button2" };
    //frame.SetEnabled(false);
    frame.AddWidget(&button);
    frame.AddWidget(&button2);
    ZCheckBox checkbox{ "test checkbox" };
    frame.AddWidget(&checkbox);
    ZSeparator separator{};
    frame.AddWidget(&separator);
    //separator.SetPos(GuiPos(0, 300));
    app.Execute();

    Z_LOG_FINISH("Program exited with code 0x{:x}", 0);
    ZThread::SleepMs(kExitTimeMs);
    log::FinishFlush();
    console::Flush();
    return 0;
}

