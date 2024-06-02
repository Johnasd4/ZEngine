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
#ifndef Z_CORE_T_ALLOCATOR_H_
#define Z_CORE_T_ALLOCATOR_H_

#pragma warning(disable : 26439)

#include "internal/drive.h"
#include "f_memory_pool.h"

namespace zengine {

/*
    The acllocator for stl containers.
*/
template<typename ObjectType>
class TAllocator : public ZObject {
public:
    using value_type = ObjectType;

    FORCEINLINE TAllocator() : SuperType() {}
    template<typename T>
    FORCEINLINE TAllocator(const TAllocator<T>& alocator) : SuperType() {}
    FORCEINLINE ~TAllocator() {}

    NODISCARD FORCEINLINE ObjectType* allocate(SizeType capacity) noexcept {
        return reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory((MemoryType)(capacity * sizeof(ObjectType))));
    }

    FORCEINLINE Void deallocate(ObjectType* object_ptr, SizeType capacity) noexcept {
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(object_ptr));
    }

protected:
    using SuperType = ZObject;
};

}//zengine

#endif // !Z_CORE_T_ALLOCATOR_H_
