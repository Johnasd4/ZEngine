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

#include "../drive.h"

#include "../z_object.h"

namespace zengine {
namespace internal {

template<typename _ObjectType>
class TOrdetDataIterator;
template<typename _ObjectType>
class TOrdetDataConstIterator;
template<typename _ObjectType>
class TOrdetDataReverseIterator;
template<typename _ObjectType>
class TOrdetDataConstReverseIterator;


template<typename _ObjectType>
class TOrdetDataIterator : public ZObject {
public:
    FORCEINLINE TOrdetDataIterator(_ObjectType* _obj_ptr) noexcept 
        : SuperType_()
        , obj_ptr_(_obj_ptr) {}
    FORCEINLINE TOrdetDataIterator(const TOrdetDataIterator& _iterator) noexcept 
        : SuperType_(_iterator)
        , obj_ptr_(_iterator.obj_ptr_) {}
    FORCEINLINE TOrdetDataIterator(TOrdetDataIterator&& _iterator) noexcept 
        : SuperType_(std::forward<TOrdetDataIterator>(_iterator))
        , obj_ptr_(_iterator.obj_ptr_)
    {
        _iterator.obj_ptr_ = nullptr;
    }

    NODISCARD FORCEINLINE Bool operator==(const _ObjectType* _obj_ptr) const noexcept {
        return obj_ptr_ == _obj_ptr;
    }
    NODISCARD FORCEINLINE Bool operator!=(const _ObjectType* _obj_ptr) const noexcept {
        return obj_ptr_ != _obj_ptr;
    }
    NODISCARD FORCEINLINE Bool operator==(const TOrdetDataIterator& _iterator) const noexcept {
        return obj_ptr_ == _iterator.obj_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TOrdetDataIterator& _iterator) const noexcept {
        return obj_ptr_ != _iterator.obj_ptr_;
    }
    FORCEINLINE TOrdetDataIterator& operator=(const TOrdetDataIterator& _iterator) noexcept {
        obj_ptr_ = _iterator.obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataIterator& operator=(TOrdetDataIterator&& _iterator) noexcept {
        obj_ptr_ = _iterator.obj_ptr_;
        _iterator.obj_ptr_ = nullptr;
        return *this;
    }
    FORCEINLINE TOrdetDataIterator& operator++() noexcept {
        ++obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataIterator& operator--() noexcept {
        --obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataIterator operator+=(SizeType _num) noexcept {
        obj_ptr_ += _num;
        return *this;
    }
    FORCEINLINE TOrdetDataIterator& operator-=(SizeType _num) noexcept {
        obj_ptr_ -= _num;
        return *this;
    }
    FORCEINLINE TOrdetDataIterator operator+(SizeType _num) noexcept {
        return TOrdetDataIterator(obj_ptr_ + _num);
    }
    FORCEINLINE TOrdetDataIterator operator-(SizeType _num) noexcept {
        return TOrdetDataIterator(obj_ptr_ - _num);
    }

    NODISCARD FORCEINLINE _ObjectType& operator*() noexcept { return *obj_ptr_; }
    NODISCARD FORCEINLINE const _ObjectType& operator*() const noexcept { return *obj_ptr_; }
    NODISCARD FORCEINLINE _ObjectType* operator->() noexcept { return obj_ptr_; }
    NODISCARD FORCEINLINE const _ObjectType* operator->() const noexcept { return obj_ptr_; }
    NODISCARD FORCEINLINE _ObjectType* Ptr() noexcept { return obj_ptr_; }
    NODISCARD FORCEINLINE const _ObjectType* Ptr() const noexcept { return obj_ptr_; }

protected:
    using SuperType_ = ZObject;
    friend class TOrdetDataConstIterator<_ObjectType>;

private:
    _ObjectType* obj_ptr_;
};

template<typename _ObjectType>
class TOrdetDataConstIterator : public ZObject {
public:
    FORCEINLINE TOrdetDataConstIterator(_ObjectType* _obj_ptr) noexcept 
        : SuperType_()
        , obj_ptr_(_obj_ptr) {}
    FORCEINLINE TOrdetDataConstIterator(const TOrdetDataConstIterator& _iterator) noexcept 
        : SuperType_(_iterator)
        , obj_ptr_(_iterator.obj_ptr_) {}
    FORCEINLINE TOrdetDataConstIterator(TOrdetDataConstIterator&& _iterator) noexcept 
        : SuperType_(std::forward<TOrdetDataConstIterator>(_iterator))
        , obj_ptr_(_iterator.obj_ptr_)
    {
        _iterator.obj_ptr_ = nullptr;
    }
    FORCEINLINE TOrdetDataConstIterator(const TOrdetDataIterator<_ObjectType>& _iterator) noexcept
        : SuperType_()
        , obj_ptr_(_iterator.obj_ptr_) {
    }
    FORCEINLINE TOrdetDataConstIterator(TOrdetDataIterator<_ObjectType>&& _iterator) noexcept
        : SuperType_()
        , obj_ptr_(_iterator.obj_ptr_)
    {
        _iterator.obj_ptr_ = nullptr;
    }

    NODISCARD FORCEINLINE Bool operator==(const _ObjectType* _obj_ptr) const noexcept {
        return obj_ptr_ == _obj_ptr;
    }
    NODISCARD FORCEINLINE Bool operator!=(const _ObjectType* _obj_ptr) const noexcept {
        return obj_ptr_ != _obj_ptr;
    }
    NODISCARD FORCEINLINE Bool operator==(const TOrdetDataConstIterator& _iterator) const noexcept {
        return obj_ptr_ == _iterator.obj_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TOrdetDataConstIterator& _iterator) const noexcept {
        return obj_ptr_ != _iterator.obj_ptr_;
    }
    FORCEINLINE TOrdetDataConstIterator& operator=(const TOrdetDataConstIterator& _iterator) noexcept {
        obj_ptr_ = _iterator.obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataConstIterator& operator=(TOrdetDataConstIterator&& _iterator) noexcept {
        obj_ptr_ = _iterator.obj_ptr_;
        _iterator.obj_ptr_ = nullptr;
        return *this;
    }
    FORCEINLINE TOrdetDataConstIterator& operator=(const TOrdetDataIterator<_ObjectType>& _iterator) noexcept {
        obj_ptr_ = _iterator.obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataConstIterator& operator=(TOrdetDataIterator<_ObjectType>&& _iterator) noexcept {
        obj_ptr_ = _iterator.obj_ptr_;
        _iterator.obj_ptr_ = nullptr;
        return *this;
    }
    FORCEINLINE TOrdetDataConstIterator& operator++() noexcept {
        ++obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataConstIterator& operator--() noexcept {
        --obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataConstIterator operator+=(SizeType _num) noexcept {
        obj_ptr_ += _num;
        return *this;
    }
    FORCEINLINE TOrdetDataConstIterator& operator-=(SizeType _num) noexcept {
        obj_ptr_ -= _num;
        return *this;
    }
    FORCEINLINE TOrdetDataConstIterator operator+(SizeType _num) noexcept {
        return TOrdetDataConstIterator(obj_ptr_ + _num);
    }
    FORCEINLINE TOrdetDataConstIterator operator-(SizeType _num) noexcept {
        return TOrdetDataConstIterator(obj_ptr_ - _num);
    }

    NODISCARD FORCEINLINE const _ObjectType& operator*() const noexcept { return *obj_ptr_; }
    NODISCARD FORCEINLINE const _ObjectType* operator->() const noexcept { return obj_ptr_; }
    NODISCARD FORCEINLINE const _ObjectType* Ptr() const noexcept { return obj_ptr_; }

protected:
    using SuperType_ = ZObject;

private:
    _ObjectType* obj_ptr_;
};

template<typename _ObjectType>
class TOrdetDataReverseIterator : public ZObject {
public:
    FORCEINLINE TOrdetDataReverseIterator(_ObjectType* _obj_ptr) noexcept 
        : SuperType_()
        , obj_ptr_(_obj_ptr) {}
    FORCEINLINE TOrdetDataReverseIterator(const TOrdetDataReverseIterator& _iterator) noexcept
        : SuperType_(_iterator)
        , obj_ptr_(_iterator.obj_ptr_) {
    }
    FORCEINLINE TOrdetDataReverseIterator(TOrdetDataReverseIterator&& _iterator) noexcept
        : SuperType_(std::forward<TOrdetDataReverseIterator>(_iterator))
        , obj_ptr_(_iterator.obj_ptr_)
    {
        _iterator.obj_ptr_ = nullptr;
    }

    NODISCARD FORCEINLINE Bool operator==(const _ObjectType* _obj_ptr) const noexcept {
        return obj_ptr_ == _obj_ptr;
    }
    NODISCARD FORCEINLINE Bool operator!=(const _ObjectType* _obj_ptr) const noexcept {
        return obj_ptr_ != _obj_ptr;
    }
    NODISCARD FORCEINLINE Bool operator==(const TOrdetDataReverseIterator& _iterator) const noexcept {
        return obj_ptr_ == _iterator.obj_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TOrdetDataReverseIterator& _iterator) const noexcept {
        return obj_ptr_ != _iterator.obj_ptr_;
    }
    FORCEINLINE TOrdetDataReverseIterator& operator=(const TOrdetDataReverseIterator& _iterator) noexcept {
        obj_ptr_ = _iterator.obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataReverseIterator& operator=(TOrdetDataReverseIterator&& _iterator) noexcept {
        obj_ptr_ = _iterator.obj_ptr_;
        _iterator.obj_ptr_ = nullptr;
        return *this;
    }
    FORCEINLINE TOrdetDataReverseIterator& operator++() noexcept {
        --obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataReverseIterator& operator--() noexcept {
        ++obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataReverseIterator operator+=(SizeType _num) noexcept {
        obj_ptr_ -= _num;
        return *this;
    }
    FORCEINLINE TOrdetDataReverseIterator& operator-=(SizeType _num) noexcept {
        obj_ptr_ += _num;
        return *this;
    }
    FORCEINLINE TOrdetDataReverseIterator operator+(SizeType _num) noexcept {
        return TOrdetDataReverseIterator(obj_ptr_ - _num);
    }
    FORCEINLINE TOrdetDataReverseIterator operator-(SizeType _num) noexcept {
        return TOrdetDataReverseIterator(obj_ptr_ + _num);
    }

    NODISCARD FORCEINLINE _ObjectType& operator*() noexcept { return *obj_ptr_; }
    NODISCARD FORCEINLINE const _ObjectType& operator*() const noexcept { return *obj_ptr_; }
    NODISCARD FORCEINLINE _ObjectType* operator->() noexcept { return obj_ptr_; }
    NODISCARD FORCEINLINE const _ObjectType* operator->() const noexcept { return obj_ptr_; }
    NODISCARD FORCEINLINE _ObjectType* Ptr() noexcept { return obj_ptr_; }
    NODISCARD FORCEINLINE const _ObjectType* Ptr() const noexcept { return obj_ptr_; }

protected:
    using SuperType_ = ZObject;
    friend class TOrdetDataConstReverseIterator<_ObjectType>;

private:
    _ObjectType* obj_ptr_;
};

template<typename _ObjectType>
class TOrdetDataConstReverseIterator : public ZObject {
public:
    FORCEINLINE TOrdetDataConstReverseIterator(_ObjectType* _obj_ptr) noexcept 
        : SuperType_()
        , obj_ptr_(_obj_ptr) {}
    FORCEINLINE TOrdetDataConstReverseIterator(const TOrdetDataConstReverseIterator& _iterator) noexcept
        : SuperType_(_iterator)
        , obj_ptr_(_iterator.obj_ptr_) {
    }
    FORCEINLINE TOrdetDataConstReverseIterator(TOrdetDataConstReverseIterator&& _iterator) noexcept
        : SuperType_(std::forward<TOrdetDataConstReverseIterator>(_iterator))
        , obj_ptr_(_iterator.obj_ptr_)
    {
        _iterator.obj_ptr_ = nullptr;
    }
    FORCEINLINE TOrdetDataConstReverseIterator(const TOrdetDataReverseIterator<_ObjectType>& _iterator) noexcept
        : SuperType_()
        , obj_ptr_(_iterator.obj_ptr_) {
    }
    FORCEINLINE TOrdetDataConstReverseIterator(TOrdetDataReverseIterator<_ObjectType>&& _iterator) noexcept
        : SuperType_()
        , obj_ptr_(_iterator.obj_ptr_)
    {
        _iterator.obj_ptr_ = nullptr;
    }

    NODISCARD FORCEINLINE Bool operator==(const _ObjectType* _obj_ptr) const noexcept {
        return obj_ptr_ == _obj_ptr;
    }
    NODISCARD FORCEINLINE Bool operator!=(const _ObjectType* _obj_ptr) const noexcept {
        return obj_ptr_ != _obj_ptr;
    }
    NODISCARD FORCEINLINE Bool operator==(const TOrdetDataConstReverseIterator& _iterator) const noexcept {
        return obj_ptr_ == _iterator.obj_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TOrdetDataConstReverseIterator& _iterator) const noexcept {
        return obj_ptr_ != _iterator.obj_ptr_;
    }
    FORCEINLINE TOrdetDataConstReverseIterator& operator=(const TOrdetDataConstReverseIterator& _iterator) noexcept {
        obj_ptr_ = _iterator.obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataConstReverseIterator& operator=(TOrdetDataConstReverseIterator&& _iterator) noexcept {
        obj_ptr_ = _iterator.obj_ptr_;
        _iterator.obj_ptr_ = nullptr;
        return *this;
    }
    FORCEINLINE TOrdetDataConstReverseIterator& operator=(
        const TOrdetDataReverseIterator<_ObjectType>& _iterator
    ) noexcept {
        obj_ptr_ = _iterator.obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataConstReverseIterator& operator=(
        TOrdetDataReverseIterator<_ObjectType>&& _iterator
    ) noexcept {
        obj_ptr_ = _iterator.obj_ptr_;
        _iterator.obj_ptr_ = nullptr;
        return *this;
    }
    FORCEINLINE TOrdetDataConstReverseIterator& operator++() noexcept {
        --obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataConstReverseIterator& operator--() noexcept {
        ++obj_ptr_;
        return *this;
    }
    FORCEINLINE TOrdetDataConstReverseIterator operator+=(SizeType _num) noexcept {
        obj_ptr_ -= _num;
        return *this;
    }
    FORCEINLINE TOrdetDataConstReverseIterator& operator-=(SizeType _num) noexcept {
        obj_ptr_ += _num;
        return *this;
    }
    FORCEINLINE TOrdetDataConstReverseIterator operator+(SizeType _num) noexcept {
        return TOrdetDataConstReverseIterator(obj_ptr_ - _num);
    }
    FORCEINLINE TOrdetDataConstReverseIterator operator-(SizeType _num) noexcept {
        return TOrdetDataConstReverseIterator(obj_ptr_ + _num);
    }

    NODISCARD FORCEINLINE const _ObjectType& operator*() const noexcept { return *obj_ptr_; }
    NODISCARD FORCEINLINE const _ObjectType* operator->() const noexcept { return obj_ptr_; }
    NODISCARD FORCEINLINE const _ObjectType* Ptr() const noexcept { return obj_ptr_; }

protected:
    using SuperType_ = ZObject;

private:
    _ObjectType* obj_ptr_;
};

}//internal
}//zengine