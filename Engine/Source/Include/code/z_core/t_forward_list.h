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
#ifndef Z_CORE_T_FORWARD_LIST_H_
#define Z_CORE_T_FORWARD_LIST_H_

#include "internal/z_drive.h"

#include <forward_list>

#include "t_allocator.h"
#include "z_object.h"

namespace zengine {

/*
    Forward List caintainer.
*/
template<typename ObjectType>
class TForwardList : public ZObject {
public:
    using STDForwardList = std::forward_list<ObjectType, TAllocator<ObjectType>>;
    using Iterator = STDForwardList::iterator;
    using ConstIterator = STDForwardList::const_iterator;
    using InitializerList = std::initializer_list<ObjectType>;

    FORCEINLINE TForwardList() noexcept : SuperType(), forward_list_() {}
    FORCEINLINE TForwardList(const TForwardList& forward_list) noexcept : 
        SuperType(), forward_list_(forward_list.forward_list_) {}
    FORCEINLINE TForwardList(TForwardList&& forward_list) noexcept : 
        SuperType(), forward_list_(std::move(forward_list.forward_list_)) {}

    FORCEINLINE TForwardList(SizeType size) noexcept : SuperType(), forward_list_(size) {}
    FORCEINLINE TForwardList(SizeType size, const ZObject& value) noexcept : SuperType(), forward_list_(size, value) {}
    template <typename InputIterator>
    FORCEINLINE TForwardList(InputIterator first, InputIterator last) noexcept : 
        SuperType(), forward_list_(first, last) {}
    FORCEINLINE TForwardList(InitializerList init_list) noexcept : SuperType(), forward_list_(init_list) {}
 
    FORCEINLINE ~TForwardList() noexcept {}

    FORCEINLINE TForwardList& operator=(const TForwardList& forward_list) noexcept { 
        forward_list_.operator=(forward_list.forward_list_);
        return *this;
    }
    FORCEINLINE TForwardList& operator=(TForwardList&& forward_list) noexcept { 
        forward_list_.operator=(std::move(forward_list.forward_list_));
        return *this;
    }
    FORCEINLINE TForwardList& operator=(InitializerList init_list) noexcept {
        forward_list_.operator=(init_list);
        return *this;
    }

    FORCEINLINE Void Assign(SizeType size, const ObjectType& value) noexcept {
        return forward_list_.assign(size, value);
    }
    template <class InputIterator>
    FORCEINLINE Void Assign(InputIterator first, InputIterator last) noexcept {
        return forward_list_.assign(first, last);
    }
    FORCEINLINE Void Assign(InitializerList init_list) noexcept {
        return forward_list_.assign(init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TForwardList& forward_list) noexcept { 
        return forward_list_ == forward_list.forward_list_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const TForwardList& forward_list) noexcept { 
        return forward_list_ != forward_list.forward_list_;
    }

    NODISCARD FORCEINLINE ObjectType& Front() noexcept { return forward_list_.front(); }
    NODISCARD FORCEINLINE const ObjectType& Front() const noexcept { return forward_list_.front(); }

    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return forward_list_.max_size(); }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return forward_list_.empty(); }

    NODISCARD FORCEINLINE Iterator BeforeBegin() noexcept { return forward_list_.before_begin(); }
    NODISCARD FORCEINLINE ConstIterator BeforeBegin() const noexcept { return forward_list_.before_begin(); }
    NODISCARD FORCEINLINE ConstIterator ConstBeforeBegin() const noexcept { return forward_list_.before_begin(); }
    NODISCARD FORCEINLINE Iterator Begin() noexcept { return forward_list_.begin(); }
    NODISCARD FORCEINLINE ConstIterator Begin() const noexcept { return forward_list_.begin(); }
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const noexcept { return forward_list_.cbegin(); }
    NODISCARD FORCEINLINE Iterator End() noexcept { return forward_list_.end(); }
    NODISCARD FORCEINLINE ConstIterator End() const noexcept { return forward_list_.end(); }
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const noexcept { return forward_list_.cend(); }

    template <typename... ArgsType>
    FORCEINLINE Iterator EmplaceFront(ArgsType&&... args) noexcept {
        return forward_list_.emplace_front(std::forward<ArgsType>(args)...);
    }
    FORCEINLINE Void PushFront(const ObjectType& value) noexcept { forward_list_.push_front(value); }
    FORCEINLINE Void PushFront(ObjectType&& value) noexcept { forward_list_.push_front(std::forward<ObjectType>(value)); }
    FORCEINLINE Void PopFront() noexcept { forward_list_.pop_front(); }

