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
    A memory piece.
*/
class CORE_DLLAPI ZMemory : public ZObject<> {
public:
    ZMemory() noexcept;
    ZMemory(const ZMemory& _mem) noexcept;
    ZMemory(ZMemory&& _mem) noexcept;
    ZMemory(SizeType _size) noexcept;

    ~ZMemory() noexcept;

    ZMemory& operator=(const ZMemory& _mem) noexcept;
    ZMemory& operator=(ZMemory&& _mem) noexcept;

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
    NODISCARD FORCEINLINE _ObjectType* GetDataPtr() noexcept {
        return reinterpret_cast<_ObjectType*>(data_ptr_);
    }
    template<typename _ObjectType>
    NODISCARD FORCEINLINE const _ObjectType* GetDataPtr() const noexcept {
        return reinterpret_cast<const _ObjectType*>(data_ptr_);
    }

    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return size_; }
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return capacity_; }

    FORCEINLINE Void Clear() noexcept { memset(data_ptr_, 0, size_); }

    Void Resize(SizeType _size) noexcept;

    FORCEINLINE Void Swap(ZMemory& _mem) noexcept { ::zengine::Swap(this, &_mem); }

protected:
    using SuperType_ = ZObject;

private:
    Byte* data_ptr_;
    SizeType size_;
    SizeType capacity_;
};

}//zengine