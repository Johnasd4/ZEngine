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

#include "internal/z_drive.h"

#include "t_lock_guard.h"
#include "z_mutex.h"
#include "z_object.h"

namespace zengine {

/*
    Atom template class, the variable will be thread safe.
*/
template<typename _ObjectType>
class TAtom : public ZObject {
public:
    FORCEINLINE TAtom() noexcept : SuperType_(), mutex_(), obj_() {}
    FORCEINLINE TAtom(const TAtom& _atom) noexcept : SuperType_(_atom), mutex_() {
        _atom.mutex_.Lock();
        obj_ = _atom.obj_;
    }
    FORCEINLINE TAtom(TAtom&& _atom) noexcept : SuperType_(std::forward<TAtom>(_atom)), mutex_() {
        TLockGuard<ZMutex> lock_guard_right(_atom.mutex_);
        obj_ = std::move(_atom.obj_);
    }
    FORCEINLINE TAtom(const _ObjectType& _obj) noexcept : SuperType_(), mutex_(), obj_(_obj) {}
    FORCEINLINE TAtom(_ObjectType&& _obj) noexcept : SuperType_(), mutex_(), obj_(std::move(_obj)) {}

    FORCEINLINE ~TAtom() noexcept {}

    FORCEINLINE TAtom& operator=(const TAtom& _atom) noexcept {
        TLockGuard<ZMutex> lock_guard_left(mutex_);
        TLockGuard<ZMutex> lock_guard_right(_atom.mutex_);
        SuperType_::operator=(_atom);
        obj_ = _atom.obj_;
        return *this;
    }
    FORCEINLINE TAtom& operator=(TAtom&& _atom) noexcept {
        TLockGuard<ZMutex> lock_guard_left(mutex_);
        TLockGuard<ZMutex> lock_guard_right(_atom.mutex_);
        SuperType_::operator=(std::forward<TAtom>(_atom));
        obj_ = std::move(_atom.obj_);
        return *this;
    }
    FORCEINLINE TAtom& operator=(const _ObjectType& _obj) noexcept {
        TLockGuard<ZMutex> lock_guard_left(mutex_);
        obj_ = _obj;
        return *this;
    }
    FORCEINLINE TAtom& operator=(_ObjectType&& _obj) noexcept {
        TLockGuard<ZMutex> lock_guard_left(mutex_);
        obj_ = std::move(_obj);
        return *this;
    }

    FORCEINLINE TAtom& operator++() noexcept {
        TLockGuard<ZMutex> lock_guard(mutex_);
        ++obj_;
        return *this;
    }
    FORCEINLINE TAtom& operator--() noexcept {
        TLockGuard<ZMutex> lock_guard(mutex_);
        --obj_;
        return *this;
    }
    template<typename _OtherObjectType>
    FORCEINLINE TAtom& operator+=(const _OtherObjectType& _obj) noexcept {
        TLockGuard<ZMutex> lock_guard(mutex_);
        obj_+= _obj;
        return *this;
    }
    template<typename _OtherObjectType>
    FORCEINLINE TAtom& operator-=(const _OtherObjectType& _obj) noexcept {
        TLockGuard<ZMutex> lock_guard(mutex_);
        obj_ -= _obj;
        return *this;
    }
    template<typename _OtherObjectType>
    FORCEINLINE TAtom& operator*=(const _OtherObjectType& _obj) noexcept {
        TLockGuard<ZMutex> lock_guard(mutex_);
        obj_ *= _obj;
        return *this;
    }
    template<typename _OtherObjectType>
    FORCEINLINE TAtom& operator/=(const _OtherObjectType& _obj) noexcept {
        TLockGuard<ZMutex> lock_guard(mutex_);
        obj_ /= _obj;
        return *this;
    }
    template<typename _OtherObjectType>
    FORCEINLINE TAtom& operator|=(const _OtherObjectType& _obj) noexcept {
        TLockGuard<ZMutex> lock_guard(mutex_);
        obj_ /= _obj;
        return *this;
    }
    template<typename _OtherObjectType>
    FORCEINLINE TAtom& operator&=(const _OtherObjectType& _obj) noexcept {
        TLockGuard<ZMutex> lock_guard(mutex_);
        obj_ /= _obj;
        return *this;
    }

