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

#include "internal/z_drive.h"

#include "z_object.h"

namespace zengine {

/*
    A memory piece.
*/
class ZMemory : public ZObject {
public:
    FORCEINLINE ZMemory() noexcept : SuperType_(), data_ptr_(), size_(0u) {}
    FORCEINLINE ZMemory(const ZMemory& _mem) noexcept : SuperType_(_mem), size_(_mem.size_) { 
        memcpy(data_ptr_, _mem.data_ptr_, size_);
    }
    FORCEINLINE ZMemory(ZMemory&& _mem) noexcept 
        : SuperType_(std::forward<ZMemory>(_mem)), data_ptr_(_mem.data_ptr_), size_(_mem.size_) 
    {
        _mem.data_ptr_ = nullptr;
        _mem.size_ = 0u;
    } 
    FORCEINLINE ZMemory(MemoryType _size) noexcept 
        : SuperType_(), data_ptr_(static_cast<Byte*>(memory_pool::ApplyMemory(_size))), size_(_size) {}

    FORCEINLINE ~ZMemory() noexcept {}

    FORCEINLINE ZMemory& operator=(const ZMemory& _mem) noexcept {
        memcpy(data_ptr_, _mem.data_ptr_, size_);
        size_ = _mem.size_;
        return *this;
    }
    FORCEINLINE ZMemory& operator=(ZMemory&& _mem) noexcept {
        data_ptr_ = _mem.data_ptr_;
        size_ = _mem.size_;
        _mem.data_ptr_ = nullptr;
        _mem.size_ = 0u;
        return *this;
    }

    NODISCARD FORCEINLINE Byte& operator[](const SizeType _index) noexcept { return data_ptr_[_index]; }
    NODISCARD FORCEINLINE const Byte& operator[](const SizeType _index) const noexcept { return data_ptr_[_index]; }

    NODISCARD FORCEINLINE Byte* DataPtr() noexcept { return data_ptr_; }
    NODISCARD FORCEINLINE const Byte* DataPtr() const noexcept { return data_ptr_; }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { return size_; }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return size_; }

    FORCEINLINE Void Clear() noexcept { memset(data_ptr_, 0, size_); }

    FORCEINLINE Void Resize(MemoryType _size) noexcept { 
        if (!memory_pool::CheckMemory(data_ptr_, _size)) {
            Byte* data_ptr = static_cast<Byte*>(memory_pool::ApplyMemory(_size));
            memcpy(data_ptr, data_ptr_, size_);
            memory_pool::ReleaseMemory(data_ptr_);
            data_ptr_ = data_ptr;
        }
        size_ = _size;
    }

    FORCEINLINE Void Swap(ZMemory& _mem) noexcept { ::zengine::Swap(this, &_mem); }

protected:
    using SuperType_ = ZObject;

private:
    Byte* data_ptr_;
    MemoryType size_;
};

}//zengine