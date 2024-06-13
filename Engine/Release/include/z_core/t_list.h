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
#ifndef Z_CORE_T_LIST_H_
#define Z_CORE_T_LIST_H_

#include "internal/z_drive.h"

#include <list>

#include "t_allocator.h"
#include "z_object.h"

namespace zengine {

/*
    List caintainer.
*/
template<typename ObjectType>
class TList : public ZObject {
public:
    using STDList = std::list<ObjectType, TAllocator<ObjectType>>;
    using Iterator = STDList::iterator;
    using ConstIterator = STDList::const_iterator;
    using ReverseIterator = STDList::reverse_iterator;
    using ConstReverseIterator = STDList::const_reverse_iterator;
    using InitializerList = std::initializer_list<ObjectType>;

    FORCEINLINE TList() noexcept : SuperType(), list_() {}
    FORCEINLINE TList(const TList& list) noexcept : 
        SuperType(), list_(list.list_) {}
    FORCEINLINE TList(TList&& list) noexcept : 
        SuperType(), list_(std::move(list.list_)) {}

    FORCEINLINE TList(SizeType size) noexcept : SuperType(), list_(size) {}
    FORCEINLINE TList(SizeType size, const ZObject& value) noexcept : SuperType(), list_(size, value) {}
    template <typename InputIterator>
    FORCEINLINE TList(InputIterator first, InputIterator last) noexcept : 
        SuperType(), list_(first, last) {}
    FORCEINLINE TList(InitializerList init_list) noexcept : SuperType(), list_(init_list) {}
 
    FORCEINLINE ~TList() noexcept {}

    FORCEINLINE TList& operator=(const TList& list) noexcept { 
        list_.operator=(list.list_);
        return *this;
    }
    FORCEINLINE TList& operator=(TList&& list) noexcept { 
        list_.operator=(std::move(list.list_));
        return *this;
    }
    FORCEINLINE TList& operator=(InitializerList init_list) noexcept {
        list_.operator=(init_list);
        return *this;
    }

    FORCEINLINE Void Assign(SizeType size, const ObjectType& value) noexcept {
        return list_.assign(size, value);
    }
    template <class InputIterator>
    FORCEINLINE Void Assign(InputIterator first, InputIterator last) noexcept {
        return list_.assign(first, last);
    }
    FORCEINLINE Void Assign(InitializerList init_list) noexcept {
        return list_.assign(init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TList& list) noexcept { 
        return list_ == list.list_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TList& list) noexcept { 
        return list_ != list.list_;
    }

    NODISCARD FORCEINLINE ObjectType& Front() noexcept { return list_.front(); }
    NODISCARD FORCEINLINE const ObjectType& Front() const noexcept { return list_.front(); }
    NODISCARD FORCEINLINE ObjectType& Back() noexcept { return list_.back(); }
    NODISCARD FORCEINLINE const ObjectType& Back() const noexcept { return list_.back(); }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { return list_.size(); }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return list_.max_size(); }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return list_.empty(); }

