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

//#include "zengine/z_engine.h"
//#include "test.h"

using namespace zengine;
using namespace zengine::file_system;
using namespace zengine::console;
//using namespace zengine::gui;
//using namespace zengine::math;
//using namespace zengine::tsrpg;
//using namespace zengine::socket;

//Int32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
Int32 main() { 
    zengine::Initialize();

    //ZMemory(1000);

    SleepMs(200);
    //Z_LOG_MESSAGE("{}\n", sizeof(ZMutex));
    //string::StringToWString("12345");
    //test::GuiTest();
    //test::LogOutputServerTest();
    //test::SocketTest();
    //ZString string("12314253252523451345345134513512341234124");
    //string.Resize(100);

    auto info = GetFileInfoByPath("I:\\ZEngine\\ZEngine\\Binaries\\debugger\\Log\\20251223212340\\20251223212340_Info.log");

    Z_PRINT(
        "{}\n{}\n{}\n{}\n",
        info.path_, info.name_, info.extension_, info.directory_
    )

    Exit(kOK);
}

