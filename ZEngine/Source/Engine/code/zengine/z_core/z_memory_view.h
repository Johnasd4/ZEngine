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

#include "z_object.h"

namespace zengine {

/*
    Memory view.
    This is only a view, does't apply any heap memory.
*/
class ZMemoryView : public ZObject {
public:
    FORCEINLINE ZMemoryView() noexcept
        : SuperType_()
        , data_ptr_(nullptr)
        , size_(0ULL)
    {}
    FORCEINLINE ZMemoryView(const ZMemoryView& _view) noexcept
        : SuperType_(_view)
        , data_ptr_(_view.data_ptr_)
        , size_(_view.size_)
    {}
    FORCEINLINE ZMemoryView(ZMemoryView&& _view) noexcept
        : SuperType_(std::forward<ZMemoryView>(_view))
        , data_ptr_(_view.data_ptr_)
        , size_(_view.size_)
    {
        _view.size_ = 0ULL;
    }
    template<typename PointerType>
    FORCEINLINE ZMemoryView(const PointerType* _data_ptr, SizeType _size) noexcept
        : SuperType_()
        , data_ptr_(reinterpret_cast<const Byte*>(_data_ptr))
        , size_(_size)
    {}
    FORCEINLINE ~ZMemoryView() noexcept {}

    FORCEINLINE ZMemoryView& operator=(const ZMemoryView& _mem) noexcept {
        Copy(data_ptr_, _mem.data_ptr_, size_);
        size_ = _mem.size_;
        return *this;
    }

    template<typename PointerType>
    NODISCARD FORCEINLINE Void Assign(const PointerType* _data_ptr, SizeType _size) noexcept {
        data_ptr_ = reinterpret_cast<const Byte*>(_data_ptr);
        size_ = _size;
    }

    NODISCARD FORCEINLINE const Byte& operator[](const SizeType _index) const noexcept { return data_ptr_[_index]; }

    template<typename _ObjectType>
    NODISCARD FORCEINLINE const _ObjectType& At(const SizeType _index) const noexcept {
        return *(reinterpret_cast<const _ObjectType*>(data_ptr_) + _index);
    }

    template<typename _ObjectType>
    NODISCARD FORCEINLINE _ObjectType* GetDataPtr() const noexcept {
        return reinterpret_cast<const _ObjectType*>(data_ptr_);
    }

    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return size_; }
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return size_; }

protected:
    using SuperType_ = ZObject;

private:
    const Byte* data_ptr_;
    SizeType size_;
};

}//zengine