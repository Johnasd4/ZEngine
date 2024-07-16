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
#ifndef Z_GUI_Z_GUI_OBJECT_H_
#define Z_GUI_Z_GUI_OBJECT_H_

#include "internal/z_drive.h"

#include "../z_core/z_object.h"
#include "../z_core/z_mutex.h"

namespace zengine {
namespace gui {

/*
    The base class of the gui classes.
    Inheriting from this class allows the instance to apply memory from the memorypool,
    instead of applying memory directly from the system.
    Object members:
    Int32 width_: The width of the gui object.
    Int32 height_: The height of the gui object.
*/
class GUI_DLLAPI ZGuiObject : public ZObject {
public:
    /* The mutex for the global opengl. */
    static ZMutex& OpenGLMutex() noexcept;
    /* The mutex for the global imgui. */
    static ZMutex& ImguiMutex() noexcept;

    NODISCARD FORCEINLINE Int32 Width() const noexcept { return width_; }
    NODISCARD FORCEINLINE Int32 Height() const noexcept { return height_; }
    NODISCARD FORCEINLINE Void SetWidth(Int32 _width) noexcept { width_ = _width; }
    NODISCARD FORCEINLINE Void SetHeight(Int32 _height) noexcept { height_ = _height; }

protected:
    using SuperType_ = ZObject;

    FORCEINLINE ZGuiObject() noexcept : SuperType_(), width_(0), height_(0) {}
    FORCEINLINE ZGuiObject(const ZGuiObject& _obj) noexcept 
        : SuperType_(_obj), width_(_obj.width_), height_(_obj.height_) {}
    FORCEINLINE ZGuiObject(ZGuiObject&& _obj) noexcept 
        : SuperType_(std::move(_obj)), width_(_obj.width_), height_(_obj.height_) 
    {
        _obj.width_ = 0;
        _obj.height_ = 0;
    }

    const ZGuiObject& operator=(const ZGuiObject& _obj) noexcept {
        SuperType_::operator=(_obj);
        width_ = _obj.width_;
        height_ = _obj.height_;
        return *this;
    }
    const ZGuiObject& operator=(ZGuiObject&& _obj) noexcept {
        SuperType_::operator=(std::move(_obj));
        width_ = _obj.width_;
        height_ = _obj.height_;
        _obj.width_ = 0;
        _obj.height_ = 0;
        return *this;
    }

    FORCEINLINE ~ZGuiObject() {}

private:
    static Void* operator new[](SizeType) = delete;
    static Void* operator new[](SizeType, Void*) = delete;
    static Void operator delete[](Void*) = delete;

    Int32 width_;
    Int32 height_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_GUI_OBJECT_H_
