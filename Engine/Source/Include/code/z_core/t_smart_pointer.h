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

    FORCEINLINE TUniquePointer() noexcept : SuperType_(), unique_ptr_() {}
    FORCEINLINE TUniquePointer(Pointer _ptr) noexcept : SuperType_(), unique_ptr_(_ptr) {}
    FORCEINLINE TVector(TVector&& _vector) noexcept : SuperType_(), vector_(std::move(_vector.vector_)) {}

    FORCEINLINE TVector(SizeType _size) noexcept : SuperType_(), vector_(_size) {}
    FORCEINLINE TVector(SizeType _size, const _ObjectType& _val) noexcept : SuperType_(), vector_(_size, _val) {}
    template <typename _InputIterator>
    FORCEINLINE TVector(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), vector_(_first, _last) {}
    FORCEINLINE TVector(InitializerList_ _init_list) noexcept : SuperType_(), vector_(_init_list) {}
 
    FORCEINLINE ~TUniquePointer() noexcept {}

    FORCEINLINE TVector& operator=(const TVector& _vector) noexcept { 
        vector_.operator=(_vector.vector_);
        return *this;
    }
    FORCEINLINE TVector& operator=(TVector&& _vector) noexcept { 
        vector_.operator=(std::move(_vector.vector_));
        return *this;
    }
    FORCEINLINE TVector& operator=(InitializerList_ _init_list) noexcept {
        vector_.operator=(_init_list);
        return *this;
    }

    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        return vector_.assign(_size, _val);
    }
    template <class _InputIterator>
    FORCEINLINE Void Assign(_InputIterator _first, _InputIterator _last) noexcept {
        return vector_.assign(_first, _last);
    }
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        return vector_.assign(_init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TVector& _vector) noexcept { return vector_ == _vector; }
    NODISCARD FORCEINLINE Bool operator!=(const TVector& _vector) noexcept { return vector_ != _vector; }

    NODISCARD FORCEINLINE _ObjectType& operator[](const SizeType _index) noexcept { return vector_[_index]; }
    NODISCARD FORCEINLINE const _ObjectType& operator[](const SizeType _index) const noexcept { return vector_[_index]; }

    NODISCARD FORCEINLINE _ObjectType& At(IndexType _index) noexcept { return vector_.at(_index); }
    NODISCARD FORCEINLINE const _ObjectType& At(IndexType _index) const noexcept { return vector_.at(_index); }

protected:
    using SuperType_ = ZObject;

private:


    STDUniquePointer_ unique_ptr_;
};

}//zengine

#endif // !Z_CORE_T_SMART_POINTER_H_