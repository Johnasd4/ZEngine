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
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "z_memory.h"

namespace zengine {

ZMemory::ZMemory() noexcept 
    : SuperType_()
    , data_ptr_()
    , size_(0ULL) 
    , capacity_(0ULL)
{}
ZMemory::ZMemory(const ZMemory& _mem) noexcept
    : SuperType_(_mem)
    , data_ptr_(static_cast<Byte*>(memory_pool::ApplyMemory(_mem.size_, &capacity_)))
    , size_(_mem.size_) 
{ 
    Copy(data_ptr_, _mem.data_ptr_, size_);
}
ZMemory::ZMemory(ZMemory&& _mem) noexcept
    : SuperType_(std::forward<ZMemory>(_mem))
    , data_ptr_(_mem.data_ptr_)
    , size_(_mem.size_) 
    , capacity_(_mem.capacity_)
{
    _mem.data_ptr_ = nullptr;
    _mem.size_ = 0ULL;
    _mem.capacity_ = 0ULL;
} 
ZMemory::ZMemory(SizeType _size) noexcept
    : SuperType_()
    , data_ptr_(static_cast<Byte*>(memory_pool::ApplyMemory(_size, &capacity_)))
    , size_(_size) 
{}

ZMemory::~ZMemory() noexcept {
    if (data_ptr_ != nullptr) {
        memory_pool::ReleaseMemory(data_ptr_);
    }
}

ZMemory& ZMemory::operator=(const ZMemory& _mem) noexcept {
    Resize(_mem.size_);
    Copy(data_ptr_, _mem.data_ptr_, size_);
    return *this;
}
ZMemory& ZMemory::operator=(ZMemory&& _mem) noexcept {
    data_ptr_ = _mem.data_ptr_;
    size_ = _mem.size_;
    capacity_ = _mem.capacity_;
    _mem.data_ptr_ = nullptr;
    _mem.size_ = 0ULL;
    _mem.capacity_ = 0ULL;
    return *this;
}

Void ZMemory::Resize(SizeType _size) noexcept {
    if (_size > capacity_) {
        Byte* data_ptr = static_cast<Byte*>(memory_pool::ApplyMemory(_size, &capacity_));
        Copy(data_ptr, data_ptr_, size_);
        memory_pool::ReleaseMemory(data_ptr_);
        data_ptr_ = data_ptr;
    }
    size_ = _size;
}

}//zengine
