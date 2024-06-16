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
#ifndef Z_CORE_T_DEQUE_H_
#define Z_CORE_T_DEQUE_H_

#include "internal/z_drive.h"

#include <deque>

#include "t_allocator.h"
#include "z_object.h"

namespace zengine {

/*
    Double end queue caintainer.
*/
template<typename ObjectType>
class TDeque : public ZObject {
public:
    using STDDeque = std::deque<ObjectType, TAllocator<ObjectType>>;
    using Iterator = STDDeque::iterator;
    using ConstIterator = STDDeque::const_iterator;
    using ReverseIterator = STDDeque::reverse_iterator;
    using ConstReverseIterator = STDDeque::const_reverse_iterator;
    using InitializerList = std::initializer_list<ObjectType>;

    FORCEINLINE TDeque() noexcept : SuperType(), deque_() {}
    FORCEINLINE TDeque(const TDeque& deque) noexcept : SuperType(), deque_(deque.deque_) {}
    FORCEINLINE TDeque(TDeque&& deque) noexcept : SuperType(), deque_(std::move(deque.deque_)) {}

    FORCEINLINE TDeque(SizeType size) noexcept : SuperType(), deque_(size) {}
    FORCEINLINE TDeque(SizeType size, const ZObject& value) noexcept : SuperType(), deque_(size, value) {}
    template <typename InputIterator>
    FORCEINLINE TDeque(InputIterator first, InputIterator last) noexcept : SuperType(), deque_(first, last) {}
    FORCEINLINE TDeque(InitializerList init_list) noexcept : SuperType(), deque_(init_list) {}
 
    FORCEINLINE ~TDeque() noexcept {}

    FORCEINLINE TDeque& operator=(const TDeque& deque) noexcept { 
        deque_.operator=(deque.deque_);
        return *this;
    }
    FORCEINLINE TDeque& operator=(TDeque&& deque) noexcept { 
        deque_.operator=(std::move(deque.deque_));
        return *this;
    }
    FORCEINLINE TDeque& operator=(InitializerList init_list) noexcept {
        deque_.operator=(init_list);
        return *this;
    }

    FORCEINLINE Void Assign(SizeType size, const ObjectType& value) noexcept {
        return deque_.assign(size, value);
    }
    template <class InputIterator>
    FORCEINLINE Void Assign(InputIterator first, InputIterator last) noexcept {
        return deque_.assign(first, last);
    }
    FORCEINLINE Void Assign(InitializerList init_list) noexcept {
        return deque_.assign(init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TDeque& deque) noexcept { return deque_ == deque; }
    NODISCARD FORCEINLINE Bool operator!=(const TDeque& deque) noexcept { return deque_ != deque; }

    NODISCARD FORCEINLINE ObjectType& operator[](const SizeType index) noexcept { return deque_[index]; }
    NODISCARD FORCEINLINE const ObjectType& operator[](const SizeType index) const noexcept { return deque_[index]; }

    NODISCARD FORCEINLINE ObjectType& At(IndexType index) noexcept { return deque_.at(index); }
    NODISCARD FORCEINLINE const ObjectType& At(IndexType index) const noexcept { return deque_.at(index); }

    NODISCARD FORCEINLINE ObjectType& Front() noexcept { return deque_.front(); }
    NODISCARD FORCEINLINE const ObjectType& Front() const noexcept { return deque_.front(); }
    NODISCARD FORCEINLINE ObjectType& Back() noexcept { return deque_.back(); }
    NODISCARD FORCEINLINE const ObjectType& Back() const noexcept { return deque_.back(); }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { return static_cast<IndexType>(deque_.size()); }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return kIndexTypeMax; }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return deque_.empty(); }

    NODISCARD FORCEINLINE Iterator Begin() noexcept { return deque_.begin(); }
    NODISCARD FORCEINLINE ConstIterator Begin() const noexcept { return deque_.begin(); }
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const noexcept { return deque_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseBegin() noexcept { return deque_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseBegin() const noexcept { return deque_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseBegin() const noexcept { return deque_.crbegin(); }
    NODISCARD FORCEINLINE Iterator End() noexcept { return deque_.end(); }
    NODISCARD FORCEINLINE ConstIterator End() const noexcept { return deque_.end(); }
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const noexcept { return deque_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseEnd() noexcept { return deque_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseEnd() const noexcept { return deque_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseEnd() const noexcept { return deque_.crend(); }

    template <typename... ArgsType>
    FORCEINLINE Iterator Emplace(ConstIterator pos, ArgsType&&... args) noexcept {
        return deque_.emplace(pos, std::forward<ArgsType>(args)...);
    }

    template <typename... ArgsType>
    FORCEINLINE ObjectType& EmplaceFront(ArgsType&&... args) noexcept {
        return deque_.emplace_front(std::forward<ArgsType>(args)...);
    }
    FORCEINLINE Void PushFront(const ObjectType& value) noexcept { deque_.push_front(value); }
    FORCEINLINE Void PushFront(ObjectType&& value) noexcept { deque_.push_front(std::forward<ObjectType>(value)); }
    FORCEINLINE Void PopFront() noexcept { deque_.pop_front(); }

    template <typename... ArgsType>
    FORCEINLINE ObjectType& EmplaceBack(ArgsType&&... args) noexcept {
        return deque_.emplace_back(std::forward<ArgsType>(args)...);
    }
    FORCEINLINE Void PushBack(const ObjectType& value) noexcept { deque_.push_back(value); }
    FORCEINLINE Void PushBack(ObjectType&& value) noexcept { deque_.push_back(std::forward<ObjectType>(value)); }
    FORCEINLINE Void PopBack() noexcept { deque_.pop_back(); }

    FORCEINLINE Iterator Insert(ConstIterator pos, const ObjectType& value) noexcept { 
        return deque_.insert(pos, value);
    }
    FORCEINLINE Iterator Insert(ConstIterator pos, ObjectType&& value) noexcept {
        return deque_.insert(pos, std::forward<ObjectType>(value));
    }
    FORCEINLINE Iterator Insert(ConstIterator pos, SizeType num, const ObjectType& value) noexcept {
        return deque_.insert(pos, num, value);
    }
    template <typename InputIterator>
    FORCEINLINE Iterator Insert(ConstIterator pos, InputIterator first, InputIterator last) noexcept {
        return deque_.insert(pos, first, last);
    }
    FORCEINLINE Iterator Insert(ConstIterator pos, InitializerList init_list) noexcept {
        return deque_.insert(pos, init_list);
    }

    FORCEINLINE Iterator Erase(ConstIterator pos) noexcept { return deque_.erase(pos); }
    FORCEINLINE Iterator Erase(ConstIterator first, ConstIterator last) noexcept { return deque_.erase(first, last); }
    FORCEINLINE Void Clear() noexcept { deque_.clear(); }

    FORCEINLINE Void Resize(SizeType size) noexcept { deque_.resize(size); }
    FORCEINLINE Void Resize(SizeType size, const ObjectType& value) noexcept { deque_.resize(size, value); }

    FORCEINLINE Void Reverse(SizeType capacity) noexcept { deque_.reverse(capacity); }

    FORCEINLINE Void Swap(TDeque& deque) noexcept { deque_.swap(deque); }

protected:
    using SuperType = ZObject;

private:
    STDDeque deque_;
};

}//zengine

#endif // !Z_CORE_T_DEQUE_H_