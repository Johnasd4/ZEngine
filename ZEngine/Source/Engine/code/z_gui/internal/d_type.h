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

#include "d_lib.h"

namespace zengine {
namespace gui {

/*
    The size of the gui objects.
*/
struct GuiSize {
    NODISCARD FORCEINLINE Bool operator==(const GuiSize& _size) const noexcept { 
        return width_ == _size.width_ && height_ == _size.height_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const GuiSize& _size) const noexcept {
        return width_ != _size.width_ || height_ != _size.height_;
    }
    Float32 width_;
    Float32 height_;
};

/*
    The pos of the gui objects.
*/
struct GuiPos {
    NODISCARD FORCEINLINE Bool operator==(const GuiPos& _pos) const noexcept {
        return x_ == _pos.x_ && y_ == _pos.y_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const GuiPos& _pos) const noexcept {
        return x_ != _pos.x_ || y_ != _pos.y_;
    }
    NODISCARD FORCEINLINE GuiPos operator+(const GuiPos& _pos) const noexcept {
        return GuiPos(x_ + _pos.x_, y_ + _pos.y_);
    }
    NODISCARD FORCEINLINE GuiPos operator-(const GuiPos& _pos) const noexcept {
        return GuiPos(x_ - _pos.x_, y_ - _pos.y_);
    }
    Float32 x_;
    Float32 y_;
};

/*
    The colour of the gui objects, values between 0 to 1.
*/
struct GuiColour {
    Float32 red_;
    Float32 green_;
    Float32 blue_;
    Float32 alpha_;
};

class ZButton;
class ZFrame;
class ZInputText;
class ZText;
class ZWindow;

}//gui
}//zengine