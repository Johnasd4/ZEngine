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

#include "z_json.h"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/allocators.h>
#include <rapidjson/prettywriter.h>

#include "m_log.h"
#include "z_file.h"
#include "z_memory.h"

using namespace rapidjson;

namespace zengine {

namespace internal {

class internal::JsonAllocatorP {
public:
    static constexpr Bool kNeedFree = true;

    Void* Malloc(SizeType _size) noexcept {
        if (_size == 0U) {
            return nullptr;
        }
        return memory_pool::ApplyMemory(static_cast<MemoryType>(_size));
    }

    Void* Realloc(Void* _old_ptr, SizeType _old_size, SizeType _new_size) noexcept {
        if (_new_size == 0U) {
            memory_pool::ReleaseMemory(_old_ptr);
            return nullptr;
        }
        return memory_pool::ReapplyMemory(_old_ptr, static_cast<MemoryType>(_new_size));
    }

    static Void Free(Void* _mem_ptr) noexcept {
        memory_pool::ReleaseMemory(_mem_ptr);
    }
};

class internal::JsonDocument
    : public rapidjson::GenericDocument<rapidjson::UTF8<>, internal::JsonAllocator, internal::JsonAllocatorP>
    , public ZObject
{};

}//internal

using JsonStringBuffer = rapidjson::GenericStringBuffer<rapidjson::UTF8<>, internal::JsonAllocatorP>;

static internal::JsonAllocator& AllocatorInstance() {
    static internal::JsonAllocator instance;
    return instance;
}

ZJsonValue::ZJsonValue() noexcept 
    : json_value_ptr_()
    , need_delete_(false) 
{}
ZJsonValue::ZJsonValue(ZJsonValue&& _value) noexcept 
    : json_value_ptr_(_value.json_value_ptr_)
    , need_delete_(_value.need_delete_)
{
    _value.json_value_ptr_ = nullptr;
    _value.need_delete_ = false;
}

ZJsonValue::ZJsonValue(const Char* _key, Int32 _value) noexcept 
    : json_value_ptr_(new internal::JsonValue())
    , need_delete_(true)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
ZJsonValue::ZJsonValue(const Char* _key, Int64 _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , need_delete_(true)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
ZJsonValue::ZJsonValue(const Char* _key, UInt32 _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , need_delete_(true)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
ZJsonValue::ZJsonValue(const Char* _key, UInt64 _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , need_delete_(true)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
ZJsonValue::ZJsonValue(const Char* _key, Float32 _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , need_delete_(true)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
ZJsonValue::ZJsonValue(const Char* _key, Float64 _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , need_delete_(true)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
ZJsonValue::ZJsonValue(const Char* _key, const Char* _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , need_delete_(true)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), internal::JsonValue(_value, AllocatorInstance()), AllocatorInstance());
}
ZJsonValue::ZJsonValue(const Char* _key, const ZJsonValue& _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , need_delete_(true)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(StringRef(_key), *_value.json_value_ptr_, AllocatorInstance());
}
ZJsonValue::ZJsonValue(const Char* _key, ZJsonValue&& _value) noexcept
    : json_value_ptr_(new internal::JsonValue())
    , need_delete_(true)
{
    json_value_ptr_->SetObject();
    json_value_ptr_->AddMember(
        StringRef(_key), std::forward<internal::JsonValue>(*_value.json_value_ptr_), AllocatorInstance()
    );
}

ZJsonValue::~ZJsonValue() noexcept {
    if (need_delete_) {
        delete json_value_ptr_;
    }
}

ZJsonValue& ZJsonValue::operator=(Int32 _value) noexcept {
    CheckPointerP();
    *json_value_ptr_ = _value;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(Int64 _value) noexcept {
    CheckPointerP();
    *json_value_ptr_ = _value;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(UInt32 _value) noexcept {
    CheckPointerP();
    *json_value_ptr_ = _value;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(UInt64 _value) noexcept {
    CheckPointerP();
    *json_value_ptr_ = _value;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(Float32 _value) noexcept {
    CheckPointerP();
    *json_value_ptr_ = _value;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(Float64 _value) noexcept {
    CheckPointerP();
    *json_value_ptr_ = _value;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(const Char* _value) noexcept {
    CheckPointerP();
    *json_value_ptr_ = internal::JsonValue(_value, AllocatorInstance());
    return *this;
}
ZJsonValue& ZJsonValue::operator=(const ZJsonValue& _value) noexcept {
    CheckPointerP();
    *json_value_ptr_ = *_value.json_value_ptr_;
    return *this;
}
ZJsonValue& ZJsonValue::operator=(ZJsonValue&& _value) noexcept {
    CheckPointerP();
    *json_value_ptr_ = std::move(*_value.json_value_ptr_);
    return *this;
}

NODISCARD ZJsonValue ZJsonValue::operator[](const Char* _key) noexcept {
    CheckPointerP();
    if (!HasKey(_key)) {
        json_value_ptr_->AddMember(StringRef(_key), 0, AllocatorInstance());
    }
    ZJsonValue value;
    value.json_value_ptr_ = &(*json_value_ptr_)[_key];
    return value;
    return value;
}

Void ZJsonValue::AddKey(const Char* _key, Int32 _value) noexcept {
    CheckPointerP();
    json_value_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
Void ZJsonValue::AddKey(const Char* _key, Int64 _value) noexcept {
    CheckPointerP();
    json_value_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
Void ZJsonValue::AddKey(const Char* _key, UInt32 _value) noexcept {
    CheckPointerP();
    json_value_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
Void ZJsonValue::AddKey(const Char* _key, UInt64 _value) noexcept {
    CheckPointerP();
    json_value_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
Void ZJsonValue::AddKey(const Char* _key, Float32 _value) noexcept {
    CheckPointerP();
    json_value_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
Void ZJsonValue::AddKey(const Char* _key, Float64 _value) noexcept {
    CheckPointerP();
    json_value_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
Void ZJsonValue::AddKey(const Char* _key, const Char* _value) noexcept {
    CheckPointerP();
    json_value_ptr_->AddMember(StringRef(_key), internal::JsonValue(_value, AllocatorInstance()), AllocatorInstance());
}
Void ZJsonValue::AddKey(const Char* _key, const ZJsonValue& _value) noexcept {
    CheckPointerP();
    json_value_ptr_->AddMember(StringRef(_key), *_value.json_value_ptr_, AllocatorInstance());
}
Void ZJsonValue::AddKey(const Char* _key, ZJsonValue&& _value) noexcept {
    CheckPointerP();
    json_value_ptr_->AddMember(
        StringRef(_key), std::forward<internal::JsonValue>(*_value.json_value_ptr_), AllocatorInstance()
    );
}

NODISCARD Bool ZJsonValue::RemoveKey(const Char* _key) noexcept {
    return json_value_ptr_ ? json_value_ptr_->RemoveMember(_key) : false;
}

NODISCARD Bool ZJsonValue::HasKey(const Char* _key) noexcept {
    return json_value_ptr_ ? json_value_ptr_->HasMember(_key) : false;
}

Void ZJsonValue::Clear() noexcept {
    if (json_value_ptr_) {
        json_value_ptr_->Clear();
    }
}

NODISCARD ZString ZJsonValue::GenerateJsonString() noexcept {
    CheckPointerP();

    //json to string
    JsonStringBuffer buffer;
    rapidjson::PrettyWriter<JsonStringBuffer> writer(buffer);
    writer.SetIndent(' ', 4);
    json_value_ptr_->Accept(writer);
    ZString str = buffer.GetString();

    return str;
}

Void ZJsonValue::CheckPointerP() noexcept {
    if (json_value_ptr_ == nullptr) {
        json_value_ptr_ = new internal::JsonValue();
        json_value_ptr_->SetObject();
        need_delete_ = true;
    }
}

ZJsonDocument::ZJsonDocument() noexcept
    : json_doc_ptr_()
    , need_delete_(false)
{}
ZJsonDocument::ZJsonDocument(ZJsonDocument&& _value) noexcept
    : json_doc_ptr_(_value.json_doc_ptr_)
    , need_delete_(_value.need_delete_)
{
    _value.json_doc_ptr_ = nullptr;
    _value.need_delete_ = false;
}

ZJsonDocument::ZJsonDocument(const Char* _key, Int32 _value) noexcept
    : json_doc_ptr_(new internal::JsonDocument())
    , need_delete_(true)
{
    json_doc_ptr_->SetObject();
    json_doc_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
ZJsonDocument::ZJsonDocument(const Char* _key, Int64 _value) noexcept
    : json_doc_ptr_(new internal::JsonDocument())
    , need_delete_(true)
{
    json_doc_ptr_->SetObject();
    json_doc_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
ZJsonDocument::ZJsonDocument(const Char* _key, UInt32 _value) noexcept
    : json_doc_ptr_(new internal::JsonDocument())
    , need_delete_(true)
{
    json_doc_ptr_->SetObject();
    json_doc_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
ZJsonDocument::ZJsonDocument(const Char* _key, UInt64 _value) noexcept
    : json_doc_ptr_(new internal::JsonDocument())
    , need_delete_(true)
{
    json_doc_ptr_->SetObject();
    json_doc_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
ZJsonDocument::ZJsonDocument(const Char* _key, Float32 _value) noexcept
    : json_doc_ptr_(new internal::JsonDocument())
    , need_delete_(true)
{
    json_doc_ptr_->SetObject();
    json_doc_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
ZJsonDocument::ZJsonDocument(const Char* _key, Float64 _value) noexcept
    : json_doc_ptr_(new internal::JsonDocument())
    , need_delete_(true)
{
    json_doc_ptr_->SetObject();
    json_doc_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
ZJsonDocument::ZJsonDocument(const Char* _key, const Char* _value) noexcept
    : json_doc_ptr_(new internal::JsonDocument())
    , need_delete_(true)
{
    json_doc_ptr_->SetObject();
    json_doc_ptr_->AddMember(StringRef(_key), internal::JsonValue(_value, AllocatorInstance()), AllocatorInstance());
}
ZJsonDocument::ZJsonDocument(const Char* _key, const ZJsonDocument& _value) noexcept
    : json_doc_ptr_(new internal::JsonDocument())
    , need_delete_(true)
{
    json_doc_ptr_->SetObject();
    json_doc_ptr_->AddMember(StringRef(_key), *_value.json_doc_ptr_, AllocatorInstance());
}
ZJsonDocument::ZJsonDocument(const Char* _key, ZJsonDocument&& _value) noexcept
    : json_doc_ptr_(new internal::JsonDocument())
    , need_delete_(true)
{
    json_doc_ptr_->SetObject();
    json_doc_ptr_->AddMember(
        StringRef(_key), std::forward<internal::JsonValue>(*_value.json_doc_ptr_), AllocatorInstance()
    );
}

ZJsonDocument::~ZJsonDocument() noexcept {
    if (need_delete_) {
        delete json_doc_ptr_;
    }
}

ZJsonDocument& ZJsonDocument::operator=(ZJsonDocument&& _value) noexcept {
    json_doc_ptr_ = _value.json_doc_ptr_;
    need_delete_ = _value.need_delete_;
    _value.json_doc_ptr_ = nullptr;
    _value.need_delete_ = false;
    return *this;
}

NODISCARD ZJsonValue ZJsonDocument::operator[](const Char* _key) noexcept {
    CheckPointerP();
    if (!HasKey(_key)) {
        json_doc_ptr_->AddMember(StringRef(_key), 0, AllocatorInstance());
    }
    ZJsonValue value;
    value.json_value_ptr_ = &(*json_doc_ptr_)[_key];
    return value;
}

Void ZJsonDocument::AddKey(const Char* _key, Int32 _value) noexcept {
    CheckPointerP();
    json_doc_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
Void ZJsonDocument::AddKey(const Char* _key, Int64 _value) noexcept {
    CheckPointerP();
    json_doc_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
Void ZJsonDocument::AddKey(const Char* _key, UInt32 _value) noexcept {
    CheckPointerP();
    json_doc_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
Void ZJsonDocument::AddKey(const Char* _key, UInt64 _value) noexcept {
    CheckPointerP();
    json_doc_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
Void ZJsonDocument::AddKey(const Char* _key, Float32 _value) noexcept {
    CheckPointerP();
    json_doc_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
Void ZJsonDocument::AddKey(const Char* _key, Float64 _value) noexcept {
    CheckPointerP();
    json_doc_ptr_->AddMember(StringRef(_key), _value, AllocatorInstance());
}
Void ZJsonDocument::AddKey(const Char* _key, const Char* _value) noexcept {
    CheckPointerP();
    json_doc_ptr_->AddMember(StringRef(_key), internal::JsonValue(_value, AllocatorInstance()), AllocatorInstance());
}
Void ZJsonDocument::AddKey(const Char* _key, const ZJsonDocument& _value) noexcept {
    CheckPointerP();
    json_doc_ptr_->AddMember(StringRef(_key), *_value.json_doc_ptr_, AllocatorInstance());
}
Void ZJsonDocument::AddKey(const Char* _key, ZJsonDocument&& _value) noexcept {
    CheckPointerP();
    json_doc_ptr_->AddMember(
        StringRef(_key), std::forward<internal::JsonValue>(*_value.json_doc_ptr_), AllocatorInstance()
    );
}

NODISCARD Bool ZJsonDocument::RemoveKey(const Char* _key) noexcept {
    return json_doc_ptr_ ? json_doc_ptr_->RemoveMember(_key) : false;
}

NODISCARD Bool ZJsonDocument::HasKey(const Char* _key) noexcept {
    return json_doc_ptr_ ? json_doc_ptr_->HasMember(_key) : false;
}

Void ZJsonDocument::Clear() noexcept {
    if (json_doc_ptr_) {
        json_doc_ptr_->Clear();
    }
}

NODISCARD ReturnType ZJsonDocument::Parse(const Char* _str) noexcept {
    CheckPointerP();
    ReturnType ret_val = kOK;

    json_doc_ptr_->Parse(_str);

    if (json_doc_ptr_->HasParseError()) {
        ret_val = error_code::kZJsonErrorCode_JsonParseError;
        Z_LOG_ERROR(ret_val, 0, L"Json prase error! pos: %d", json_doc_ptr_->GetErrorOffset());
        return ret_val;
    }

    return ret_val;
}

NODISCARD ZString ZJsonDocument::GenerateJsonString() noexcept {
    CheckPointerP();

   //json to string
    JsonStringBuffer buffer;
    rapidjson::PrettyWriter<JsonStringBuffer> writer(buffer);
    writer.SetIndent(' ', 4);
    json_doc_ptr_->Accept(writer);
    ZString str = buffer.GetString();

    return str;
}

NODISCARD ReturnType ZJsonDocument::ReadFile(const WChar* _path_dir) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    ZFile file;

    //open file
    link_code = file.Open(_path_dir, ZFile::kOpenTypeReadBin);
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZFile::Open() link error!");
        return ret_val;
    }

    //read json string from the file
    MemoryType json_str_size = static_cast<MemoryType>(file.Size());
    ZMemory json_raw_str(json_str_size + 1);
    link_code = file.Read(json_raw_str.DataPtr(), json_str_size);
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZFile::Read() link error!");
        return ret_val;
    }
    json_raw_str.DataPtr()[json_str_size] = '\0';

    //parse the string
    link_code = Parse(reinterpret_cast<Char*>(json_raw_str.DataPtr()));
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZJsonDocument::Parse() link error!");
        return ret_val;
    }

    //close file
    link_code = file.Close();
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZFile::Close() link error!");
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZJsonDocument::WriteFile(const WChar* _path_dir) noexcept {
    CheckPointerP();
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    ZFile file;

    //open file
    link_code = file.Open(_path_dir, ZFile::kOpenTypeWriteBin);
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZFile::Open() link error!");
        return ret_val;
    }

    //json to string
    ZString json_str = GenerateJsonString();

    //write string to file
    link_code = file.Write(static_cast<const Void*>(json_str.String()), json_str.Size());
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZFile::Write() link error!");
        return ret_val;
    }

    //close file
    link_code = file.Close();
    if (link_code != kOK) {
        ret_val = error_code::kZJsonErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZFile::Close() link error!");
        return ret_val;
    }
    return ret_val;
}

Void ZJsonDocument::CheckPointerP() noexcept {
    if (json_doc_ptr_ == nullptr) {
        json_doc_ptr_ = new internal::JsonDocument();
        json_doc_ptr_->SetObject();
        need_delete_ = true;
    }
}

}//zengine