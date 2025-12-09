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

#include "z_json.h"

#include "m_log.h" 
#include "z_file.h"
#include "z_memory.h"

using namespace rapidjson;

namespace zengine {

using JsonStringBuffer = rapidjson::GenericStringBuffer<rapidjson::UTF8<>, internal::JsonAllocatorP>;

ZJsonValue::~ZJsonValue() noexcept {}

ZJsonValue& ZJsonValue::operator=(Bool _value) noexcept {
    *json_value_ptr_ = _value;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(Int32 _value) noexcept {
    *json_value_ptr_ = _value;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(Int64 _value) noexcept {
    *json_value_ptr_ = _value;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(UInt32 _value) noexcept {
    *json_value_ptr_ = _value;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(UInt64 _value) noexcept {
    *json_value_ptr_ = _value;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(Float32 _value) noexcept {
    *json_value_ptr_ = _value;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(Float64 _value) noexcept {
    *json_value_ptr_ = _value;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(const Char* _value) noexcept {
    *json_value_ptr_ = internal::JsonValue(_value, *json_allocator_ptr_);
    return *this;
}
ZJsonValue& ZJsonValue::operator=(const ZJsonValue& _value) noexcept {
    *json_value_ptr_ = *_value.json_value_ptr_;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(ZJsonValue&& _value) noexcept {
    *json_value_ptr_ = std::move(*_value.json_value_ptr_);
    return *this;
}

ZJsonValue::operator Bool() const noexcept {
    return json_value_ptr_ == nullptr ? false : json_value_ptr_->GetBool();
}
ZJsonValue::operator Int32() const noexcept {
    return json_value_ptr_ == nullptr ? 0 : json_value_ptr_->GetInt();
}
ZJsonValue::operator Int64() const noexcept {
    return json_value_ptr_ == nullptr ? 0LL : json_value_ptr_->GetInt64();
}
ZJsonValue::operator UInt32() const noexcept {
    return json_value_ptr_ == nullptr ? 0U : json_value_ptr_->GetUint();
}
ZJsonValue::operator UInt64() const noexcept {
    return json_value_ptr_ == nullptr ? 0ULL : json_value_ptr_->GetUint64();
}
ZJsonValue::operator Float32() const noexcept {
    return json_value_ptr_ == nullptr ? 0.0F : json_value_ptr_->GetFloat();
}
ZJsonValue::operator Float64() const noexcept {
    return json_value_ptr_ == nullptr ? 0.0 : json_value_ptr_->GetDouble();
}
ZJsonValue::operator const Char*() const noexcept {
    return json_value_ptr_ == nullptr ? "" : json_value_ptr_->GetString();
}

NODISCARD ZJsonValue ZJsonValue::operator[](const Char* _key) noexcept {
    if (!HasMember(_key)) {
        json_value_ptr_->AddMember(StringRef(_key), 0, *json_allocator_ptr_);
    }
    ZJsonValue value;
    value.json_value_ptr_ = &(*json_value_ptr_)[_key];
    value.json_allocator_ptr_ = json_allocator_ptr_;
    return value;
}

NODISCARD const ZJsonValue ZJsonValue::operator[](const Char* _key) const noexcept {
    if (!HasMember(_key)) {
        json_value_ptr_->AddMember(StringRef(_key), 0, *json_allocator_ptr_);
    }
    ZJsonValue value;
    value.json_value_ptr_ = &(*json_value_ptr_)[_key];
    value.json_allocator_ptr_ = json_allocator_ptr_;
    return value;
}

NODISCARD Bool ZJsonValue::IsBool() const noexcept {
    return json_value_ptr_ == nullptr ? false : json_value_ptr_->IsBool();
}
NODISCARD Bool ZJsonValue::IsInt32() const noexcept {
    return json_value_ptr_ == nullptr ? false : json_value_ptr_->IsInt();
}
NODISCARD Bool ZJsonValue::IsInt64() const noexcept {
    return json_value_ptr_ == nullptr ? false : json_value_ptr_->IsInt64();
}
NODISCARD Bool ZJsonValue::IsUInt32() const noexcept {
    return json_value_ptr_ == nullptr ? false : json_value_ptr_->IsUint();
}
NODISCARD Bool ZJsonValue::IsUInt64() const noexcept {
    return json_value_ptr_ == nullptr ? false : json_value_ptr_->IsUint64();
}
NODISCARD Bool ZJsonValue::IsFloat32() const noexcept {
    return json_value_ptr_ == nullptr ? false : json_value_ptr_->IsFloat();
}
NODISCARD Bool ZJsonValue::IsFloat64() const noexcept {
    return json_value_ptr_ == nullptr ? false : json_value_ptr_->IsDouble();
}
NODISCARD Bool ZJsonValue::IsString() const noexcept {
    return json_value_ptr_ == nullptr ? false : json_value_ptr_->IsString();
}

NODISCARD Bool ZJsonValue::GetBool() const noexcept {
    return json_value_ptr_ == nullptr ? false : json_value_ptr_->GetBool();
}
NODISCARD Int32 ZJsonValue::GetInt32() const noexcept {
    return json_value_ptr_ == nullptr ? 0 : json_value_ptr_->GetInt();
}
NODISCARD Int64 ZJsonValue::GetInt64() const noexcept {
    return json_value_ptr_ == nullptr ? 0LL : json_value_ptr_->GetInt64();
}
NODISCARD UInt32 ZJsonValue::GetUInt32() const noexcept {
    return json_value_ptr_ == nullptr ? 0U : json_value_ptr_->GetUint();
}
NODISCARD UInt64 ZJsonValue::GetUInt64() const noexcept {
    return json_value_ptr_ == nullptr ? 0ULL : json_value_ptr_->GetUint64();
}
NODISCARD Float32 ZJsonValue::GetFloat32() const noexcept {
    return json_value_ptr_ == nullptr ? 0.0F : json_value_ptr_->GetFloat();
}
NODISCARD Float64 ZJsonValue::GetFloat64() const noexcept {
    return json_value_ptr_ == nullptr ? 0.0 : json_value_ptr_->GetDouble();
}
NODISCARD const Char* ZJsonValue::GetString() const noexcept {
    return json_value_ptr_ == nullptr ? "" : json_value_ptr_->GetString();
}

Void ZJsonValue::AddMember(const Char* _key, Bool _value, Bool _overwrite_exist) noexcept {
    if (json_value_ptr_->HasMember(_key)) {
        if (_overwrite_exist) {
            (*json_value_ptr_)[_key] = _value;
        }
    }
    else {
        json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
    }
}
Void ZJsonValue::AddMember(const Char* _key, Int32 _value, Bool _overwrite_exist) noexcept {
    if (json_value_ptr_->HasMember(_key)) {
        if (_overwrite_exist) {
            (*json_value_ptr_)[_key] = _value;
        }

    }
    else {
        json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
    }
}
Void ZJsonValue::AddMember(const Char* _key, Int64 _value, Bool _overwrite_exist) noexcept {
    if (json_value_ptr_->HasMember(_key)) {
        if (_overwrite_exist) {
            (*json_value_ptr_)[_key] = _value;
        }
    }
    else {
        json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
    }
}
Void ZJsonValue::AddMember(const Char* _key, UInt32 _value, Bool _overwrite_exist) noexcept {
    if (json_value_ptr_->HasMember(_key)) {
        if (_overwrite_exist) {
            (*json_value_ptr_)[_key] = _value;
        }
    }
    else {
        json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
    }
}
Void ZJsonValue::AddMember(const Char* _key, UInt64 _value, Bool _overwrite_exist) noexcept {
    if (json_value_ptr_->HasMember(_key)) {
        if (_overwrite_exist) {
            (*json_value_ptr_)[_key] = _value;
        }
    }
    else {
        json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
    }
}
Void ZJsonValue::AddMember(const Char* _key, Float32 _value, Bool _overwrite_exist) noexcept {
    if (json_value_ptr_->HasMember(_key)) {
        if (_overwrite_exist) {
            (*json_value_ptr_)[_key] = _value;
        }
    }
    else {
        json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
    }
}
Void ZJsonValue::AddMember(const Char* _key, Float64 _value, Bool _overwrite_exist) noexcept {
    if (json_value_ptr_->HasMember(_key)) {
        if (_overwrite_exist) {
            (*json_value_ptr_)[_key] = _value;
        }
    }
    else {
        json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
    }
}
Void ZJsonValue::AddMember(const Char* _key, const Char* _value, Bool _overwrite_exist) noexcept {
    if (json_value_ptr_->HasMember(_key)) {
        if (_overwrite_exist) {
            (*json_value_ptr_)[_key] = internal::JsonValue(_value, *json_allocator_ptr_);
        }
    }
    else {
        json_value_ptr_->AddMember(
            StringRef(_key),
            internal::JsonValue(_value, *json_allocator_ptr_),
            *json_allocator_ptr_
        );
    }
}
Void ZJsonValue::AddMember(const Char* _key, const ZJsonValue& _value, Bool _overwrite_exist) noexcept {
    if (json_value_ptr_->HasMember(_key)) {
        if (_overwrite_exist) {
            (*json_value_ptr_)[_key] = internal::JsonValue(*_value.json_value_ptr_, *json_allocator_ptr_);
        }
    }
    else {
        json_value_ptr_->AddMember(StringRef(_key), *_value.json_value_ptr_, *json_allocator_ptr_);
    }
}
Void ZJsonValue::AddMember(const Char* _key, ZJsonValue&& _value, Bool _overwrite_exist) noexcept {
    if (json_value_ptr_->HasMember(_key)) {
        (*json_value_ptr_)[_key] = internal::JsonValue(
            std::move(*_value.json_value_ptr_), *json_allocator_ptr_
        );
    }
    else {
        json_value_ptr_->AddMember(
            StringRef(_key), std::move(*_value.json_value_ptr_), *json_allocator_ptr_
        );
    }
}

NODISCARD Bool ZJsonValue::RemoveMember(const Char* _key) noexcept {
    return json_value_ptr_ ? json_value_ptr_->RemoveMember(_key) : false;
}

NODISCARD Bool ZJsonValue::HasMember(const Char* _key) const noexcept {
    return json_value_ptr_ ? json_value_ptr_->HasMember(_key) : false;
}

Void ZJsonValue::Clear() noexcept {
    if (json_value_ptr_) {
        json_value_ptr_->Clear();
    }
}

NODISCARD ZString ZJsonValue::GenerateJsonString() const noexcept {
    if (json_value_ptr_ == nullptr) {
        return ZString("");
    }

    //json to string
    JsonStringBuffer buffer;
    rapidjson::PrettyWriter<JsonStringBuffer> writer(buffer);
    writer.SetIndent(' ', 4);
    json_value_ptr_->Accept(writer);
    ZString str = buffer.GetString();

    return str;
}

ZJsonValue::ZJsonValue() noexcept
    : json_value_ptr_(nullptr)
    , json_allocator_ptr_(nullptr)
{}
ZJsonValue::ZJsonValue(ZJsonValue&& _value) noexcept
    : json_value_ptr_(_value.json_value_ptr_)
    , json_allocator_ptr_(_value.json_allocator_ptr_)
{
    _value.json_value_ptr_ = nullptr;
    _value.json_allocator_ptr_ = nullptr;
}

ZJsonValue::ZJsonValue(const Char* _key, Bool _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , json_allocator_ptr_(nullptr)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
}
ZJsonValue::ZJsonValue(const Char* _key, Int32 _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , json_allocator_ptr_(nullptr)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
}
ZJsonValue::ZJsonValue(const Char* _key, Int64 _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , json_allocator_ptr_(nullptr)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
}
ZJsonValue::ZJsonValue(const Char* _key, UInt32 _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , json_allocator_ptr_(nullptr)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
}
ZJsonValue::ZJsonValue(const Char* _key, UInt64 _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , json_allocator_ptr_(nullptr)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
}
ZJsonValue::ZJsonValue(const Char* _key, Float32 _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , json_allocator_ptr_(nullptr)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
}
ZJsonValue::ZJsonValue(const Char* _key, Float64 _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , json_allocator_ptr_(nullptr)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), _value, *json_allocator_ptr_);
}
ZJsonValue::ZJsonValue(const Char* _key, const Char* _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , json_allocator_ptr_(nullptr)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(
        StringRef(_key),
        internal::JsonValue(_value, *json_allocator_ptr_),
        *json_allocator_ptr_
    );
}
ZJsonValue::ZJsonValue(const Char* _key, const ZJsonValue& _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , json_allocator_ptr_(nullptr)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), *_value.json_value_ptr_, *json_allocator_ptr_);
}
ZJsonValue::ZJsonValue(const Char* _key, ZJsonValue&& _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , json_allocator_ptr_(nullptr)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(
        StringRef(_key), std::move(*_value.json_value_ptr_),
        *json_allocator_ptr_
    );
}

ZJsonDocument::ZJsonDocument() noexcept
    : json_doc_()
{}
ZJsonDocument::ZJsonDocument(ZJsonDocument&& _value) noexcept
    : json_doc_(std::move(_value.json_doc_))
{}
ZJsonDocument::ZJsonDocument(const Char* _key, Bool _value) noexcept
    : json_doc_()
{
    json_doc_.SetObject();
    json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
}
ZJsonDocument::ZJsonDocument(const Char* _key, Int32 _value) noexcept
    : json_doc_()
{
    json_doc_.SetObject();
    json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
}
ZJsonDocument::ZJsonDocument(const Char* _key, Int64 _value) noexcept
    : json_doc_()
{
    json_doc_.SetObject();
    json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
}
ZJsonDocument::ZJsonDocument(const Char* _key, UInt32 _value) noexcept
    : json_doc_()
{
    json_doc_.SetObject();
    json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
}
ZJsonDocument::ZJsonDocument(const Char* _key, UInt64 _value) noexcept
    : json_doc_()
{
    json_doc_.SetObject();
    json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
}
ZJsonDocument::ZJsonDocument(const Char* _key, Float32 _value) noexcept
    : json_doc_()
{
    json_doc_.SetObject();
    json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
}
ZJsonDocument::ZJsonDocument(const Char* _key, Float64 _value) noexcept
    : json_doc_()
{
    json_doc_.SetObject();
    json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
}
ZJsonDocument::ZJsonDocument(const Char* _key, const Char* _value) noexcept
    : json_doc_()
{
    json_doc_.SetObject();
    json_doc_.AddMember(
        StringRef(_key), 
        internal::JsonValue(_value, json_doc_.GetAllocator()), 
        json_doc_.GetAllocator()
    );
}
ZJsonDocument::ZJsonDocument(const Char* _key, const ZJsonValue& _value) noexcept
    : json_doc_()
{
    json_doc_.SetObject();
    json_doc_.AddMember(StringRef(_key), *_value.json_value_ptr_, json_doc_.GetAllocator());
}
ZJsonDocument::ZJsonDocument(const Char* _key, ZJsonValue&& _value) noexcept
    : json_doc_()
{
    json_doc_.SetObject();
    json_doc_.AddMember(
        StringRef(_key), std::move(*_value.json_value_ptr_), json_doc_.GetAllocator()
    );
}
ZJsonDocument::ZJsonDocument(const Char* _key, const ZJsonDocument& _doc) noexcept
    : json_doc_()
{
    json_doc_.SetObject();
    json_doc_.AddMember(
        StringRef(_key), 
        internal::JsonValue(_doc.json_doc_, json_doc_.GetAllocator()),
        json_doc_.GetAllocator()
    );
}
ZJsonDocument::ZJsonDocument(const Char* _key, ZJsonDocument&& _doc) noexcept
    : json_doc_()
{
    json_doc_.SetObject();
    json_doc_.AddMember(
        StringRef(_key), std::move(_doc.json_doc_), json_doc_.GetAllocator()
    );
}

ZJsonDocument::~ZJsonDocument() noexcept {}

ZJsonDocument& ZJsonDocument::operator=(ZJsonDocument&& _doc) noexcept {
    json_doc_ = std::move(_doc.json_doc_);
    return *this;
}

NODISCARD ZJsonValue ZJsonDocument::operator[](const Char* _key) noexcept {
    if (!HasMember(_key)) {
        json_doc_.AddMember(StringRef(_key), 0, json_doc_.GetAllocator());
    }
    ZJsonValue value;
    value.json_value_ptr_ = &json_doc_[_key];
    value.json_allocator_ptr_ = &json_doc_.GetAllocator();
    return value;
}

NODISCARD const ZJsonValue ZJsonDocument::operator[](const Char* _key) const noexcept {
    if (!HasMember(_key)) {
        &const_cast<ZJsonDocument*>(this)->json_doc_.AddMember(
            StringRef(_key), 
            0, 
            const_cast<ZJsonDocument*>(this)->json_doc_.GetAllocator()
        );
    }
    ZJsonValue value;
    value.json_value_ptr_ = &const_cast<ZJsonDocument*>(this)->json_doc_[_key];
    value.json_allocator_ptr_ = &const_cast<ZJsonDocument*>(this)->json_doc_.GetAllocator();
    return value;
}

Void ZJsonDocument::AddMember(const Char* _key, Bool _value, Bool _overwrite_exist) noexcept {
    if (json_doc_.HasMember(_key)) {
        if (_overwrite_exist) {
            json_doc_[_key] = _value;
        }
    }
    else {
        json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
    }
}
Void ZJsonDocument::AddMember(const Char* _key, Int32 _value, Bool _overwrite_exist) noexcept {
    if (json_doc_.HasMember(_key)) {
        if (_overwrite_exist) {
            json_doc_[_key] = _value;
        }
    }
    else {
        json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
    }
}
Void ZJsonDocument::AddMember(const Char* _key, Int64 _value, Bool _overwrite_exist) noexcept {
    if (json_doc_.HasMember(_key)) {
        if (_overwrite_exist) {
            json_doc_[_key] = _value;
        }
    }
    else {
        json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
    }
}
Void ZJsonDocument::AddMember(const Char* _key, UInt32 _value, Bool _overwrite_exist) noexcept {
    if (json_doc_.HasMember(_key)) {
        if (_overwrite_exist) {
            json_doc_[_key] = _value;
        }
    }
    else {
        json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
    }
}
Void ZJsonDocument::AddMember(const Char* _key, UInt64 _value, Bool _overwrite_exist) noexcept {
    if (json_doc_.HasMember(_key)) {
        if (_overwrite_exist) {
            json_doc_[_key] = _value;
        }
    }
    else {
        json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
    }
}
Void ZJsonDocument::AddMember(const Char* _key, Float32 _value, Bool _overwrite_exist) noexcept {
    if (json_doc_.HasMember(_key)) {
        if (_overwrite_exist) {
            json_doc_[_key] = _value;
        }
    }
    else {
        json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
    }
}
Void ZJsonDocument::AddMember(const Char* _key, Float64 _value, Bool _overwrite_exist) noexcept {
    if (json_doc_.HasMember(_key)) {
        if (_overwrite_exist) {
            json_doc_[_key] = _value;
        }
    }
    else {
        json_doc_.AddMember(StringRef(_key), _value, json_doc_.GetAllocator());
    }
}
Void ZJsonDocument::AddMember(const Char* _key, const Char* _value, Bool _overwrite_exist) noexcept {
    if (json_doc_.HasMember(_key)) {
        if (_overwrite_exist) {
            json_doc_[_key] = internal::JsonValue(_value, json_doc_.GetAllocator());
        }
    }
    else {
        json_doc_.AddMember(
            StringRef(_key), 
            internal::JsonValue(_value, json_doc_.GetAllocator()), 
            json_doc_.GetAllocator()
        );
    }
}
Void ZJsonDocument::AddMember(const Char* _key, const ZJsonValue& _value, Bool _overwrite_exist) noexcept {
    if (json_doc_.HasMember(_key)) {
        if (_overwrite_exist) {
            json_doc_[_key] = internal::JsonValue(*_value.json_value_ptr_, json_doc_.GetAllocator());
        }
    }
    else {
        json_doc_.AddMember(
            StringRef(_key), 
            internal::JsonValue(*_value.json_value_ptr_, json_doc_.GetAllocator()),
            json_doc_.GetAllocator()
        );
    }
}
Void ZJsonDocument::AddMember(const Char* _key, ZJsonValue&& _value, Bool _overwrite_exist) noexcept {
    if (json_doc_.HasMember(_key)) {
        if (_overwrite_exist) {
            json_doc_[_key] = internal::JsonValue(
                std::move(*_value.json_value_ptr_), json_doc_.GetAllocator()
            );
        }
    }
    else {
        json_doc_.AddMember(
            StringRef(_key), 
            std::move(*_value.json_value_ptr_),
            json_doc_.GetAllocator()
        );
    }
}
Void ZJsonDocument::AddMember(const Char* _key, const ZJsonDocument& _doc, Bool _overwrite_exist) noexcept {
    if (json_doc_.HasMember(_key)) {
        if (_overwrite_exist) {
            json_doc_[_key] = internal::JsonValue(_doc.json_doc_, json_doc_.GetAllocator());
        }
    }
    else {
        json_doc_.AddMember(
            StringRef(_key), 
            internal::JsonValue(_doc.json_doc_, json_doc_.GetAllocator()),
            json_doc_.GetAllocator()
        );
    }
}
Void ZJsonDocument::AddMember(const Char* _key, ZJsonDocument&& _doc, Bool _overwrite_exist) noexcept {
    if (json_doc_.HasMember(_key)) {
        if (_overwrite_exist) {
            json_doc_[_key] = internal::JsonValue(
                std::move(_doc.json_doc_), json_doc_.GetAllocator()
            );
        }
    }
    else {
        json_doc_.AddMember(
            StringRef(_key), std::move(_doc.json_doc_), json_doc_.GetAllocator()
        );
    }
}


NODISCARD ReturnType ZJsonDocument::Parse(const Char* _str) noexcept {
    ReturnType ret_val = kOK;

    json_doc_.Parse(_str);

    if (json_doc_.HasParseError()) {
        ret_val = error_code::kZJsonErrorCode_JsonParseError;
        Z_LOG_ERROR(ret_val, 0, "Json prase error! pos: %d", json_doc_.GetErrorOffset());
        return ret_val;
    }

    return ret_val;
}

NODISCARD ZString ZJsonDocument::GenerateJsonString() const noexcept {
    if (json_doc_.IsNull()) {
        return ZString("");
    }

   //json to string
    JsonStringBuffer buffer;
    rapidjson::PrettyWriter<JsonStringBuffer> writer(buffer);
    writer.SetIndent(' ', 4);
    json_doc_.Accept(writer);
    ZString str = buffer.GetString();

    return str;
}

NODISCARD ReturnType ZJsonDocument::ReadFile(const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    ZFile file;

    //open file
    link_code = file.Open(_path_dir, ZFile::kOpenTypeReadBin);
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZFile::Open() link error!");
        return ret_val;
    }

    //read json string from the file
    Int32 json_str_size = static_cast<Int32>(file.Size());
    ZMemory json_raw_str(json_str_size + 1);
    link_code = file.Read(json_raw_str.DataPtr<Void>(), json_str_size);
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZFile::Read() link error!");
        return ret_val;
    }
    json_raw_str.DataPtr<Char>()[json_str_size] = '\0';

    //parse the string
    link_code = Parse(json_raw_str.DataPtr<Char>());
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZJsonDocument::Parse() link error!");
        return ret_val;
    }

    //close file
    link_code = file.Close();
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZFile::Close() link error!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZJsonDocument::WriteFile(const Char* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    ZFile file;

    //open file
    link_code = file.OpenSafe(_path_dir, ZFile::kOpenTypeWriteBin);
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZFile::Open() link error!");
        return ret_val;
    }

    //json to string
    ZString json_str = GenerateJsonString();

    //write string to file
    link_code = file.Write(static_cast<const Void*>(json_str.DataPtr()), json_str.Size());
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZFile::Write() link error!");
        return ret_val;
    }

    //close file
    link_code = file.Close();
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZFile::Close() link error!");
        return ret_val;
    }
    return ret_val;
}

}//zengine