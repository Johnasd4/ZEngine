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
template<typename ObjectType>
class TVector : public ZObject {
public:
    using STDVector = std::vector<ObjectType, TAllocator<ObjectType>>;
    using Iterator = STDVector::iterator;
    using ConstIterator = STDVector::const_iterator;
    using ReverseIterator = STDVector::reverse_iterator;
    using ConstReverseIterator = STDVector::const_reverse_iterator;
    using InitializerList = std::initializer_list<ObjectType>;

    FORCEINLINE TVector() noexcept : SuperType(), vector_() {}
    FORCEINLINE TVector(const TVector& vector) noexcept : SuperType(), vector_(vector.vector_) {}
    FORCEINLINE TVector(TVector&& vector) noexcept : SuperType(), vector_(std::move(vector.vector_)) {}

    FORCEINLINE TVector(SizeType size) noexcept : SuperType(), vector_(size) {}
    FORCEINLINE TVector(SizeType size, const ObjectType& value) noexcept : SuperType(), vector_(size, value) {}
    template <typename InputIterator>
    FORCEINLINE TVector(InputIterator first, InputIterator last) noexcept : SuperType(), vector_(first, last) {}
    FORCEINLINE TVector(InitializerList init_list) noexcept : SuperType(), vector_(init_list) {}
 
    FORCEINLINE ~TVector() noexcept {}

    FORCEINLINE TVector& operator=(const TVector& vector) noexcept { 
        vector_.operator=(vector.vector_);
        return *this;
    }
    FORCEINLINE TVector& operator=(TVector&& vector) noexcept { 
        vector_.operator=(std::move(vector.vector_));
        return *this;
    }
    FORCEINLINE TVector& operator=(InitializerList init_list) noexcept {
        vector_.operator=(init_list);
        return *this;
    }

    FORCEINLINE Void Assign(SizeType size, const ObjectType& value) noexcept {
        return vector_.assign(size, value);
    }
    template <class InputIterator>
    FORCEINLINE Void Assign(InputIterator first, InputIterator last) noexcept {
        return vector_.assign(first, last);
    }
    FORCEINLINE Void Assign(InitializerList init_list) noexcept {
        return vector_.assign(init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TVector& vector) noexcept { return vector_ == vector; }
    NODISCARD FORCEINLINE Bool operator!=(const TVector& vector) noexcept { return vector_ != vector; }

    NODISCARD FORCEINLINE ObjectType& operator[](const SizeType index) noexcept { return vector_[index]; }
    NODISCARD FORCEINLINE const ObjectType& operator[](const SizeType index) const noexcept { return vector_[index]; }

    NODISCARD FORCEINLINE ObjectType& At(IndexType index) noexcept { return vector_.at(index); }
    NODISCARD FORCEINLINE const ObjectType& At(IndexType index) const noexcept { return vector_.at(index); }

    NODISCARD FORCEINLINE ObjectType& Front() noexcept { return vector_.front(); }
    NODISCARD FORCEINLINE const ObjectType& Front() const noexcept { return vector_.front(); }
    NODISCARD FORCEINLINE ObjectType& Back() noexcept { return vector_.back(); }
    NODISCARD FORCEINLINE const ObjectType& Back() const noexcept { return vector_.back(); }
    NODISCARD FORCEINLINE ObjectType* DataPtr() noexcept { return vector_.data(); }
    NODISCARD FORCEINLINE const ObjectType* DataPtr() const noexcept { return vector_.data(); }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { return static_cast<IndexType>(vector_.size()); }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return vector_.capacity(); }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return vector_.empty(); }

    NODISCARD FORCEINLINE Iterator Begin() noexcept { return vector_.begin(); }
    NODISCARD FORCEINLINE ConstIterator Begin() const noexcept { return vector_.begin(); }
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const noexcept { return vector_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseBegin() noexcept { return vector_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseBegin() const noexcept { return vector_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseBegin() const noexcept { return vector_.crbegin(); }
    NODISCARD FORCEINLINE Iterator End() noexcept { return vector_.end(); }
    NODISCARD FORCEINLINE ConstIterator End() const noexcept { return vector_.end(); }
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const noexcept { return vector_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseEnd() noexcept { return vector_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseEnd() const noexcept { return vector_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseEnd() const noexcept { return vector_.crend(); }

    FORCEINLINE Void PushBack(const ObjectType& value) noexcept { vector_.push_back(value); }
    FORCEINLINE Void PushBack(ObjectType&& value) noexcept { vector_.push_back(std::forward<ObjectType>(value)); }
    FORCEINLINE Void PopBack() noexcept { vector_.pop_back(); }

    FORCEINLINE Iterator Insert(ConstIterator pos, const ObjectType& value) noexcept { 
        return vector_.insert(pos, value);
    }
    FORCEINLINE Iterator Insert(ConstIterator pos, ObjectType&& value) noexcept {
        return vector_.insert(pos, std::forward<ObjectType>(value));
    }
    FORCEINLINE Iterator Insert(ConstIterator pos, SizeType num, const ObjectType& value) noexcept {
        return vector_.insert(pos, num, value);
    }
    template <typename InputIterator>
    FORCEINLINE Iterator Insert(ConstIterator pos, InputIterator first, InputIterator last) noexcept {
        return vector_.insert(pos, first, last);
    }
    FORCEINLINE Iterator Insert(ConstIterator pos, InitializerList init_list) noexcept {
        return vector_.insert(pos, init_list);
    }

    FORCEINLINE Iterator Erase(ConstIterator pos) noexcept { return vector_.erase(pos); }
    FORCEINLINE Iterator Erase(ConstIterator first, ConstIterator last) noexcept { return vector_.erase(first, last); }
    FORCEINLINE Void Clear() noexcept { vector_.clear(); }

    template <typename... ArgsType>
    FORCEINLINE Iterator Emplace(ConstIterator pos, ArgsType&&... args) noexcept {
        return vector_.emplace(pos, std::forward<ArgsType>(args)...);
    }
    template <typename... ArgsType>
    FORCEINLINE ObjectType& EmplaceBack(ArgsType&&... args) noexcept {
        return vector_.emplace_back(std::forward<ArgsType>(args)...); 
    }

    FORCEINLINE Void Resize(SizeType size) noexcept { vector_.resize(size); }
    FORCEINLINE Void Resize(SizeType size, const ObjectType& value) noexcept { vector_.resize(size, value); }

    FORCEINLINE Void Reserve(SizeType capacity) noexcept { vector_.reserve(capacity); }

    FORCEINLINE Void Swap(TVector& vector) noexcept { vector_.swap(vector); }

protected:
    using SuperType = ZObject;

private:
    STDVector vector_;
};

}//zengine

#endif // !Z_CORE_T_VECTOR_H_