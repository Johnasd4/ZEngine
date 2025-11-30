/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creatiarrayommons.org/licenses/by-nc/4.0/

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

#include "internal/t_order_data_iterator.h"
#include "t_lock_guard.h"
#include "z_object.h"

namespace zengine {

/*
    Array view, initialize with a object pointer.
    This is only a view, does't apply any heap memory.
*/
template<typename _ObjectType>
class TArrayView : public ZObject {
public:
    using ConstIterator_ = internal::TOrdetDataConstIterator;
    using ConstReverseIterator_ = internal::TOrdetDataConstReverseIterator;

    FORCEINLINE TArrayView() noexcept
        : SuperType_()
        , data_ptr_(nullptr)
        , size_(0ULL)
    {}
    FORCEINLINE TArrayView(const TArrayView& _view) noexcept
        : SuperType_(_view)
        , data_ptr_(_view.data_ptr_)
        , size_(_view.size_)
    {}
    FORCEINLINE TArrayView(TArrayView&& _view) noexcept
        : SuperType_(std::forward<TArrayView>(_view))
        , data_ptr_(_view.data_ptr_)
        , size_(_view.size_)
    {
        _view.size_ = 0ULL;
    }
    FORCEINLINE TArrayView(_ObjectType* _data_ptr, SizeType _size) noexcept
        : SuperType_()
        , data_ptr_(_data_ptr)
        , size_(_size)
    {}
    FORCEINLINE ~TArrayView() noexcept {}

    FORCEINLINE TArrayView& operator=(const TArrayView& _view) noexcept {
        SuperType_::operator=(_view);
        data_ptr_ = _view.data_ptr_;
        size_ = _view.size_;
        return *this;
    }
    FORCEINLINE TArrayView& operator=(TArrayView&& _view) noexcept {
        SuperType_::operator=(std::forward<TArrayView>(_view));
        data_ptr_ = _view.data_ptr_;
        size_ = _view.size_;
        _view.size_ = 0ULL;
        return *this;
    }

    FORCEINLINE Void Assign(_ObjectType* _data_ptr, SizeType _size) noexcept {
        data_ptr_ = _data_ptr;
        size_ = _size;
    }

    NODISCARD FORCEINLINE Bool operator==(const TArrayView& _view) noexcept {
        return data_ptr_ == _view.data_ptr_ && size_ == _view.size_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TArrayView& _view) noexcept {
        return data_ptr_ != _view.data_ptr_ || size_ != _view.size_;
    }

    NODISCARD FORCEINLINE const _ObjectType& operator[](SizeType _index) const noexcept { return data_ptr_[_index]; }

    NODISCARD FORCEINLINE const _ObjectType& At(SizeType _index) const noexcept { return data_ptr_[_index]; }

    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return data_ptr_[0ULL]; }
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return data_ptr_[size_ - 1ULL]; }
    NODISCARD FORCEINLINE const _ObjectType* DataPtr() const noexcept { return data_ptr_; }

    NODISCARD FORCEINLINE SizeType Size() const noexcept { return size_; }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return size_ == 0ULL; }

    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept {
        return ConstIterator_(data_ptr_);
    }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept {
        return ConstIterator_(data_ptr_);
    }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept {
        return ConstReverseIterator_(data_ptr_ + (size_ - 1ULL));
    }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept {
        return ConstReverseIterator_(data_ptr_ + (size_ - 1ULL));
    }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept {
        return ConstIterator_(data_ptr_ + size_);
    }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept {
        return ConstIterator_(data_ptr_ + size_);
    }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept {
        return Iterator_(data_ptr_ - 1ULL);
    }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept {
        return Iterator_(data_ptr_ - 1ULL);
    }

protected:
    using SuperType_ = ZObject;

private:
    _ObjectType* data_ptr_;
    SizeType size_;
};

}//zengine