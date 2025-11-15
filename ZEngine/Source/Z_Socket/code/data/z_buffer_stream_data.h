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

#include <boost/asio.hpp>

#include "z_core/t_allocator.h"
#include "z_core/z_object.h"

namespace zengine {
namespace socket {
namespace internal {

struct ZBufferStreamData : public ZObject {
public:
    ZBufferStreamData() noexcept;
    ZBufferStreamData(SizeType _prepare_size) noexcept;
    ZBufferStreamData(SizeType _prepare_size, SizeType _max_size) noexcept;

protected:
    using SuperType_ = ZObject;

public:
    boost::asio::basic_streambuf<TAllocator<Char>> buffer_;
    SizeType read_size_;
};

}//internal
}//socket
}//zengine

