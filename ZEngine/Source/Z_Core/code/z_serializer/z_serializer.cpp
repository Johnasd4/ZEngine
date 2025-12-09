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

#include "z_serializer.h"

namespace zengine {

ZSerializer::ZSerializer() noexcept
    : SuperType_()
    , data_ptr_(nullptr)
    , size_(0ULL) 
    , capacity_(0ULL)
{}

ZSerializer::ZSerializer(ZSerializer&& _serializer) noexcept
    : SuperType_()
    , data_ptr_(_serializer.data_ptr_)
    , size_(_serializer.size_)
    , capacity_(_serializer.capacity_)
{
    _serializer.data_ptr_ = nullptr;
    _serializer.size_ = 0ULL;
    _serializer.capacity_ = 0ULL;
}

ZSerializer::ZSerializer(SizeType _capacity) noexcept
    : SuperType_()
    , data_ptr_(static_cast<Byte*>(memory_pool::ApplyMemory(_capacity, &capacity_)))
    , size_(0ULL) 
{}

ZSerializer::~ZSerializer() noexcept {
    if (data_ptr_ != nullptr) {
        memory_pool::ReleaseMemory(data_ptr_);
    }
}

ZSerializer& ZSerializer::operator=(ZSerializer&& _serializer) noexcept {
    if (this == &_serializer) {
        return *this;
    }
    if (data_ptr_ != nullptr) {
        memory_pool::ReleaseMemory(data_ptr_);
    }
    data_ptr_ = _serializer.data_ptr_;
    size_ = _serializer.size_;
    capacity_ = _serializer.capacity_;
    _serializer.data_ptr_ = nullptr;
    _serializer.size_ = 0ULL;
    _serializer.capacity_ = 0ULL;
    return *this;
}

NODISCARD ReturnType ZSerializer::Write(const Void* _data_ptr, SizeType _size) noexcept {
    ReturnType ret_val = kOK;

    Z_CHECK(
        _data_ptr != nullptr,
        error_code::kZSerializerErrorCode_NullptrParam,
        "_data_ptr is nullptr!"
    );

    SizeType pre_size = size_;
    AddSizeP(_size);
    Copy(data_ptr_ + pre_size, _data_ptr, _size);

    return ret_val;
};

Void ZSerializer::Reserve(SizeType _capcity) noexcept {
    if (_capcity > capacity_) {
        Byte* data_ptr = static_cast<Byte*>(memory_pool::ApplyMemory(_capcity, &capacity_));
        Copy(data_ptr, data_ptr_, size_);
        if (data_ptr_ != nullptr) {
            memory_pool::ReleaseMemory(data_ptr_);
        }
        data_ptr_ = data_ptr;
    }
}

}//zengine