    template<typename _OtherObjectType>
    FORCEINLINE auto operator>>(const _OtherObjectType& _obj) noexcept {
        TLockGuard<ZMutex> lock_guard(mutex_);
        return obj_ >> _obj;
    }
    template<typename _OtherObjectType>
    FORCEINLINE auto operator<<(const _OtherObjectType& _obj) noexcept {
        TLockGuard<ZMutex> lock_guard(mutex_);
        return obj_ << _obj;
    }

    NODISCARD FORCEINLINE auto operator~() noexcept {
        TLockGuard<ZMutex> lock_guard(mutex_);
        return ~obj_;
    }
    NODISCARD FORCEINLINE auto operator!() noexcept {
        TLockGuard<ZMutex> lock_guard(mutex_);
        return !obj_;
    }

    friend NODISCARD FORCEINLINE auto operator+(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ + _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator+(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left + _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator+(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ + _right;
    }
    friend NODISCARD FORCEINLINE auto operator-(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ - _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator-(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left - _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator-(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ - _right;
    }
    friend NODISCARD FORCEINLINE auto operator*(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ * _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator*(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left * _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator*(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ * _right;
    }
    friend NODISCARD FORCEINLINE auto operator/(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ / _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator/(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left / _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator/(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ / _right;
    }
    friend NODISCARD FORCEINLINE auto operator%(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ % _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator%(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left % _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator%(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ % _right;
    }
    friend NODISCARD FORCEINLINE auto operator^(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ ^ _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator^(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left ^ _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator^(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ ^ _right;
    }
    friend NODISCARD FORCEINLINE auto operator|(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ | _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator|(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left | _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator|(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ | _right;
    }
    friend NODISCARD FORCEINLINE auto operator&(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ & _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator&(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left & _right.obj_;
    }
    friend NODISCARD FORCEINLINE auto operator&(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ & _right;
    }

    friend NODISCARD FORCEINLINE Bool operator==(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ == _right.obj_;
    }
    friend NODISCARD FORCEINLINE Bool operator==(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left == _right.obj_;
    }
    friend NODISCARD FORCEINLINE Bool operator==(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ == _right;
    }
    friend NODISCARD FORCEINLINE Bool operator!=(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ != _right.obj_;
    }
    friend NODISCARD FORCEINLINE Bool operator!=(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left != _right.obj_;
    }
    friend NODISCARD FORCEINLINE Bool operator!=(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ != _right;
    }
    friend NODISCARD FORCEINLINE Bool operator>(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ > _right.obj_;
    }
    friend NODISCARD FORCEINLINE Bool operator>(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left > _right.obj_;
    }
    friend NODISCARD FORCEINLINE Bool operator>(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ > _right;
    }
    friend NODISCARD FORCEINLINE Bool operator>=(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ >= _right.obj_;
    }
    friend NODISCARD FORCEINLINE Bool operator>=(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left >= _right.obj_;
    }
    friend NODISCARD FORCEINLINE Bool operator>=(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ >= _right;
    }
    friend NODISCARD FORCEINLINE Bool operator<(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ < _right.obj_;
    }
    friend NODISCARD FORCEINLINE Bool operator<(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left < _right.obj_;
    }
    friend NODISCARD FORCEINLINE Bool operator<(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ < _right;
    }
    friend NODISCARD FORCEINLINE Bool operator<=(const TAtom& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left.obj_ <= _right.obj_;
    }
    friend NODISCARD FORCEINLINE Bool operator<=(const _ObjectType& _left, const TAtom& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_right(_right.mutex_);
        return _left <= _right.obj_;
    }
    friend NODISCARD FORCEINLINE Bool operator<=(const TAtom& _left, const _ObjectType& _right) noexcept {
        TLockGuard<ZMutex> lock_guard_left(_left.mutex_);
        return _left.obj_ <= _right;
    }

    NODISCARD FORCEINLINE _ObjectType Value() noexcept { 
        TLockGuard<ZMutex> lock_guard(mutex_);
        return obj_;
    }
    NODISCARD FORCEINLINE const _ObjectType Value() const noexcept {
        TLockGuard<ZMutex> lock_guard(mutex_);
        return obj_; 
    }

protected:
    using SuperType_ = ZObject;

private:
    mutable ZMutex mutex_;
    _ObjectType obj_;
};

}//zengine