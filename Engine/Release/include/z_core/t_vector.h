/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#ifndef Z_CORE_T_VECTOR_H_
#define Z_CORE_T_VECTOR_H_

#include "internal/z_drive.h"

#include <vector>

#include "t_allocator.h"
#include "z_object.h"

namespace zengine {

/*
    Vector caintainer.
*/
template<typename _ObjectType>
class TVector : public ZObject {
public:
    using STDVector_ = std::vector<_ObjectType, TContainerAllocator<_ObjectType>>;
    using Iterator_ = STDVector_::iterator;
    using ConstIterator_ = STDVector_::const_iterator;
    using ReverseIterator_ = STDVector_::reverse_iterator;
    using ConstReverseIterator_ = STDVector_::const_reverse_iterator;
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TVector() noexcept : SuperType_(), vector_() {}
    FORCEINLINE TVector(const TVector& _vector) noexcept : SuperType_(), vector_(_vector.vector_) {}
    FORCEINLINE TVector(TVector&& _vector) noexcept : SuperType_(), vector_(std::move(_vector.vector_)) {}

    FORCEINLINE TVector(SizeType _size) noexcept : SuperType_(), vector_(_size) {}
    FORCEINLINE TVector(SizeType _size, const _ObjectType& _val) noexcept : SuperType_(), vector_(_size, _val) {}
    template <typename _InputIterator>
    FORCEINLINE TVector(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), vector_(_first, _last) {}
    FORCEINLINE TVector(InitializerList_ _init_list) noexcept : SuperType_(), vector_(_init_list) {}
 
    FORCEINLINE ~TVector() noexcept {}

    FORCEINLINE TVector& operator=(const TVector& _vector) noexcept { 
        vector_ = _vector.vector_;
        return *this;
    }
    FORCEINLINE TVector& operator=(TVector&& _vector) noexcept { 
        vector_ = std::move(_vector.vector_);
        return *this;
    }
    FORCEINLINE TVector& operator=(InitializerList_ _init_list) noexcept {
        vector_ = _init_list;
        return *this;
    }

    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        return vector_.assign(_size, _val);
    }
    template <class _InputIterator>
    FORCEINLINE Void Assign(_InputIterator _first, _InputIterator _last) noexcept {
        return vector_.assign(_first, _last);
    }
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        return vector_.assign(_init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TVector& _vector) noexcept { return vector_ == _vector; }
    NODISCARD FORCEINLINE Bool operator!=(const TVector& _vector) noexcept { return vector_ != _vector; }

    NODISCARD FORCEINLINE _ObjectType& operator[](const SizeType _index) noexcept { return vector_[_index]; }
    NODISCARD FORCEINLINE const _ObjectType& operator[](const SizeType _index) const noexcept { return vector_[_index]; }

    NODISCARD FORCEINLINE _ObjectType& At(IndexType _index) noexcept { return vector_.at(_index); }
    NODISCARD FORCEINLINE const _ObjectType& At(IndexType _index) const noexcept { return vector_.at(_index); }

    NODISCARD FORCEINLINE _ObjectType& Front() noexcept { return vector_.front(); }
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return vector_.front(); }
    NODISCARD FORCEINLINE _ObjectType& Back() noexcept { return vector_.back(); }
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return vector_.back(); }
    NODISCARD FORCEINLINE _ObjectType* DataPtr() noexcept { return vector_.data(); }
    NODISCARD FORCEINLINE const _ObjectType* DataPtr() const noexcept { return vector_.data(); }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { return static_cast<IndexType>(vector_.size()); }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return vector_.capacity(); }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return vector_.empty(); }

    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return vector_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return vector_.begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return vector_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return vector_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return vector_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return vector_.crbegin(); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return vector_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return vector_.end(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return vector_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return vector_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return vector_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return vector_.crend(); }

    FORCEINLINE Void PushBack(const _ObjectType& _val) noexcept { vector_.push_back(_val); }
    FORCEINLINE Void PushBack(_ObjectType&& _val) noexcept { vector_.push_back(std::forward<_ObjectType>(_val)); }
    FORCEINLINE Void PopBack() noexcept { vector_.pop_back(); }

    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, const _ObjectType& _val) noexcept { 
        return vector_.insert(_pos, _val);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _ObjectType&& _val) noexcept {
        return vector_.insert(_pos, std::forward<_ObjectType>(_val));
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, SizeType _num, const _ObjectType& _val) noexcept {
        return vector_.insert(_pos, _num, _val);
    }
    template <typename _InputIterator>
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, _InputIterator _first, _InputIterator _last) noexcept {
        return vector_.insert(_pos, _first, _last);
    }
    FORCEINLINE Iterator_ Insert(ConstIterator_ _pos, InitializerList_ _init_list) noexcept {
        return vector_.insert(_pos, _init_list);
    }

    FORCEINLINE Iterator_ Erase(ConstIterator_ _pos) noexcept { return vector_.erase(_pos); }
    FORCEINLINE Iterator_ Erase(ConstIterator_ _first, ConstIterator_ _last) noexcept 
    { return vector_.erase(_first, _last); 
    }
    FORCEINLINE Void Clear() noexcept { vector_.clear(); }

    template <typename... _ArgsType>
    FORCEINLINE Iterator_ Emplace(ConstIterator_ _pos, _ArgsType&&... _args) noexcept {
        return vector_.emplace(_pos, std::forward<_ArgsType>(_args)...);
    }
    template <typename... _ArgsType>
    FORCEINLINE _ObjectType& EmplaceBack(_ArgsType&&... _args) noexcept {
        return vector_.emplace_back(std::forward<_ArgsType>(_args)...); 
    }

    FORCEINLINE Void Resize(SizeType _size) noexcept { vector_.resize(_size); }
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { vector_.resize(_size, _val); }

    FORCEINLINE Void Reserve(SizeType _capacity) noexcept { vector_.reserve(_capacity); }

    FORCEINLINE Void Swap(TVector& _vector) noexcept { vector_.swap(_vector); }

protected:
    using SuperType_ = ZObject;

private:
    STDVector_ vector_;
};

}//zengine

#endif // !Z_CORE_T_VECTOR_H_