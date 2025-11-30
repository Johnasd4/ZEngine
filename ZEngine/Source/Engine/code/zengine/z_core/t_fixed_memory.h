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

#include "z_object.h"

namespace zengine {

/*
    A memory piece. Fixed size.
*/
template<SizeType _size>
class TFixedMemory : public ZObject {
public:
    FORCEINLINE TFixedMemory() noexcept : SuperType_(), data_ptr_() {}
    FORCEINLINE TFixedMemory(const TFixedMemory& _mem) noexcept : SuperType_(_mem) { 
        Copy(data_ptr_, _mem.data_ptr_, _size);
    }

    FORCEINLINE ~TFixedMemory() noexcept {}

    FORCEINLINE TFixedMemory& operator=(const TFixedMemory& _mem) noexcept {
        Copy(data_ptr_, _mem.data_ptr_, _size);
        return *this;
    }

    NODISCARD FORCEINLINE Byte& operator[](const SizeType _index) noexcept { return data_ptr_[_index]; }
    NODISCARD FORCEINLINE const Byte& operator[](const SizeType _index) const noexcept { return data_ptr_[_index]; }

    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType& At(const SizeType _index) noexcept {
        return *(reinterpret_cast<_ObjectType*>(data_ptr_) + _index);
    }
    template<typename _ObjectType>
    NODISCARD FORCEINLINE const _ObjectType& At(const SizeType _index) const noexcept {
        return *(reinterpret_cast<const _ObjectType*>(data_ptr_) + _index);
    }

    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* DataPtr() noexcept { 
        return reinterpret_cast<_ObjectType*>(data_ptr_);
    }
    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* DataPtr() const noexcept { 
        return reinterpret_cast<const _ObjectType*>(data_ptr_);
    }

    FORCEINLINE static constexpr SizeType Size() noexcept { return _size; }
    FORCEINLINE static constexpr SizeType Capacity() noexcept { return _size; }

    FORCEINLINE Void Clear() noexcept { memset(data_ptr_, 0, _size); }

protected:
    using SuperType_ = ZObject;

private:
    Byte data_ptr_[_size];
};

}//zengine