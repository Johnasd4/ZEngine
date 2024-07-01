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
#ifndef Z_CORE_T_SMART_POINTER_H_
#define Z_CORE_T_SMART_POINTER_H_

#include "internal/z_drive.h"

#include "t_allocator.h"
#include "z_object.h"

namespace zengine {

/*
    Unique smart pointer.
*/
template<typename _ObjectType>
class TUniquePointer : public ZObject {
public:
    using STDUniquePointer_ = std::unique_ptr<_ObjectType>;
    using Pointer = STDUniquePointer_::pointer;

    FORCEINLINE TUniquePointer() noexcept : SuperType_(), ptr_() {}
    FORCEINLINE TUniquePointer(TUniquePointer&& _ptr) noexcept : SuperType_(), ptr_(std::move(_ptr.ptr_)) {}
    FORCEINLINE TUniquePointer(Pointer _ptr) noexcept : SuperType_(), ptr_(_ptr) {}

    FORCEINLINE ~TUniquePointer() noexcept {}

    FORCEINLINE TUniquePointer& operator=(TUniquePointer&& _ptr) noexcept { 
        ptr_.operator=(std::move(_ptr.ptr_));
        return *this;
    }

    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        return ptr_.assign(_size, _val);
    }
    template <class _InputIterator>
    FORCEINLINE Void Assign(_InputIterator _first, _InputIterator _last) noexcept {
        return ptr_.assign(_first, _last);
    }
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        return ptr_.assign(_init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TUniquePointer& _ptr) noexcept { return ptr_ == _ptr; }
    NODISCARD FORCEINLINE Bool operator!=(const TUniquePointer& _ptr) noexcept { return ptr_ != _ptr; }

    NODISCARD FORCEINLINE _ObjectType& operator[](const SizeType _index) noexcept { return ptr_[_index]; }
    NODISCARD FORCEINLINE const _ObjectType& operator[](const SizeType _index) const noexcept { return ptr_[_index]; }

    NODISCARD FORCEINLINE _ObjectType& At(IndexType _index) noexcept { return ptr_.at(_index); }
    NODISCARD FORCEINLINE const _ObjectType& At(IndexType _index) const noexcept { return ptr_.at(_index); }

protected:
    using SuperType_ = ZObject;

private:
    TUniquePointer(const TUniquePointer&) = delete;
    TUniquePointer& operator=(const TUniquePointer&) = delete;

    STDUniquePointer_ ptr_;
};

}//zengine

#endif // !Z_CORE_T_SMART_POINTER_H_