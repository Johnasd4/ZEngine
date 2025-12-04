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

ZDeserializer::ZDeserializer() noexcept
    : SuperType_()
    , data_ptr_(nullptr)
    , size_left_(0ULL)
{}

ZDeserializer::ZDeserializer(ZDeserializer&& _serializer) noexcept
    : SuperType_()
    , data_ptr_(_serializer.data_ptr_)
    , size_left_(_serializer.size_left_)
{
    _serializer.data_ptr_ = nullptr;
    _serializer.size_left_ = 0ULL;
}

ZDeserializer::ZDeserializer(ZConstBuffer _buffer) noexcept
    : SuperType_()
    , data_ptr_(_buffer.DataPtr<const Byte>())
    , size_left_(_buffer.Size())
{}

ZDeserializer::~ZDeserializer() noexcept {}

ZDeserializer& ZDeserializer::operator=(ZDeserializer&& _serializer) noexcept {
    if (this == &_serializer) {
        return *this;
    }
    data_ptr_ = _serializer.data_ptr_;
    size_left_ = _serializer.size_left_;
    _serializer.data_ptr_ = nullptr;
    _serializer.size_left_ = 0ULL;
    return *this;
}

NODISCARD ReturnType ZDeserializer::Read(Void* _data_ptr, SizeType _size) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        _data_ptr != nullptr,
        error_code::kZSerializerErrorCode_NullptrParam,
        L"_data_ptr is nullptr!"
    );

    link_code = ApplyDataSizeCheck(_size);
    if (link_code != kOK) {
        ret_val = error_code::kZSerializerErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZDeserializer::MoveDataPointerP() link error!"
        );
        return ret_val;
    }
    Copy(_data_ptr, data_ptr_, _size);
    MoveDataPointerP(_size);

    return ret_val;
}

NODISCARD ReturnType ZDeserializer::ApplyDataSizeCheck(SizeType _offset) noexcept {
    ReturnType ret_val = kOK;

    if (_offset > size_left_) {
        ret_val = error_code::kZSerializerErrorCode_DeserializerOutOfData;
        Z_LOG_ERROR(
            ret_val, 0,
            L"size_left_(%llu) is smaller than _offset(%llu)!",
            size_left_, _offset
        );
        return ret_val;
    }

    return ret_val;
}

}//zengine
