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
#include "z_object.h"

namespace zengine {

/*
    The acllocator for stl containers.
*/
template<typename _ObjectType>
class TContainerAllocator : public ZObject {
public:
    using value_type = _ObjectType;

    NODISCARD static TContainerAllocator& Instance() noexcept {
        static TContainerAllocator allocator;
        return allocator;
    }

    FORCEINLINE TContainerAllocator() noexcept : SuperType_() {}
    template<typename _OtherObjectType>
    FORCEINLINE TContainerAllocator(const TContainerAllocator<_OtherObjectType>& _alocator) noexcept 
        : SuperType_(_alocator) {}
    FORCEINLINE ~TContainerAllocator() {}

    NODISCARD FORCEINLINE _ObjectType* allocate(SizeType _capacity) noexcept {
        return reinterpret_cast<_ObjectType*>(memory_pool::ApplyMemory((SizeType)(_capacity * sizeof(_ObjectType))));
    }

    FORCEINLINE Void deallocate(_ObjectType* _object_ptr, SizeType _capacity) noexcept {
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(_object_ptr));
    }

protected:
    using SuperType_ = ZObject;
};

/*
    The acllocator for smart pointers.
*/
template<typename _ObjectType>
class TSmartPointerAllocator : public ZObject {
public:
    using value_type = _ObjectType;

    FORCEINLINE TSmartPointerAllocator() noexcept : SuperType_() {}
    template<typename _OtherObjectType>
    FORCEINLINE TSmartPointerAllocator(const TSmartPointerAllocator<_OtherObjectType>& _alocator) noexcept
        : SuperType_(_alocator) {}
    FORCEINLINE ~TSmartPointerAllocator() {}

    NODISCARD FORCEINLINE _ObjectType* allocate(SizeType _capacity) noexcept {
        return reinterpret_cast<_ObjectType*>(memory_pool::ApplySmartPointerMemory());
    }

    FORCEINLINE Void deallocate(_ObjectType* _object_ptr, SizeType _capacity) noexcept {
        memory_pool::ReleaseSmartPointerMemory(reinterpret_cast<Void*>(_object_ptr));
    }

protected:
    using SuperType_ = ZObject;
};

}//zengine