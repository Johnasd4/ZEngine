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
#include "../z_core/z_object.h"

#include "z_buffer.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZBufferStreamData;

}//internal
}//socket
}//zengine

namespace zengine {
namespace socket {

/*
    Buffer type, initialize with a base buffer size.
*/
class SOCKET_DLLAPI ZBufferStream : public ZObject {
public:
    ZBufferStream() noexcept;
    ZBufferStream(SizeType _prepare_size) noexcept;
    ZBufferStream(SizeType _prepare_size, SizeType _max_size) noexcept;

    ZBufferStream(ZBufferStream&& _buffer) noexcept;

    ~ZBufferStream() noexcept;

    ZBufferStream& operator=(ZBufferStream&& _buffer) noexcept;

    NODISCARD SizeType Size() const noexcept;

    NODISCARD const ZBuffer ReadData() noexcept;
    NODISCARD const ZBuffer ReadData(SizeType _size) noexcept;

    SizeType DumpData() noexcept;
    SizeType DumpData(SizeType _size) noexcept;

    Void Clear() noexcept;

protected:
    using SuperType_ = ZObject;
    friend class ZTCPSocket;

private:
    ZBufferStream(const ZBufferStream&) = delete;
    ZBufferStream& operator=(const ZBufferStream&) = delete;

private:
    TUniquePointer<internal::ZBufferStreamData> buffer_stream_data_ptr_;
};

}//socket
}//zengine