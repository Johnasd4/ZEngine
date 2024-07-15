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
*/
class GUI_DLLAPI ZGuiObject : public ZObject {
public:
    /* The mutex for the global opengl. */
    static ZMutex& OpenGLMutex() noexcept;
    /* The mutex for the global imgui. */
    static ZMutex& ImguiMutex() noexcept;

protected:
    using SuperType_ = ZObject;

    FORCEINLINE constexpr ZGuiObject() : SuperType_() {}
    FORCEINLINE constexpr ZGuiObject(const ZGuiObject& _obj) : SuperType_(_obj) {}
    FORCEINLINE constexpr ZGuiObject(ZGuiObject&& _obj) : SuperType_(std::move(_obj)) {}

    FORCEINLINE constexpr const ZGuiObject& operator=(const ZGuiObject& _obj) { return *this; }
    FORCEINLINE constexpr const ZGuiObject& operator=(ZGuiObject&& _obj) {
        SuperType_::operator=(std::move(_obj));
        return *this;
    }

    FORCEINLINE constexpr ~ZGuiObject() {}

private:
    static Void* operator new[](SizeType) = delete;
    static Void* operator new[](SizeType, Void*) = delete;
    static Void operator delete[](Void*) = delete;
};

}//gui
}//zengine

#endif // !Z_GUI_Z_GUI_OBJECT_H_
