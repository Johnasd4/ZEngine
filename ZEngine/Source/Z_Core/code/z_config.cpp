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

#include "z_config.h"

#include "f_file_system.h"
#include "m_log.h"
#include "z_json.h"
#include "z_system_time.h"

namespace zengine {

ZConfig::ZConfig(const WChar* _file_dir) noexcept 
    : config_data_ptr_(new ZJsonDocument())
    , file_dir_(_file_dir)
{
    ReturnType link_code = kOK;
    if (file_system::PathExist(_file_dir)) {
        link_code = config_data_ptr_->ReadFile(_file_dir);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kZJsonErrorCode_LinkError, link_code, 
                L"ZJsonDocument::ReadFile() link error!"
            );

            //rename broken config file
            file_system::RenameFileByPath(_file_dir, (ZWString(_file_dir) + kBackUpExtension).String());

            //save config
            link_code = SaveConfigP();
            if (link_code != kOK) {
                Z_LOG_ERROR(
                    error_code::kZJsonErrorCode_LinkError, link_code,
                    L"ZJsonDocument::CreateFileByPath() link error!"
                );
                return;
            }

            return;
        }
    }
    else {
        //save config
        link_code = SaveConfigP();
        if (link_code != kOK) {
            Z_LOG_ERROR(
                error_code::kZJsonErrorCode_LinkError, link_code,
                L"ZJsonDocument::CreateFileByPath() link error!"
            );
            return;
        }
    }
}

ZConfig::~ZConfig() noexcept {
    if (config_data_ptr_ != nullptr) {
        delete config_data_ptr_;
    }
}

NODISCARD Bool ZConfig::GetMemberValueBool(const Char* _key) noexcept {
    return config_data_ptr_->HasMember(_key) ? (*config_data_ptr_)[_key].GetBool() : false;
}
NODISCARD Int32 ZConfig::GetMemberValueInt32(const Char* _key) noexcept {
    return config_data_ptr_->HasMember(_key) ? (*config_data_ptr_)[_key].GetInt32() : 0;
}
NODISCARD Int64 ZConfig::GetMemberValueInt64(const Char* _key) noexcept {
    return config_data_ptr_->HasMember(_key) ? (*config_data_ptr_)[_key].GetInt64() : 0LL;
}
NODISCARD UInt32 ZConfig::GetMemberValueUInt32(const Char* _key) noexcept {
    return config_data_ptr_->HasMember(_key) ? (*config_data_ptr_)[_key].GetUInt32() : 0U;
}
NODISCARD UInt64 ZConfig::GetMemberValueUInt64(const Char* _key) noexcept {
    return config_data_ptr_->HasMember(_key) ? (*config_data_ptr_)[_key].GetUInt64() : 0ULL;
}
NODISCARD Float32 ZConfig::GetMemberValueFloat32(const Char* _key) noexcept {
    return config_data_ptr_->HasMember(_key) ? (*config_data_ptr_)[_key].GetFloat32() : 0.0F;
}
NODISCARD Float64 ZConfig::GetMemberValueFloat64(const Char* _key) noexcept {
    return config_data_ptr_->HasMember(_key) ? (*config_data_ptr_)[_key].GetFloat64() : 0.0;
}
NODISCARD const Char* ZConfig::GetMemberValueString(const Char* _key) noexcept {
    return config_data_ptr_->HasMember(_key) ? (*config_data_ptr_)[_key].GetString() : "";
}

