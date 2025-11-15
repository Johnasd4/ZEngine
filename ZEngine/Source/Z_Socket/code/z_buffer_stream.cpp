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

#include "z_buffer_stream.h"

#include "z_core/m_log.h"

#include "data/z_buffer_stream_data.h"

namespace zengine {
namespace socket {

ZBufferStream::ZBufferStream() noexcept
    : SuperType_()
    , buffer_stream_data_ptr_(MakeUnique<internal::ZBufferStreamData>())
{}

ZBufferStream::ZBufferStream(SizeType _prepare_size) noexcept
    : SuperType_()
    , buffer_stream_data_ptr_(MakeUnique<internal::ZBufferStreamData>(_prepare_size))
{}

ZBufferStream::ZBufferStream(SizeType _prepare_size, SizeType _max_size) noexcept
    : SuperType_()
    , buffer_stream_data_ptr_(MakeUnique<internal::ZBufferStreamData>(_prepare_size, _max_size))
{}

ZBufferStream::ZBufferStream(ZBufferStream&& _buffer) noexcept
    : SuperType_(std::forward<ZBufferStream>(_buffer))
    , buffer_stream_data_ptr_(std::move(_buffer.buffer_stream_data_ptr_))
{}

ZBufferStream::~ZBufferStream() noexcept {}

ZBufferStream& ZBufferStream::operator=(ZBufferStream&& _buffer) noexcept {
    buffer_stream_data_ptr_->buffer_.consume(10);
    SuperType_::operator=(std::forward<ZBufferStream>(_buffer));
    buffer_stream_data_ptr_ = std::move(_buffer.buffer_stream_data_ptr_);
    return *this;
}

NODISCARD SizeType ZBufferStream::Size() const noexcept {
    return buffer_stream_data_ptr_->buffer_.data().size() - buffer_stream_data_ptr_->read_size_;
}

NODISCARD const ZBuffer ZBufferStream::ReadData() noexcept {
    ZBuffer buffer;
    buffer.size_ = buffer_stream_data_ptr_->buffer_.data().size() - buffer_stream_data_ptr_->read_size_;
    buffer.data_ptr_ = const_cast<Void*>(reinterpret_cast<const Void*>(
        reinterpret_cast<const Char*>(buffer_stream_data_ptr_->buffer_.data().data()) + 
        buffer_stream_data_ptr_->read_size_
    ));
    buffer_stream_data_ptr_->read_size_ += buffer.size_;
    return buffer;
}

NODISCARD const ZBuffer ZBufferStream::ReadData(SizeType _size) noexcept {
    ZBuffer buffer;
    buffer.size_ = buffer_stream_data_ptr_->buffer_.data().size() - buffer_stream_data_ptr_->read_size_;
    if (buffer.size_ < _size) {
        buffer.size_ = _size;
    }
    buffer.data_ptr_ = const_cast<Void*>(reinterpret_cast<const Void*>(
        reinterpret_cast<const Char*>(buffer_stream_data_ptr_->buffer_.data().data()) +
        buffer_stream_data_ptr_->read_size_
    ));
    buffer_stream_data_ptr_->read_size_ += buffer.size_;
    return buffer;
}

SizeType ZBufferStream::DumpData() noexcept {
    SizeType buffer_size = buffer_stream_data_ptr_->buffer_.data().size();
    SizeType data_size = buffer_size - buffer_stream_data_ptr_->read_size_;
    buffer_stream_data_ptr_->read_size_ = buffer_size;
    return data_size;
}

SizeType ZBufferStream::DumpData(SizeType _size) noexcept {
    SizeType buffer_size = buffer_stream_data_ptr_->buffer_.data().size();
    SizeType data_size = buffer_size - buffer_stream_data_ptr_->read_size_;
    if (_size > data_size) {
        _size = data_size;
    }
    buffer_stream_data_ptr_->read_size_ += _size;
    return _size;
}

Void ZBufferStream::Clear() noexcept {
    buffer_stream_data_ptr_->buffer_.consume(buffer_stream_data_ptr_->buffer_.size());
    buffer_stream_data_ptr_->read_size_ = 0ULL;
}

}//socket
}//zengine