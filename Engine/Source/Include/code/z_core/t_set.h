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
#ifndef Z_CORE_T_SET_H_
#define Z_CORE_T_SET_H_

#include "internal/z_drive.h"

#include <set>

#include "t_allocator.h"
#include "t_pair.h"
#include "z_object.h"

namespace zengine {

/*
    Set caintainer.
*/
template<typename ObjectType>
class TSet : public ZObject {
public:
    using STDSet = std::set<ObjectType, std::less<ObjectType>, TAllocator<ObjectType>>;
    using Iterator = STDSet::iterator;
    using ConstIterator = STDSet::const_iterator;
    using ReverseIterator = STDSet::reverse_iterator;
    using ConstReverseIterator = STDSet::const_reverse_iterator;
    using InitializerList = std::initializer_list<ObjectType>;

    FORCEINLINE TSet() noexcept : SuperType(), set_() {}
    FORCEINLINE TSet(const TSet& set) noexcept : SuperType(), set_(set.set_) {}
    FORCEINLINE TSet(TSet&& set) noexcept : SuperType(), set_(std::move(set.set_)) {}

    template <typename InputIterator>
    FORCEINLINE TSet(InputIterator first, InputIterator last) noexcept : SuperType(), set_(first, last) {}
    FORCEINLINE TSet(InitializerList init_list) noexcept : SuperType(), set_(init_list) {}
 
    FORCEINLINE ~TSet() noexcept {}

    FORCEINLINE TSet& operator=(const TSet& set) noexcept { 
        set_.operator=(set.set_);
        return *this;
    }
    FORCEINLINE TSet& operator=(TSet&& set) noexcept { 
        set_.operator=(std::move(set.set_));
        return *this;
    }
    FORCEINLINE TSet& operator=(InitializerList init_list) noexcept {
        set_.operator=(init_list);
        return *this;
    }

    NODISCARD FORCEINLINE Bool operator==(const TSet& set) noexcept { return set_ == set; }
    NODISCARD FORCEINLINE Bool operator!=(const TSet& set) noexcept { return set_ != set; }

    NODISCARD FORCEINLINE ObjectType& Front() noexcept { return set_.front(); }
    NODISCARD FORCEINLINE const ObjectType& Front() const noexcept { return set_.front(); }
    NODISCARD FORCEINLINE ObjectType& Back() noexcept { return set_.back(); }
    NODISCARD FORCEINLINE const ObjectType& Back() const noexcept { return set_.back(); }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { return static_cast<IndexType>(set_.size()); }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return kIndexTypeMax; }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return set_.empty(); }
    NODISCARD FORCEINLINE IndexType Count() const noexcept { return set_.count(); }


    NODISCARD FORCEINLINE Iterator Begin() noexcept { return set_.begin(); }
    NODISCARD FORCEINLINE ConstIterator Begin() const noexcept { return set_.begin(); }
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const noexcept { return set_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseBegin() noexcept { return set_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseBegin() const noexcept { return set_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseBegin() const noexcept { return set_.crbegin(); }
    NODISCARD FORCEINLINE Iterator End() noexcept { return set_.end(); }
    NODISCARD FORCEINLINE ConstIterator End() const noexcept { return set_.end(); }
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const noexcept { return set_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseEnd() noexcept { return set_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseEnd() const noexcept { return set_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseEnd() const noexcept { return set_.crend(); }

    NODISCARD FORCEINLINE Iterator Find(const ObjectType& value) noexcept { return set_.find(value); }
    NODISCARD FORCEINLINE ConstIterator Find(const ObjectType& value) const noexcept { return set_.find(value); }
    NODISCARD FORCEINLINE Iterator LowerBound(const ObjectType& value) noexcept { return set_.lower_bound(value); }
    NODISCARD FORCEINLINE ConstIterator LowerBound(const ObjectType& value) const noexcept { 
        return set_.lower_bound(value); 
    }
    NODISCARD FORCEINLINE Iterator UpperBound(const ObjectType& value) noexcept { return set_.upper_bound(value); }
    NODISCARD FORCEINLINE ConstIterator UpperBound(const ObjectType& value) const noexcept {
        return set_.upper_bound(value);
    }
    NODISCARD TPair<Iterator, Iterator> EqualRange(const ObjectType& value) noexcept {
        auto temp_pair = set_.equal_range(value);
        return TPair<Iterator, Iterator>(temp_pair.first, temp_pair.second);
    }
    NODISCARD TPair<ConstIterator, ConstIterator> EqualRange(const ObjectType& value) const noexcept {
        auto temp_pair = set_.equal_range(value);
        return TPair<ConstIterator, ConstIterator>(temp_pair.first, temp_pair.second);
    }

    FORCEINLINE TPair<Iterator, Bool> Insert(const ObjectType& value) noexcept {
        auto temp_pair = set_.insert(value);
        return TPair<Iterator, Bool>(temp_pair.first, temp_pair.second);
    }
    FORCEINLINE TPair<Iterator, Bool> Insert(ObjectType&& value) noexcept {
        auto temp_pair = set_.insert(std::forward<ObjectType>(value));
        return TPair<Iterator, Bool>(temp_pair.first, temp_pair.second);
    }
    FORCEINLINE Iterator Insert(ConstIterator hint_pos, const ObjectType& value) noexcept {
        return set_.insert(hint_pos, value);
    }
    FORCEINLINE Iterator Insert(ConstIterator hint_pos, ObjectType&& value) noexcept {
        return set_.insert(hint_pos, std::forward<ObjectType>(value));
    }
    template <typename InputIterator>
    FORCEINLINE Iterator Insert(ConstIterator pos, InputIterator first, InputIterator last) noexcept {
        return set_.insert(pos, first, last);
    }
    FORCEINLINE Iterator Insert(ConstIterator pos, InitializerList init_list) noexcept {
        return set_.insert(pos, init_list);
    }

    FORCEINLINE Iterator Erase(const ObjectType& value) noexcept { return set_.erase(value); }
    FORCEINLINE Iterator Erase(Iterator pos) noexcept { return set_.erase(pos); }
    FORCEINLINE Iterator Erase(ConstIterator first, ConstIterator last) noexcept { return set_.erase(first, last); }
    FORCEINLINE Void Clear() noexcept { set_.clear(); }

    FORCEINLINE Void Swap(TSet& set) noexcept { set_.swap(set); }

protected:
    using SuperType = ZObject;

private:
    STDSet set_;
};

}//zengine

#endif // !Z_CORE_T_SET_H_