NODISCARD ReturnType ZConfig::SetMemberValue(const Char* _key, Bool _value) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (!config_data_ptr_->HasMember(_key)) {
        ret_val = error_code::kZConfigErrorCode_MemberNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Member not exist! key: %ls", string::String2WString(_key).String());
        return ret_val;
    }
    (*config_data_ptr_)[_key] = _value;
    link_code = SaveConfigP();
    if (link_code != kOK) {
        ret_val = error_code::kZConfigErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
        return ret_val;
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::SetMemberValue(const Char* _key, Int32 _value) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (!config_data_ptr_->HasMember(_key)) {
        ret_val = error_code::kZConfigErrorCode_MemberNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Member not exist! key: %ls", string::String2WString(_key).String());
        return ret_val;
    }
    (*config_data_ptr_)[_key] = _value;
    link_code = SaveConfigP();
    if (link_code != kOK) {
        ret_val = error_code::kZConfigErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
        return ret_val;
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::SetMemberValue(const Char* _key, Int64 _value) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (!config_data_ptr_->HasMember(_key)) {
        ret_val = error_code::kZConfigErrorCode_MemberNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Member not exist! key: %ls", string::String2WString(_key).String());
        return ret_val;
    }
    (*config_data_ptr_)[_key] = _value;
    link_code = SaveConfigP();
    if (link_code != kOK) {
        ret_val = error_code::kZConfigErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
        return ret_val;
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::SetMemberValue(const Char* _key, UInt32 _value) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (!config_data_ptr_->HasMember(_key)) {
        ret_val = error_code::kZConfigErrorCode_MemberNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Member not exist! key: %ls", string::String2WString(_key).String());
        return ret_val;
    }
    (*config_data_ptr_)[_key] = _value;
    link_code = SaveConfigP();
    if (link_code != kOK) {
        ret_val = error_code::kZConfigErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
        return ret_val;
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::SetMemberValue(const Char* _key, UInt64 _value) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (!config_data_ptr_->HasMember(_key)) {
        ret_val = error_code::kZConfigErrorCode_MemberNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Member not exist! key: %ls", string::String2WString(_key).String());
        return ret_val;
    }
    (*config_data_ptr_)[_key] = _value;
    link_code = SaveConfigP();
    if (link_code != kOK) {
        ret_val = error_code::kZConfigErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
        return ret_val;
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::SetMemberValue(const Char* _key, Float32 _value) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (!config_data_ptr_->HasMember(_key)) {
        ret_val = error_code::kZConfigErrorCode_MemberNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Member not exist! key: %ls", string::String2WString(_key).String());
        return ret_val;
    }
    (*config_data_ptr_)[_key] = _value;
    link_code = SaveConfigP();
    if (link_code != kOK) {
        ret_val = error_code::kZConfigErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
        return ret_val;
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::SetMemberValue(const Char* _key, Float64 _value) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (!config_data_ptr_->HasMember(_key)) {
        ret_val = error_code::kZConfigErrorCode_MemberNotExist;
        Z_LOG_ERROR(ret_val, 0, L"Member not exist! key: %ls", string::String2WString(_key).String());
        return ret_val;
    }
    (*config_data_ptr_)[_key] = _value;
    link_code = SaveConfigP();
    if (link_code != kOK) {
        ret_val = error_code::kZConfigErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
        return ret_val;
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::SetMemberValue(const Char* _key, const Char* _value) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    (*config_data_ptr_)[_key] = _value;
    link_code = SaveConfigP();
    if (link_code != kOK) {
        ret_val = error_code::kZConfigErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
        return ret_val;
    }
    return ret_val;
}

NODISCARD ReturnType ZConfig::AddMember(const Char* _key, Bool _value, Bool _overwrite_exist) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if ((!config_data_ptr_->HasMember(_key)) || _overwrite_exist) {
        (*config_data_ptr_)[_key] = _value;
        link_code = SaveConfigP();
        if (link_code != kOK) {
            ret_val = error_code::kZConfigErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
            return ret_val;
        }
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::AddMember(const Char* _key, Int32 _value, Bool _overwrite_exist) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if ((!config_data_ptr_->HasMember(_key)) || _overwrite_exist) {
        (*config_data_ptr_)[_key] = _value;
        link_code = SaveConfigP();
        if (link_code != kOK) {
            ret_val = error_code::kZConfigErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
            return ret_val;
        }
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::AddMember(const Char* _key, Int64 _value, Bool _overwrite_exist) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if ((!config_data_ptr_->HasMember(_key)) || _overwrite_exist) {
        (*config_data_ptr_)[_key] = _value;
        link_code = SaveConfigP();
        if (link_code != kOK) {
            ret_val = error_code::kZConfigErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
            return ret_val;
        }
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::AddMember(const Char* _key, UInt32 _value, Bool _overwrite_exist) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if ((!config_data_ptr_->HasMember(_key)) || _overwrite_exist) {
        (*config_data_ptr_)[_key] = _value;
        link_code = SaveConfigP();
        if (link_code != kOK) {
            ret_val = error_code::kZConfigErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
            return ret_val;
        }
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::AddMember(const Char* _key, UInt64 _value, Bool _overwrite_exist) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if ((!config_data_ptr_->HasMember(_key)) || _overwrite_exist) {
        (*config_data_ptr_)[_key] = _value;
        link_code = SaveConfigP();
        if (link_code != kOK) {
            ret_val = error_code::kZConfigErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
            return ret_val;
        }
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::AddMember(const Char* _key, Float32 _value, Bool _overwrite_exist) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if ((!config_data_ptr_->HasMember(_key)) || _overwrite_exist) {
        (*config_data_ptr_)[_key] = _value;
        link_code = SaveConfigP();
        if (link_code != kOK) {
            ret_val = error_code::kZConfigErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
            return ret_val;
        }
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::AddMember(const Char* _key, Float64 _value, Bool _overwrite_exist) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if ((!config_data_ptr_->HasMember(_key)) || _overwrite_exist) {
        (*config_data_ptr_)[_key] = _value;
        link_code = SaveConfigP();
        if (link_code != kOK) {
            ret_val = error_code::kZConfigErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
            return ret_val;
        }
    }
    return ret_val;
}
NODISCARD ReturnType ZConfig::AddMember(const Char* _key, const Char* _value, Bool _overwrite_exist) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if ((!config_data_ptr_->HasMember(_key)) || _overwrite_exist) {
        (*config_data_ptr_)[_key] = _value;
        link_code = SaveConfigP();
        if (link_code != kOK) {
            ret_val = error_code::kZConfigErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
            return ret_val;
        }
    }
    return ret_val;
}

NODISCARD ReturnType ZConfig::RemoveMember(const Char* _key) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (config_data_ptr_->RemoveMember(_key)) {
        link_code = SaveConfigP();
        if (link_code != kOK) {
            ret_val = error_code::kZConfigErrorCode_LinkError;
            Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
            return ret_val;
        }
    }
    return ret_val;
}

NODISCARD ReturnType ZConfig::Clear() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    config_data_ptr_->Clear();
    link_code = SaveConfigP();
    if (link_code != kOK) {
        ret_val = error_code::kZConfigErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZConfig::SaveConfigP() link error!");
        return ret_val;
    }
    return ret_val;
}

NODISCARD ReturnType ZConfig::SaveConfigP() noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    //only once
    static Bool temp = UpdateVersionP();
    UpdateSaveTimeP();

    link_code = config_data_ptr_->WriteFile(file_dir_.String());
    if (link_code != kOK) {
        ret_val = error_code::kZConfigErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, L"ZJsonDocument::WriteFile() link error!");
        return ret_val;
    }
    return ret_val;
}

Bool ZConfig::UpdateVersionP() noexcept {
    if (!config_data_ptr_->HasMember(kVersionKey)) {
        config_data_ptr_->AddMember(kVersionKey, 0);
    }
    else {
        (*config_data_ptr_)[kVersionKey] = (*config_data_ptr_)[kVersionKey].GetInt32() + 1;
    }
    return true;
}

Void ZConfig::UpdateSaveTimeP() noexcept {
    ZSystemTime::Instance().UpdateTime();
    TFixedString<kUpdateTimeStringLength> save_time_str;
    save_time_str.SetString(
        "%04d/%02d/%02d-%02d:%02d:%02d",
        ZSystemTime::Instance().Year(), ZSystemTime::Instance().Month(), ZSystemTime::Instance().Day(),
        ZSystemTime::Instance().Hour(), ZSystemTime::Instance().Min(), ZSystemTime::Instance().Sec());
    (*config_data_ptr_)[kSaveTimeKey] = save_time_str.DataPtr();
}

}//zengine