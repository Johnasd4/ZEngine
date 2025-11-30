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

#include "../z_core/t_function.h"
#include "../z_core/t_pool_list.h"
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_object.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZTLSContextData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    TLS context. 
    Client initialize steps:
    1.Call SetVerifyMode() to set verify mode.
    2.Load verify file.  
    Server initialize steps:
    1.Call UseCertificateFile() or UseCertificateChainFile() to set certificate file.
    2.Call UsePrivateKeyFile() to set key.
*/
class SOCKET_DLLAPI ZTLSContext : public ZObject {
public:
    enum class StateEnum_ : Int32 {
        kUninitialized,
        kInitialized
    };

    ZTLSContext(TLSTypeEnum _tls_type) noexcept;
    ~ZTLSContext() noexcept;

    NODISCARD FORCEINLINE StateEnum_ State() const noexcept { return state_; }
    NODISCARD FORCEINLINE TLSTypeEnum TLSType() const noexcept { return tls_type_; }
    NODISCARD FORCEINLINE Bool CertificateLoaded() const noexcept { return certificate_loaded_; }
    NODISCARD FORCEINLINE Bool PrivateKeyLoaded() const noexcept { return private_key_loaded_; }

    NODISCARD ReturnType SetVerifyMode(TLSVerifyModeEnum _tls_verify_mode) noexcept;

    /*
        Load verify file by path, will not clear the current verify files.
    */
    NODISCARD ReturnType LoadVerifyFile(const Char* _file_dir) noexcept;

    /*
        Load verify files from system default path, will not clear the current verify files.
    */
    NODISCARD ReturnType LoadSystemVerifyFiles() noexcept;
    
    NODISCARD ReturnType UseCertificateFile(
        const Char* _file_dir, 
        CertificateFileFormatTypeEnum _file_format
    ) noexcept;

    NODISCARD ReturnType UseCertificateChainFile(
        const Char* _file_dir
    ) noexcept;

    NODISCARD ReturnType UsePrivateKeyFile(
        const Char* _file_dir, 
        CertificateFileFormatTypeEnum _file_format
    ) noexcept;

protected:
    using SuperType_ = ZObject;
    friend class ZTLSStream;

private:
    ZTLSContext(const ZTLSContext&) = delete;
    ZTLSContext(ZTLSContext&&) = delete;
    ZTLSContext& operator=(const ZTLSContext&) = delete;
    ZTLSContext& operator=(ZTLSContext&&) = delete;

private:
    TUniquePointer<internal::ZTLSContextData> data_ptr_;
    StateEnum_ state_;
    TLSTypeEnum tls_type_;
    Bool certificate_loaded_;
    Bool private_key_loaded_;
};

}//socket
}//zengine