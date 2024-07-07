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
#ifndef Z_CORE_Z_OBJECT_H_
#define Z_CORE_Z_OBJECT_H_

#pragma warning(disable : 26439)

#include "internal/z_drive.h"
#include "f_memory_pool.h"

namespace zengine {

/*
    The base class of almost everything(except the constant class).
    Inheriting from this class allows the instance to apply memory from the memorypool,
    instead of applying memory directly from the system.
*/
class CORE_DLLAPI ZObject {
public:
    /*
        Overwrite the new and delete operator to use the memory from the memorypool.
    */
    NODISCARD FORCEINLINE static Void* operator new(SizeType _size) {
        return memory_pool::ApplyMemory(static_cast<MemoryType>(_size));
    }
    NODISCARD FORCEINLINE static Void* operator new(SizeType _size, Void* _memory_ptr) {
        return _memory_ptr;
    }
    NODISCARD FORCEINLINE static Void operator delete(Void* _memory_ptr) {
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(_memory_ptr));
    }
    NODISCARD FORCEINLINE static Void operator delete(Void* _delete_memory, Void* _memory_ptr) {}

protected:
    FORCEINLINE constexpr ZObject() {}
    FORCEINLINE constexpr ZObject(const ZObject& _object) {}
    FORCEINLINE constexpr ZObject(ZObject&& _object) { MoveP(std::forward<ZObject>(_object)); }

    FORCEINLINE constexpr const ZObject& operator=(const ZObject& _object) { return *this; }
    FORCEINLINE constexpr const ZObject& operator=(ZObject&& _object) {
        MoveP(std::forward<ZObject>(_object));
        return *this;
    }

    FORCEINLINE constexpr ~ZObject() {}

private:

    static Void* operator new[](SizeType) = delete;
    static Void* operator new[](SizeType, Void*) = delete;
    static Void operator delete[](Void*) = delete;

    /*
        Reset the object to null when moved.
    */
    FORCEINLINE constexpr Void MoveP(ZObject&& _object) {}
};

}//zengine

#endif // !Z_CORE_Z_OBJECT_H_
