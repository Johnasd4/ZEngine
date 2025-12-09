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
#define SOCKET_DLLFILE
#include "drive/d_pch.h"

#include "z_tls_context.h"

#include "data/z_tls_context_data.h"

namespace zengine {
namespace socket {

ZTLSContext::ZTLSContext(TLSTypeEnum _tls_type) noexcept
    : SuperType_()
    , data_ptr_(MakeUnique<internal::ZTLSContextData>(_tls_type))
    , state_(StateEnum_::kUninitialized)
    , tls_type_(_tls_type)
{
    if (tls_type_ != TLSTypeEnum::kClient && tls_type_ != TLSTypeEnum::kServer) {
        Z_LOG_ERROR(
            error_code::kSocketErrorCode_TLSVerifyModeNotValid, 0,
            "TLS type not valid! _tls_type: %d", _tls_type
        );
        return;
    }

    state_ = StateEnum_::kInitialized;
}

ZTLSContext::~ZTLSContext() noexcept {}

NODISCARD ReturnType ZTLSContext::SetVerifyMode(TLSVerifyModeEnum _tls_verify_mode) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kInitialized,
        error_code::kSocketErrorCode_StateError,
        "TLS context state error! state: %d expect state: %d",
        state_, StateEnum_::kInitialized
    );

    Int32 verify_mode;

    switch (_tls_verify_mode) {
    case TLSVerifyModeEnum::kNone:
        verify_mode = boost::asio::ssl::context::verify_none;
        break;
    case TLSVerifyModeEnum::kPeer:
        verify_mode = boost::asio::ssl::context::verify_peer;
        if (tls_type_ == TLSTypeEnum::kServer) {
            verify_mode |= boost::asio::ssl::context::verify_fail_if_no_peer_cert;
        }
        break;
    default:
        ret_val = error_code::kSocketErrorCode_TLSVerifyModeNotValid;
        Z_LOG_ERROR(
            ret_val, 0,
            "TLS verify mode not valid! _tls_verify_mode: %d", _tls_verify_mode
        );
    }

    data_ptr_->tls_context_.set_verify_mode(verify_mode, error_code);
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr()
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTLSContext::LoadVerifyFile(const Char* _file_dir) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kInitialized,
        error_code::kSocketErrorCode_StateError,
        "TLS context state error! state: %d expect state: %d",
        state_, StateEnum_::kInitialized
    );

    data_ptr_->tls_context_.load_verify_file(_file_dir);
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr()
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTLSContext::LoadSystemVerifyFiles() noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kInitialized,
        error_code::kSocketErrorCode_StateError,
        "TLS context state error! state: %d expect state: %d",
        state_, StateEnum_::kInitialized
    );

    data_ptr_->tls_context_.set_default_verify_paths(error_code);
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr()
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZTLSContext::UseCertificateFile(
    const Char* _file_dir,
    CertificateFileFormatTypeEnum _file_format
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kInitialized,
        error_code::kSocketErrorCode_StateError,
        "TLS context state error! state: %d expect state: %d",
        state_, StateEnum_::kInitialized
    );

    boost::asio::ssl::context_base::file_format file_format;

    switch (_file_format) {
    case CertificateFileFormatTypeEnum::kASN_1:
        file_format = boost::asio::ssl::context::asn1;
        break;
    case CertificateFileFormatTypeEnum::kPEM:
        file_format = boost::asio::ssl::context::pem;
        break;
    default:
        ret_val = error_code::kSocketErrorCode_CertificateFileFormatNotValid;
        Z_LOG_ERROR(
            ret_val, 0,
            "Certificate file format not valid! _file_format: %d", _file_format
        );
        return ret_val;
    }

    data_ptr_->tls_context_.use_certificate_file(_file_dir, file_format, error_code);
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr()
        );
        return ret_val;
    }

    certificate_loaded_ = true;

    return ret_val;
}

NODISCARD ReturnType ZTLSContext::UseCertificateChainFile(
    const Char* _file_dir
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kInitialized,
        error_code::kSocketErrorCode_StateError,
        "TLS context state error! state: %d expect state: %d",
        state_, StateEnum_::kInitialized
    );

    data_ptr_->tls_context_.use_certificate_chain_file(_file_dir, error_code);
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr()
        );
        return ret_val;
    }

    certificate_loaded_ = true;

    return ret_val;
}

NODISCARD ReturnType ZTLSContext::UsePrivateKeyFile(
    const Char* _file_dir,
    CertificateFileFormatTypeEnum _file_format
) noexcept {
    ReturnType ret_val = kOK;
    boost::system::error_code error_code;

    Z_CHECK(
        state_ != StateEnum_::kInitialized,
        error_code::kSocketErrorCode_StateError,
        "TLS context state error! state: %d expect state: %d",
        state_, StateEnum_::kInitialized
    );

    boost::asio::ssl::context_base::file_format file_format;

    switch (_file_format) {
    case CertificateFileFormatTypeEnum::kASN_1:
        file_format = boost::asio::ssl::context::asn1;
        break;
    case CertificateFileFormatTypeEnum::kPEM:
        file_format = boost::asio::ssl::context::pem;
        break;
    default:
        ret_val = error_code::kSocketErrorCode_CertificateFileFormatNotValid;
        Z_LOG_ERROR(
            ret_val, 0,
            "Certificate file format not valid! _file_format: %d", _file_format
        );
        return ret_val;
    }

    data_ptr_->tls_context_.use_private_key_file(_file_dir, file_format, error_code);
    if (error_code) {
        ret_val = error_code::kSocketErrorCode_SystemError;
        Z_LOG_ERROR(
            ret_val, error_code.value(),
            "System error! error info: %ls",
            string::StringToWString(error_code.message().c_str()).DataPtr()
        );
        return ret_val;
    }

    private_key_loaded_ = true;

    return ret_val;
}

}//socket
}//zengine