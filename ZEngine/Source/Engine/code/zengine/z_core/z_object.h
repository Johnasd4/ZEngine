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

#include "f_memory_pool.h"

namespace zengine {

/*
    The base class of almost everything.
    Inheriting from this class allows the instance to apply memory from the memorypool,
    instead of applying memory directly from the system.
    WARNING: Adding any member will cause fatal error!!!!!!!
*/
class ZObject {
public:
    /*
        Overwrite the new and delete operator to use the memory from the memorypool.
    */
    NODISCARD FORCEINLINE static Void* operator new(SizeType _size) noexcept {
        return memory_pool::ApplyMemory(static_cast<SizeType>(_size));
    }
    NODISCARD FORCEINLINE static Void* operator new(SizeType _size, Void* _memory_ptr) noexcept {
        return _memory_ptr;
    }
    NODISCARD FORCEINLINE static Void operator delete(Void* _memory_ptr) noexcept {
        memory_pool::ReleaseMemory(_memory_ptr);
    }
    NODISCARD FORCEINLINE static Void operator delete(Void* _delete_memory, Void* _memory_ptr) noexcept {}

    FORCEINLINE constexpr const ZObject& operator=(const ZObject& _obj) noexcept { return *this; }
    FORCEINLINE constexpr const ZObject& operator=(ZObject&& _obj) noexcept { return *this; }

    NODISCARD FORCEINLINE Bool operator==(const ZObject& _obj) noexcept {
        return this == &_obj;
    }
    NODISCARD FORCEINLINE Bool operator!=(const ZObject& _obj) noexcept {
        return this != &_obj;
    }

protected:
    FORCEINLINE constexpr ZObject() noexcept {}
    FORCEINLINE constexpr ZObject(const ZObject& _obj) noexcept {}
    FORCEINLINE constexpr ZObject(ZObject&& _obj) noexcept {}

    FORCEINLINE constexpr ~ZObject() noexcept {}

private:
    static Void* operator new[](SizeType) = delete;
    static Void* operator new[](SizeType, Void*) = delete;
    static Void operator delete[](Void*) = delete;
};

#pragma warning(disable : 26439)

}//zengine