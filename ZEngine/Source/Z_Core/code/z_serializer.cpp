/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "z_serializer.h"

#include "m_log.h"

namespace zengine {

ZSerializer::ZSerializer(TFunction<ReturnType(ZConstBuffer)>&& _output_func) noexcept
    : SuperType_()
    , output_func_(std::move(_output_func))
{}

ZSerializer::ZSerializer(ZSerializer&& _serializer) noexcept
    : SuperType_(std::move(_serializer))
    , output_func_(std::move(_serializer.output_func_))
{}

ZSerializer& ZSerializer::operator=(ZSerializer&& _serializer) noexcept {
    if (this == &_serializer) {
        return *this;
    }

    output_func_ = std::move(_serializer.output_func_);

    return *this;
}

NODISCARD ReturnType ZSerializer::WriteString(ZStringView _string) noexcept {
    ReturnType link_code = kOK;
    SizeType str_size = _string.GetSize();
    if (str_size >= kMaxStringLength) {
        ReturnType ret_val = error_code::kZSerializerErrorCode_StringTooLarge;
        Z_LOG_ERROR(
            ret_val, 0ULL,
            "String size too large! size: {} max size: {}",
            str_size, kMaxStringLength
        );
        return ret_val;
    }
    link_code = WriteUInt64(str_size);
    if (link_code != kOK) {
        return link_code;
    }
    if (str_size > 0ULL) {
        link_code = WriteData(ZConstBuffer(_string.GetDataPtr(), str_size));
    }
    return link_code;
};

NODISCARD ReturnType ZSerializer::WriteData(ZConstBuffer _buffer) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = output_func_(_buffer);
    if (link_code != kOK) {
        ret_val = error_code::kZSerializerErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZSerializer::output_func_() link error!"
        );
        return ret_val;
    }
    return ret_val;
};

ZDeserializer::ZDeserializer(TFunction<ReturnType(ZBuffer)>&& _input_func) noexcept
    : SuperType_()
    , input_func_(std::move(_input_func))
{
}

ZDeserializer::ZDeserializer(ZDeserializer&& _deserializer) noexcept 
    : SuperType_(std::move(_deserializer))
    , input_func_(std::move(_deserializer.input_func_))
{}

ZDeserializer& ZDeserializer::operator=(ZDeserializer&& _deserializer) noexcept {
    if (this == &_deserializer) {
        return *this;
    }

    input_func_ = std::move(_deserializer.input_func_);

    return *this;
}

NODISCARD ReturnType ZDeserializer::ReadString(ZString* _string_ptr) noexcept {
    ReturnType link_code = kOK;
    SizeType str_size = 0;
    link_code = Read(&str_size);
    if(link_code != kOK) {
        return link_code;
    }
    if(str_size >= kMaxStringLength) {
        ReturnType ret_val = error_code::kZSerializerErrorCode_StringTooLarge;
        Z_LOG_ERROR(
            ret_val, 0,
            "String size too large! size: {} max size: {}",
            str_size, kMaxStringLength
        );
        return ret_val;
    }

    if (str_size > 0ULL) {
        _string_ptr->Resize(str_size);
        return ReadData(ZBuffer(&(*_string_ptr)[0], str_size));
    }
    else {
        _string_ptr->Clear();
    }
    return kOK;
}

NODISCARD ReturnType ZDeserializer::ReadData(ZBuffer _buffer) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    link_code = input_func_(_buffer);
    if (link_code != kOK) {
        ret_val = error_code::kZSerializerErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            "ZSerializer::input_func_() link error!"
        );
        return ret_val;
    }
    return ret_val;
}

} // zengine
