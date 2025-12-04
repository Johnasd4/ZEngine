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

#include "z_udp_message.h"

namespace zengine {
namespace socket {

NODISCARD ReturnType ZUDPMessage::OnSerialize(ZSerializer& _out) const noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    link_code = _out.Write(&message_type_, MessageHeaderSize);
    if (link_code != kOK) {
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZSerializer::Write() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

NODISCARD ReturnType ZUDPMessage::OnDeserialize(ZDeserializer& _in) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    link_code = _in.Read(&message_type_, MessageHeaderSize);
    if (link_code != kOK) {
        ret_val = error_code::kSocketErrorCode_LinkError;
        Z_LOG_ERROR(
            ret_val, link_code,
            L"ZDeserializer::Read() link error!"
        );
        return ret_val;
    }

    return ret_val;
}

ZUDPMessage::ZUDPMessage(UInt32 _message_type) noexcept
    : SuperType_()
    , message_type_(_message_type)
{}

ZUDPMessage::~ZUDPMessage() noexcept {}

}//socket
}//zengine