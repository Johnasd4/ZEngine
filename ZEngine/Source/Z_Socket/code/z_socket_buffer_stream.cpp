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

#include "z_socket_buffer_stream.h"

#include "data/z_buffer_stream_data.h"

namespace zengine {
namespace socket {

ZSocketBufferStream::ZSocketBufferStream() noexcept
    : SuperType_()
    , data_ptr_(MakeUnique<internal::ZSocketBufferStreamData>())
{}

ZSocketBufferStream::ZSocketBufferStream(SizeType _prepare_size) noexcept
    : SuperType_()
    , data_ptr_(MakeUnique<internal::ZSocketBufferStreamData>(_prepare_size))
{}

ZSocketBufferStream::ZSocketBufferStream(SizeType _prepare_size, SizeType _max_size) noexcept
    : SuperType_()
    , data_ptr_(MakeUnique<internal::ZSocketBufferStreamData>(_prepare_size, _max_size))
{}

ZSocketBufferStream::ZSocketBufferStream(ZSocketBufferStream&& _buffer) noexcept
    : SuperType_(std::forward<ZSocketBufferStream>(_buffer))
    , data_ptr_(std::move(_buffer.data_ptr_))
{}

ZSocketBufferStream::~ZSocketBufferStream() noexcept {}

ZSocketBufferStream& ZSocketBufferStream::operator=(ZSocketBufferStream&& _buffer) noexcept {
    data_ptr_->buffer_.consume(10);
    SuperType_::operator=(std::forward<ZSocketBufferStream>(_buffer));
    data_ptr_ = std::move(_buffer.data_ptr_);
    return *this;
}

NODISCARD SizeType ZSocketBufferStream::Size() const noexcept {
    return data_ptr_->buffer_.data().size() - data_ptr_->read_size_;
}

NODISCARD const ZConstBuffer ZSocketBufferStream::ReadData() noexcept {
    SizeType buffer_size = data_ptr_->buffer_.data().size() - data_ptr_->read_size_;
    Void* buffer_data_ptr = const_cast<Void*>(reinterpret_cast<const Void*>(
        reinterpret_cast<const Char*>(data_ptr_->buffer_.data().data()) + 
        data_ptr_->read_size_
    ));
    data_ptr_->read_size_ += buffer_size;
    return ZConstBuffer(buffer_data_ptr, buffer_size);
}

NODISCARD const ZConstBuffer ZSocketBufferStream::ReadData(SizeType _size) noexcept {
    ZConstBuffer buffer;
    SizeType buffer_size = data_ptr_->buffer_.data().size() - data_ptr_->read_size_;
    if (buffer_size > _size) {
        buffer_size = _size;
    }
    Void* buffer_data_ptr = const_cast<Void*>(reinterpret_cast<const Void*>(
        reinterpret_cast<const Char*>(data_ptr_->buffer_.data().data()) +
        data_ptr_->read_size_
    ));
    data_ptr_->read_size_ += buffer_size;
    return ZConstBuffer(buffer_data_ptr, buffer_size);
}

SizeType ZSocketBufferStream::DumpData() noexcept {
    SizeType buffer_size = data_ptr_->buffer_.data().size();
    SizeType data_size = buffer_size - data_ptr_->read_size_;
    data_ptr_->read_size_ = buffer_size;
    return data_size;
}

SizeType ZSocketBufferStream::DumpData(SizeType _size) noexcept {
    SizeType buffer_size = data_ptr_->buffer_.data().size();
    SizeType data_size = buffer_size - data_ptr_->read_size_;
    if (_size > data_size) {
        _size = data_size;
    }
    data_ptr_->read_size_ += _size;
    return _size;
}

Void ZSocketBufferStream::Reserve(SizeType _size) noexcept {
    data_ptr_->buffer_.prepare(_size);
}

Void ZSocketBufferStream::Clear() noexcept {
    data_ptr_->buffer_.consume(data_ptr_->buffer_.size());
    data_ptr_->read_size_ = 0ULL;
}

}//socket
}//zengine