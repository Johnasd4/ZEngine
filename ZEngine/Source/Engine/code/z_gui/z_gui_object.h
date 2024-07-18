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
    NODISCARD FORCEINLINE Void* OwnerPointer() const noexcept { return owner_ptr_; }
    FORCEINLINE Void SetWidth(Int32 _width) noexcept { width_ = _width; }
    FORCEINLINE Void SetHeight(Int32 _height) noexcept { height_ = _height; }
    FORCEINLINE Void SetOwnerPointer(Void* _owner_ptr) noexcept { owner_ptr_ = _owner_ptr; }

protected:
    using SuperType_ = ZObject;

    ZGuiObject() noexcept : SuperType_(), width_(0), height_(0), owner_ptr_(nullptr) {}
    ZGuiObject(const ZGuiObject& _obj) noexcept : SuperType_(_obj) { CopyP(_obj); }
    ZGuiObject(ZGuiObject&& _obj) noexcept : SuperType_(std::move(_obj)) { MoveP(std::forward<ZGuiObject>(_obj)); }

    const ZGuiObject& operator=(const ZGuiObject& _obj) noexcept {
        SuperType_::operator=(_obj);
        CopyP(_obj);
        return *this;
    }
    const ZGuiObject& operator=(ZGuiObject&& _obj) noexcept {
        SuperType_::operator=(std::move(_obj));
        MoveP(std::forward<ZGuiObject>(_obj));
        return *this;
    }

    FORCEINLINE ~ZGuiObject() {}

private:
    FORCEINLINE Void CopyP(const ZGuiObject& _obj) noexcept {
        width_ = _obj.width_;
        height_ = _obj.height_;
        owner_ptr_ = _obj.owner_ptr_;
    }

    FORCEINLINE Void MoveP(ZGuiObject&& _obj) noexcept {
        width_ = _obj.width_;
        height_ = _obj.height_;
        owner_ptr_ = _obj.owner_ptr_;
        _obj.width_ = 0;
        _obj.height_ = 0;
        _obj.owner_ptr_ = nullptr;
    }

    Int32 width_;
    Int32 height_;
    Void* owner_ptr_;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_GUI_OBJECT_H_