    NODISCARD FORCEINLINE Iterator Begin() noexcept { return list_.begin(); }
    NODISCARD FORCEINLINE ConstIterator Begin() const noexcept { return list_.begin(); }
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const noexcept { return list_.cbegin(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseBegin() noexcept { return list_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseBegin() const noexcept { return list_.rbegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseBegin() const noexcept { return list_.crbegin(); }
    NODISCARD FORCEINLINE Iterator End() noexcept { return list_.end(); }
    NODISCARD FORCEINLINE ConstIterator End() const noexcept { return list_.end(); }
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const noexcept { return list_.cend(); }
    NODISCARD FORCEINLINE ReverseIterator ReverseEnd() noexcept { return list_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ReverseEnd() const noexcept { return list_.rend(); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseEnd() const noexcept { return list_.crend(); }

    template <typename... ArgsType>
    FORCEINLINE Iterator EmplaceFront(ArgsType&&... args) noexcept {
        return list_.emplace_front(std::forward<ArgsType>(args)...);
    }
    FORCEINLINE Void PushFront(const ObjectType& value) noexcept { list_.push_front(value); }
    FORCEINLINE Void PushFront(ObjectType&& value) noexcept { list_.push_front(std::forward<ObjectType>(value)); }
    FORCEINLINE Void PopFront() noexcept { list_.pop_front(); }

    template <typename... ArgsType>
    FORCEINLINE Iterator EmplaceBack(ArgsType&&... args) noexcept {
        return list_.emplace_back(std::forward<ArgsType>(args)...);
    }
    FORCEINLINE Void PushBack(const ObjectType& value) noexcept { list_.push_back(value); }
    FORCEINLINE Void PushBack(ObjectType&& value) noexcept { list_.push_back(std::forward<ObjectType>(value)); }
    FORCEINLINE Void PopBack() noexcept { list_.push_back(); }

    template <typename... ArgsType>
    FORCEINLINE Iterator Emplace(ConstIterator pos, ArgsType&&... args) noexcept {
        return list_.emplace(pos, std::forward<ArgsType>(args)...);
    }
    FORCEINLINE Iterator Insert(ConstIterator pos, const ObjectType& value) noexcept {
        return list_.insert(pos, value);
    }
    FORCEINLINE Iterator Insert(ConstIterator pos, ObjectType&& value) noexcept {
        return list_.insert(pos, std::forward<ObjectType>(value));
    }
    FORCEINLINE Iterator Insert(ConstIterator pos, SizeType num, const ObjectType& value) noexcept {
        return list_.insert(pos, num, value);
    }
    template <typename InputIterator>
    FORCEINLINE Iterator Insert(ConstIterator pos, InputIterator first, InputIterator last) noexcept {
        return list_.insert(pos, first, last);
    }
    FORCEINLINE Iterator Insert(ConstIterator pos, InitializerList init_list) noexcept {
        return list_.insert(pos, init_list);
    }

    FORCEINLINE Iterator Splice(ConstIterator pos, TList& list) noexcept {
        return list_.splice(pos, list.list_);
    }
    FORCEINLINE Iterator Splice(ConstIterator pos, TList&& list) noexcept {
        return list_.splice(pos, std::move(list.list_));
    }
    FORCEINLINE Iterator Splice(ConstIterator pos, TList& list, ConstIterator start_pos) noexcept {
        return list_.splice(pos, list.list_, start_pos);
    }
    FORCEINLINE Iterator Splice(ConstIterator pos, TList&& list, ConstIterator start_pos) noexcept {
        return list_.splice(pos, std::move(list.list_), start_pos);
    }
    FORCEINLINE Iterator Splice(ConstIterator pos, TList& list,
                                     ConstIterator first, ConstIterator last) noexcept {
        return list_.splice(pos, list.list_, first, last);
    }
    FORCEINLINE Iterator Splice(ConstIterator pos, TList&& list,
                                     ConstIterator first, ConstIterator last) noexcept {
        return list_.splice(pos, std::move(list.list_), first, last);
    }

    FORCEINLINE Iterator Erase(ConstIterator pos) noexcept { return list_.erase(pos); }
    FORCEINLINE Iterator Erase(ConstIterator first, ConstIterator last) noexcept {
        return list_.erase(first, last);
    }

    /*
        Remove all the objects same as the given value.
    */
    FORCEINLINE Void Remove(const ObjectType& value) noexcept {
        list_.remove(value);
    }
    /*
        Remove all the objects that comply with the fucntion.
    */
    template <typename JudgeFunction>
    FORCEINLINE Void RemoveIf(JudgeFunction func) noexcept {
        list_.remove_if(func);
    }
    /*
        Make all the objects unique.
    */
    FORCEINLINE Void Unique() noexcept {
        list_.unique();
    }
    /*
        Make all the objects unique that comply with the fucntion.
    */
    template <typename JudgeFunction>
    FORCEINLINE Void UniqueIf(JudgeFunction func) noexcept {
        list_.unique(func);
    }

    /*
        Merge the two lists together sorted.
    */
    FORCEINLINE Void Merge(TList& list) {
        list_.unique(list);
    }
    /*
        Merge the two lists together sorted.
    */
    FORCEINLINE Void Merge(TList&& list) {
        list_.unique(std::move(list.list_));
    }
    /*
        Merge the two lists together sorted.
    */
    template <typename CompareFunction>
    FORCEINLINE Void Merge(TList& list, CompareFunction func) {
        list_.unique(list, func);
    }
    /*
        Merge the two lists together sorted.
    */
    template <typename CompareFunction>
    FORCEINLINE Void Merge(TList&& list, CompareFunction func) {
        list_.unique(std::move(list.list_), func);
    }

    FORCEINLINE Void Sort() noexcept {
        list_.sort();
    }
    template <typename CompareFunction>
    FORCEINLINE Void Sort(CompareFunction func) noexcept {
        list_.sort(func);
    }

    FORCEINLINE Void Clear() const noexcept { list_.clear(); }

    FORCEINLINE Void Resize(SizeType size) noexcept { list_.resize(size); }
    FORCEINLINE Void Resize(SizeType size, const ObjectType& value) noexcept { list_.resize(size, value); }

    FORCEINLINE Void Reverse() noexcept { list_.reverse(); }

    FORCEINLINE Void Swap(TList& list) noexcept { list_.swap(list); }

protected:
    using SuperType = ZObject;

private:
    STDList list_;
};

}//zengine

#endif // !Z_CORE_T_LIST_H_