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

#include "drive.h"

#include <charconv>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <regex>
#include <stdarg.h>
#include <thread>
#ifdef _WIN32
#include <shellapi.h>
#include <shobjidl.h>
#include <windows.h>
#endif

#include <simdutf/simdutf.h>
#include <whereami/whereami.h>

#include "library/l_fmt.h"
#include "library/l_rapidjson.h"