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

#include "m_log.h"
#include "z_buffer.h"
#include "z_object.h"

namespace zengine {
namespace error_code {
enum ZSerializerErrorCodeEnum : ReturnType {
    kZSerializerErrorCode_LinkError = kErrorCodeBase_ZSerializer,
    kZSerializerErrorCode_SystemError,
    kZSerializerErrorCode_NullptrParam,
    kZSerializerErrorCode_ParamOutOfRange,
    kZSerializerErrorCode_DeserializerOutOfData,
};
}//error_code
}//zengine

namespace zengine {

/*
    Serializer class.
    Non-copyable class need to implement OnSerialize() and CalculateSerializeSize() methods.
    ReturnType OnSerialize(ZSerializer& _out) const;
    SizeType CalculateSerializeSize();
*/
class CORE_DLLAPI ZSerializer : public ZObject {
private:
    static inline constexpr Float32 kAutoExtendMultFactor = 1.5f;

public:
    template<typename _ObjectType>
    NODISCARD FORCEINLINE static constexpr SizeType CalculateSerializeSize(const _ObjectType& _obj) noexcept {
        SizeType total_size = 0ULL;
        //complex type
        if constexpr (requires{ _obj.CalculateSerializeSize(); }) {
            total_size += _obj.CalculateSerializeSize();
        }
        //simple type
        else {
            total_size += sizeof(_ObjectType);
        }
        return total_size;
    }

    ZSerializer() noexcept;
    ZSerializer(ZSerializer&& _serializer) noexcept;
    ZSerializer(SizeType _capacity) noexcept;
    ~ZSerializer() noexcept;

    ZSerializer& operator=(ZSerializer&& _serializer) noexcept;

    template<typename _ObjectType>
    NODISCARD ReturnType Write(const _ObjectType& _obj) noexcept {
        ReturnType ret_val = kOK;
        ReturnType link_code = kOK;

        //complex type
        if constexpr (requires{ _obj.OnSerialize(*this); }) {
            link_code = _obj.OnSerialize(*this);
            if (link_code != kOK) {
                ret_val = error_code::kZSerializerErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    L"_ObjectType::OnSerialize() link error!"
                );
                return ret_val;
            }
        }
        //simple type
        else {
            static_assert(std::is_trivially_copyable_v<_ObjectType>,
                "ZSerializer: Type must be trivially copyable or implement OnSerialize to prevent shallow copy bugs."
            );

            link_code = Write(reinterpret_cast<const Void*>(&_obj), sizeof(_ObjectType));
            if (link_code != kOK) {
                ret_val = error_code::kZSerializerErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    L"ZDeserializer::Write() link error!"
                );
                return ret_val;
            }
        }

        return ret_val;
    }

    NODISCARD ReturnType Write(const Void* _data_ptr, SizeType _size) noexcept;

    NODISCARD FORCEINLINE ZConstBuffer ToBuffer() const noexcept { return ZConstBuffer(data_ptr_, size_); }

    NODISCARD FORCEINLINE SizeType Size() const noexcept { return size_; }
    NODISCARD FORCEINLINE SizeType Capacity() const noexcept { return capacity_; }

    FORCEINLINE Void Clear() noexcept { size_ = 0ULL; }

    Void Reserve(SizeType _size) noexcept;

protected:
    using SuperType_ = ZObject;

private:
    ZSerializer(const ZSerializer&) = delete;
    ZSerializer& operator=(const ZSerializer&) = delete;

    FORCEINLINE Void AddSizeP(SizeType _size) noexcept {
        size_ += _size;
        if (size_ > capacity_) {
            Reserve(static_cast<SizeType>(static_cast<Float32>(size_) * kAutoExtendMultFactor));
        }
    }

    Byte* data_ptr_;
    SizeType size_;
    SizeType capacity_;
};

/*
    Deerializer class.
    Non-copyable class need to implement OnDeserialize() methods.
    ReturnType OnDeserialize(ZDeserializer& _in);
*/
class CORE_DLLAPI ZDeserializer : public ZObject {
public:
    ZDeserializer() noexcept;
    ZDeserializer(ZDeserializer&& _deserializer) noexcept;
    ZDeserializer(ZConstBuffer _buffer) noexcept;
    ~ZDeserializer() noexcept;

    ZDeserializer& operator=(ZDeserializer&& _deserializer) noexcept;

    template<typename _ObjectType>
    NODISCARD ReturnType Read(_ObjectType* _obj_ptr) noexcept {
        ReturnType ret_val = kOK;
        ReturnType link_code = kOK;

        //complex type
        if constexpr (requires{ _obj_ptr->OnDeserialize(*this); }) {       
            link_code = _obj_ptr->OnDeserialize(*this);
            if (link_code != kOK) {
                ret_val = error_code::kZSerializerErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    L"_ObjectType::OnDeserialize() link error!"
                );
                return ret_val;
            }
        }
        //simple type
        else {
            static_assert(std::is_trivially_copyable_v<_ObjectType>,
                "ZDeserializer: Type must be trivially copyable or implement OnDeserialize to prevent shallow copy bugs."
            );

            link_code = Read(_obj_ptr, sizeof(_ObjectType));
            if (link_code != kOK) {
                ret_val = error_code::kZSerializerErrorCode_LinkError;
                Z_LOG_ERROR(
                    ret_val, link_code,
                    L"ZDeserializer::Read() link error!"
                );
                return ret_val;
            }
        }

        return ret_val;
    }

    NODISCARD ReturnType Read(Void* _data_ptr, SizeType _size) noexcept;

    FORCEINLINE Void FromBuffer(ZConstBuffer _buffer) noexcept { 
        data_ptr_ = _buffer.DataPtr<const Byte>();
        size_left_ = _buffer.Size();
    }

    NODISCARD FORCEINLINE SizeType SizeLeft() const noexcept { return size_left_; }
    FORCEINLINE Void Clear() noexcept { size_left_ = 0ULL; }

protected:
    using SuperType_ = ZObject;

private:
    ZDeserializer(const ZDeserializer&) = delete;
    ZDeserializer& operator=(const ZDeserializer&) = delete;

    NODISCARD ReturnType ApplyDataSizeCheck(SizeType _offset) noexcept;
    FORCEINLINE Void MoveDataPointerP(SizeType _offset) noexcept {
        size_left_ -= _offset;
        data_ptr_ += _offset;
    }

    const Byte* data_ptr_;
    SizeType size_left_;
};

}//zengine