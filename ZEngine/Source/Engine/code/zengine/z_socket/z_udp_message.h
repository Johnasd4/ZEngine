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

#include "../z_core/t_array.h"
#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_buffer.h"
#include "../z_core/z_memory.h"
#include "../z_core/z_object.h"
#include "../z_core/z_serializer.h"

namespace zengine {
namespace socket {

/*
    Message base class for UDP communication.
*/
struct ZUDPMessage : public ZObject {
private:
    static constexpr SizeType MessageHeaderSize = 8ULL;

public:
    /*
        Calculate the size of the data after expanding the message.
    */
    NODISCARD virtual SizeType CalculateSerializeSize() noexcept = 0;

    /*
        Serialize the message data.
    */
    NODISCARD virtual ReturnType OnSerialize(ZSerializer& _out) const noexcept;
    /*
        Deserialize the message data.
    */
    NODISCARD virtual ReturnType OnDeserialize(ZDeserializer& _in) noexcept;
    
protected:
    using SuperType_ = ZObject;

    ZUDPMessage(UInt32 _message_type) noexcept;
    virtual ~ZUDPMessage() noexcept;

public:
    UInt16 message_type_;
    //Only used for message size below 540B
    UInt16 data_size_;
    UInt32 message_id_;
};

}//socket
}//zengine