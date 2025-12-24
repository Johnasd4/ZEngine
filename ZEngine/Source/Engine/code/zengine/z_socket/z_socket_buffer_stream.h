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

#include "../z_core/t_smart_pointer.h"
#include "../z_core/z_buffer.h"
#include "../z_core/z_object.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZSocketBufferStreamData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    Buffer type, initialize with a base buffer size.
    Clear() can only and must be called when a http response is finished and prepared to receive another http response.
*/
class SOCKET_DLLAPI ZSocketBufferStream : public ZObject {
public:
    ZSocketBufferStream() noexcept;
    ZSocketBufferStream(SizeType _prepare_size) noexcept;
    ZSocketBufferStream(SizeType _prepare_size, SizeType _max_size) noexcept;

    ZSocketBufferStream(ZSocketBufferStream&& _buffer) noexcept;

    ~ZSocketBufferStream() noexcept;

    ZSocketBufferStream& operator=(ZSocketBufferStream&& _buffer) noexcept;

    NODISCARD SizeType GetSize() const noexcept;

    NODISCARD const ZConstBuffer ReadData() noexcept;
    NODISCARD const ZConstBuffer ReadData(SizeType _size) noexcept;

    SizeType DumpData() noexcept;
    SizeType DumpData(SizeType _size) noexcept;

    Void Reserve(SizeType _size) noexcept;

    Void Clear() noexcept;

protected:
    using SuperType_ = ZObject;
    friend class ZTCPSocket;
    friend class ZTLSStream;

private:
    ZSocketBufferStream(const ZSocketBufferStream&) = delete;
    ZSocketBufferStream& operator=(const ZSocketBufferStream&) = delete;

private:
    TUniquePointer<internal::ZSocketBufferStreamData> data_ptr_;
};

}//socket
}//zengine