    template <typename... ArgsType>
    FORCEINLINE Iterator EmplaceAfter(ConstIterator pos, ArgsType&&... args) noexcept {
        return forward_list_.emplace_after(pos, std::forward<ArgsType>(args)...);
    }
    FORCEINLINE Iterator InsertAfter(ConstIterator pos, const ObjectType& value) noexcept {
        return forward_list_.insert_after(pos, value);
    }
    FORCEINLINE Iterator InsertAfter(ConstIterator pos, ObjectType&& value) noexcept {
        return forward_list_.insert_after(pos, std::forward<ObjectType>(value));
    }
    FORCEINLINE Iterator InsertAfter(ConstIterator pos, SizeType num, const ObjectType& value) noexcept {
        return forward_list_.insert_after(pos, num, value);
    }
    template <typename InputIterator>
    FORCEINLINE Iterator InsertAfter(ConstIterator pos, InputIterator first, InputIterator last) noexcept {
        return forward_list_.insert_after(pos, first, last);
    }
    FORCEINLINE Iterator InsertAfter(ConstIterator pos, InitializerList init_list) noexcept {
        return forward_list_.insert_after(pos, init_list);
    }

    FORCEINLINE Iterator SpliceAfter(ConstIterator pos, TForwardList& forward_list) noexcept {
        return forward_list_.splice_after(pos, forward_list.forward_list_);
    }
    FORCEINLINE Iterator SpliceAfter(ConstIterator pos, TForwardList&& forward_list) noexcept {
        return forward_list_.splice_after(pos, std::move(forward_list.forward_list_));
    }
    FORCEINLINE Iterator SpliceAfter(ConstIterator pos, TForwardList& forward_list, ConstIterator start_pos) noexcept {
        return forward_list_.splice_after(pos, forward_list.forward_list_, start_pos);
    }
    FORCEINLINE Iterator SpliceAfter(ConstIterator pos, TForwardList&& forward_list, ConstIterator start_pos) noexcept {
        return forward_list_.splice_after(pos, std::move(forward_list.forward_list_), start_pos);
    }
    FORCEINLINE Iterator SpliceAfter(ConstIterator pos, TForwardList& forward_list, 
                                     ConstIterator first, ConstIterator last) noexcept {
        return forward_list_.splice_after(pos, forward_list.forward_list_, first, last);
    }
    FORCEINLINE Iterator SpliceAfter(ConstIterator pos, TForwardList&& forward_list, 
                                     ConstIterator first, ConstIterator last) noexcept {
        return forward_list_.splice_after(pos, std::move(forward_list.forward_list_), first, last);
    }

    FORCEINLINE Iterator EraseAfter(ConstIterator pos) noexcept { return forward_list_.erase_after(pos); }
    FORCEINLINE Iterator EraseAfter(ConstIterator first, ConstIterator last) noexcept {
        return forward_list_.erase_after(first, last);
    }

    /*
        Remove all the objects same as the given value.
    */
    FORCEINLINE Void Remove(const ObjectType& value) noexcept {
        forward_list_.remove(value);
    }
    /*
        Remove all the objects that comply with the fucntion.
    */
    template <typename JudgeFunction>
    FORCEINLINE Void RemoveIf(JudgeFunction func) noexcept {
        forward_list_.remove_if(func);
    }
    /*
        Make all the objects unique.
    */
    FORCEINLINE Void Unique() noexcept {
        forward_list_.unique();
    }
    /*
        Make all the objects unique that comply with the fucntion.
    */
    template <typename JudgeFunction>
    FORCEINLINE Void UniqueIf(JudgeFunction func) noexcept {
        forward_list_.unique(func);
    }

    /*
        Merge the two lists together sorted.
    */
    FORCEINLINE Void Merge(TForwardList& forward_list) {
        forward_list_.unique(forward_list.forward_list_);
    }
    /*
        Merge the two lists together sorted.
    */
    FORCEINLINE Void Merge(TForwardList&& forward_list) {
        forward_list_.unique(std::move(forward_list.forward_list_));
    }
    /*
        Merge the two lists together sorted.
    */
    template <typename CompareFunction>
    FORCEINLINE Void Merge(TForwardList& forward_list, CompareFunction func) {
        forward_list_.unique(forward_list.forward_list_, func);
    }
    /*
        Merge the two lists together sorted.
    */
    template <typename CompareFunction>
    FORCEINLINE Void Merge(TForwardList&& forward_list, CompareFunction func) {
        forward_list_.unique(std::move(forward_list.forward_list_), func);
    }

    FORCEINLINE Void Sort() noexcept {
        forward_list_.sort();
    }
    template <typename CompareFunction>
    FORCEINLINE Void Sort(CompareFunction func) noexcept {
        forward_list_.sort(func);
    }

    FORCEINLINE Void Clear() const noexcept { forward_list_.clear(); }

    FORCEINLINE Void Resize(SizeType size) noexcept { forward_list_.resize(size); }
    FORCEINLINE Void Resize(SizeType size, const ObjectType& value) noexcept { forward_list_.resize(size, value); }

    FORCEINLINE Void Reverse() noexcept { forward_list_.reverse(); }

    FORCEINLINE Void Swap(TForwardList& forward_list) noexcept { forward_list_.swap(forward_list.forward_list_); }

protected:
    using SuperType = ZObject;

private:
    STDForwardList forward_list_;
};

}//zengine

#endif // !Z_CORE_T_FORWARD_LIST_H_S