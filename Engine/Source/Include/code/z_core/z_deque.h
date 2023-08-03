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
#ifndef Z_CORE_Z_DEQUE_H_
#define Z_CORE_Z_DEQUE_H_

#include "internal/drive.h"

#include "m_error_message.h"
#include "z_object.h"

namespace zengine {

namespace internal {

template<typename ObjectType>
class ZSimpleDequeIteratorBase {
public:
    FORCEINLINE ZSimpleDequeIteratorBase(ObjectType* object_ptr, ObjectType* begin_ptr, ObjectType* end_ptr) 
        : object_ptr_(object_ptr), begin_ptr_(begin_ptr), end_ptr_(end_ptr) {}
    FORCEINLINE ZSimpleDequeIteratorBase(const ZSimpleDequeIteratorBase& iterator) { CopyP(iterator); }
    FORCEINLINE ZSimpleDequeIteratorBase(ZSimpleDequeIteratorBase&& iterator) { 
        MoveP(std::forward<ZSimpleDequeIteratorBase>(iterator)); 
    }

    FORCEINLINE ZSimpleDequeIteratorBase& operator=(const ZSimpleDequeIteratorBase& iterator) {
        CopyP(iterator);
        return *this;
    }
    FORCEINLINE ZSimpleDequeIteratorBase& operator=(ZSimpleDequeIteratorBase&& iterator) {
        MoveP(std::forward<ZSimpleDequeIteratorBase>(iterator));
        return *this;
    }

    NODISCARD FORCEINLINE Bool operator==(const ZSimpleDequeIteratorBase& iterator) const {
        return object_ptr_ == iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const ZSimpleDequeIteratorBase& iterator) const {
        return object_ptr_ != iterator.object_ptr_;
    }

    NODISCARD FORCEINLINE ObjectType& operator*() const { return *object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* operator->() const { return object_ptr_; }


    NODISCARD FORCEINLINE operator ObjectType* () const { return object_ptr_; }

    FORCEINLINE ~ZSimpleDequeIteratorBase() {}

    NODISCARD FORCEINLINE ObjectType& object() const { return *object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* object_ptr() const { return object_ptr_; }

protected:
    ObjectType* FindObject(IndexType offset) noexcept {
        ObjectType* temp_ptr = object_ptr_ + offset;
        if (temp_ptr >= end_ptr_) {
            temp_ptr -= end_ptr_ - begin_ptr_;
        }
        else if (temp_ptr <= end_ptr_) {
            temp_ptr += end_ptr_ - begin_ptr_;
        }
        return temp_ptr;
    }

    ObjectType* object_ptr_;
    ObjectType* begin_ptr_;
    ObjectType* end_ptr_;

private:
    FORCEINLINE Void CopyP(const ZSimpleDequeIteratorBase& iterator) {
        object_ptr_ = iterator.object_ptr_;
        begin_ptr_ = iterator.begin_ptr_;
        end_ptr_ = iterator.end_ptr_;
    }

    FORCEINLINE Void MoveP(ZSimpleDequeIteratorBase&& iterator) {
        object_ptr_ = iterator.object_ptr_;
        begin_ptr_ = iterator.begin_ptr_;
        end_ptr_ = iterator.end_ptr_;
        iterator.object_ptr_ = nullptr;
        iterator.begin_ptr_ = nullptr;
        iterator.end_ptr_ = nullptr;
    }
};

template<typename ObjectType>
class ZSimpleDequeIterator : public ZSimpleDequeIteratorBase<ObjectType> {
public:
    FORCEINLINE ZSimpleDequeIterator& operator=(ObjectType* object_ptr) {
        SuperType::object_ptr_ = object_ptr;
        return *this;
    }

    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) const {
        return *SuperType::FindObject(index);
    }

    FORCEINLINE ZSimpleDequeIterator& operator+=(IndexType data_num) {
        SuperType::object_ptr_ = SuperType::FindObject(data_num);
        return *this;
    }
    FORCEINLINE ZSimpleDequeIterator& operator-=(IndexType data_num) {
        SuperType::object_ptr_ = SuperType::FindObject(-data_num);
        return *this;
    }

    FORCEINLINE ZSimpleDequeIterator& operator++() {
        return SuperType::object_ptr_[++SuperType::object_ptr_ >= SuperType::end_ptr_ ?
                                      SuperType::object_ptr_ - (SuperType::end_ptr_ - SuperType::begin_ptr_): 
                                      SuperType::object_ptr_];
        return *this;
    }
    FORCEINLINE ZSimpleDequeIterator& operator++(IndexType) {
        return SuperType::object_ptr_[++SuperType::object_ptr_ >= SuperType::end_ptr_ ?
                                      SuperType::object_ptr_ - (SuperType::end_ptr_ - SuperType::begin_ptr_): 
                                      SuperType::object_ptr_];
        return *this;
    }
    FORCEINLINE ZSimpleDequeIterator& operator--() {
        return SuperType::object_ptr_[--SuperType::object_ptr_ >= SuperType::end_ptr_ ?
                                      SuperType::object_ptr_ - (SuperType::end_ptr_ - SuperType::begin_ptr_): 
                                      SuperType::object_ptr_];
        return *this;
    }
    FORCEINLINE ZSimpleDequeIterator& operator--(IndexType) {
        return SuperType::object_ptr_[--SuperType::object_ptr_ >= SuperType::end_ptr_ ?
                                      SuperType::object_ptr_ - (SuperType::end_ptr_ - SuperType::begin_ptr_): 
                                      SuperType::object_ptr_];
        return *this;
    }
    NODISCARD FORCEINLINE ZSimpleDequeIterator operator+(IndexType data_num) const {
        return ZSimpleDequeIterator(SuperType::FindObject(data_num), SuperType::begin_ptr_, SuperType::end_ptr_);
    }
    NODISCARD FORCEINLINE ZSimpleDequeIterator operator-(IndexType data_num) const {
        return ZSimpleDequeIterator(SuperType::FindObject(-data_num), SuperType::begin_ptr_, SuperType::end_ptr_);
    }

protected:
    using SuperType = ZSimpleDequeIteratorBase<ObjectType>;
};

template<typename ObjectType>
class ZSimpleDequeReverseIterator : public ZSimpleDequeIteratorBase<ObjectType> {
public:
    FORCEINLINE ZSimpleDequeReverseIterator& operator=(ObjectType* object_ptr) {
        SuperType::object_ptr_ = object_ptr;
        return *this;
    }

    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) const {
        return *SuperType::FindObject(-index);
    }

    FORCEINLINE ZSimpleDequeReverseIterator& operator+=(IndexType data_num) {
        SuperType::object_ptr_ = SuperType::FindObject(-data_num);
        return *this;
    }
    FORCEINLINE ZSimpleDequeReverseIterator& operator-=(IndexType data_num) {
        SuperType::object_ptr_ = SuperType::FindObject(data_num);
        return *this;
    }

    FORCEINLINE ZSimpleDequeReverseIterator& operator++() {
        return SuperType::object_ptr_[--SuperType::object_ptr_ >= SuperType::end_ptr_ ?
                                      SuperType::object_ptr_ - (SuperType::end_ptr_ - SuperType::begin_ptr_): 
                                      SuperType::object_ptr_];
        return *this;
    }
    FORCEINLINE ZSimpleDequeReverseIterator& operator++(IndexType) {
        return SuperType::object_ptr_[--SuperType::object_ptr_ >= SuperType::end_ptr_ ?
                                      SuperType::object_ptr_ - (SuperType::end_ptr_ - SuperType::begin_ptr_): 
                                      SuperType::object_ptr_];
        return *this;
    }
    FORCEINLINE ZSimpleDequeReverseIterator& operator--() {
        return SuperType::object_ptr_[++SuperType::object_ptr_ >= SuperType::end_ptr_ ?
                                      SuperType::object_ptr_ - (SuperType::end_ptr_ - SuperType::begin_ptr_): 
                                      SuperType::object_ptr_];
        return *this;
    }
    FORCEINLINE ZSimpleDequeReverseIterator& operator--(IndexType) {
        return SuperType::object_ptr_[++SuperType::object_ptr_ >= SuperType::end_ptr_ ?
                                      SuperType::object_ptr_ - (SuperType::end_ptr_ - SuperType::begin_ptr_): 
                                      SuperType::object_ptr_];
        return *this;
    }
    NODISCARD FORCEINLINE ZSimpleDequeReverseIterator operator+(IndexType data_num) const {
        return ZSimpleDequeReverseIterator(SuperType::FindObject(-data_num), SuperType::begin_ptr_, SuperType::end_ptr_);
    }
    NODISCARD FORCEINLINE ZSimpleDequeReverseIterator operator-(IndexType data_num) const {
        return ZSimpleDequeReverseIterator(SuperType::FindObject(+data_num), SuperType::begin_ptr_, SuperType::end_ptr_);
    }

protected:
    using SuperType = ZSimpleDequeIteratorBase<ObjectType>;
};

/*
    Order iterator types.
*/
template<typename IteratorType, typename ObjectType>
concept kIsOrderSimpleDequeIterator = kSameType<IteratorType, ZSimpleDequeIterator<ObjectType>> ||
                                    kSameType<IteratorType, ZSimpleDequeIterator<const ObjectType>>;

/*
    Reverse iterator types.
*/
template<typename IteratorType, typename ObjectType>
concept kIsReverseSimpleDequeIterator = kSameType<IteratorType, ZSimpleDequeReverseIterator<ObjectType>> ||
                                      kSameType<IteratorType, ZSimpleDequeReverseIterator<const ObjectType>>;

/*
    Const iterator types.
*/
template<typename IteratorType, typename ObjectType>
concept kIsConstSimpleDequeIterator = kSameType<IteratorType, ZSimpleDequeIterator<const ObjectType>> ||
                                    kSameType<IteratorType, ZSimpleDequeReverseIterator<const ObjectType>>;

/*
    Non Const iterator types.
*/
template<typename IteratorType, typename ObjectType>
concept kIsNonConstSimpleDequeIterator = kSameType<IteratorType, ZSimpleDequeIterator<ObjectType>> ||
                                       kSameType<IteratorType, ZSimpleDequeReverseIterator<ObjectType>>;


/*
    All iterator types.
*/
template<typename IteratorType, typename ObjectType>
concept kIsSimpleDequeIterator = kIsOrderSimpleDequeIterator<IteratorType, ObjectType> ||
                               kIsReverseSimpleDequeIterator<IteratorType, ObjectType>;

}//internal

/*
    Simple deque caintainer. Uses smaller space than the deque. 
    The porformence of push and pop is always o(1) unless the the container is
    out of space, but the push and pop is slightly slower then the deque.
    If kIfUnique is true, will call the constuctor when the object is created.
*/
template<typename ObjectType, Bool kIfUnique = kIsClass<ObjectType>>
class ZSimpleDeque : public ZObject {
private:
    static constexpr Float32 kAutoExtendMulFactor = 1.5F;

public:
    using Iterator = internal::ZSimpleDequeIterator<ObjectType>;
    using ConstIterator = internal::ZSimpleDequeIterator<const ObjectType>;
    using ReverseIterator = internal::ZSimpleDequeReverseIterator<ObjectType>;
    using ConstReverseIterator = internal::ZSimpleDequeReverseIterator<const ObjectType>;

    ZSimpleDeque() noexcept;
    ZSimpleDeque(const ZSimpleDeque& deque) noexcept;
    ZSimpleDeque(ZSimpleDeque&& deque) noexcept;

    ZSimpleDeque(IndexType capacity) noexcept;
    /*
        Fills the container by the object constructed by the arguements.
    */
    template<typename... ArgsType>
    ZSimpleDeque(IndexType capacity, ArgsType&&... args) noexcept;    

    ZSimpleDeque(Iterator begin, Iterator end) noexcept : SuperType() { ZSimpleDequeP(begin, end); }
    ZSimpleDeque(ConstIterator begin, ConstIterator end) noexcept : SuperType() { ZSimpleDequeP(begin, end); }
    ZSimpleDeque(ReverseIterator begin, ReverseIterator end) noexcept : SuperType() { ZSimpleDequeP(begin, end); }
    ZSimpleDeque(ConstReverseIterator begin, ConstReverseIterator end) noexcept 
        : SuperType() { ZSimpleDequeP(begin, end); }

    FORCEINLINE ZSimpleDeque& operator=(const ZSimpleDeque& deque);
    inline ZSimpleDeque& operator=(ZSimpleDeque&& deque) noexcept;

    NODISCARD FORCEINLINE Bool operator==(const ZSimpleDeque& deque) noexcept { 
        return this == &deque;
    }    
    NODISCARD FORCEINLINE Bool operator!=(const ZSimpleDeque& deque) noexcept { 
        return this != &deque;
    }

    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) {
        DEBUG(index < 0 || index >= size(), "Index out of bounds!");
        return data_ptr_[(index += begin_index_) >= capacity_ ? index - capacity_ : index];
    }
    NODISCARD FORCEINLINE const ObjectType& operator[](IndexType index) const {
        DEBUG(index < 0 || index >= size(), "Index out of bounds!");
        return data_ptr_[(index += begin_index_) >= capacity_ ? index - capacity_ : index];
    }

    ~ZSimpleDeque() noexcept;

    NODISCARD FORCEINLINE ObjectType& At(IndexType index) {
        DEBUG(index < 0 || index >= size(), "Index out of bounds!");
        return data_ptr_[(index += begin_index_) >= capacity_ ? index - capacity_ : index];
    }
    NODISCARD FORCEINLINE const ObjectType& At(IndexType index) const {
        DEBUG(index < 0 || index >= size(), "Index out of bounds!");
        return data_ptr_[(index += begin_index_) >= capacity_ ? index - capacity_ : index];
    }
    NODISCARD FORCEINLINE ObjectType* AtPtr(IndexType index) {
        DEBUG(index < 0 || index >= size(), "Index out of bounds!");
        return data_ptr_ + ((index += begin_index_) >= capacity_ ? index - capacity_ : index);
    }
    NODISCARD FORCEINLINE const ObjectType* AtPtr(IndexType index) const {
        DEBUG(index < 0 || index >= size(), "Index out of bounds!");
        return data_ptr_ + ((index += begin_index_) >= capacity_ ? index - capacity_ : index);
    }
    NODISCARD FORCEINLINE ObjectType& Front() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[begin_index_]; 
    }
    NODISCARD FORCEINLINE const ObjectType& Front() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[begin_index_];
    }
    NODISCARD FORCEINLINE ObjectType* FrontPtr() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_ + begin_index_;
    }
    NODISCARD FORCEINLINE const ObjectType* FrontPtr() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_ + begin_index_;
    }
    NODISCARD FORCEINLINE ObjectType& Back() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[end_index_ - 1];
    }
    NODISCARD FORCEINLINE const ObjectType& Back() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[end_index_ - 1];
    }
    NODISCARD FORCEINLINE ObjectType* BackPtr() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_ + (end_index_ - 1);
    }
    NODISCARD FORCEINLINE const ObjectType* BackPtr() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_ + (end_index_ - 1);
    }

    NODISCARD FORCEINLINE IndexType size() const { 
        return (begin_index_ > end_index_ ? end_index_ + capacity_ : end_index_) - begin_index_;
    }
    NODISCARD FORCEINLINE IndexType capacity() const { return capacity_; }
    NODISCARD FORCEINLINE const ObjectType* data_ptr() const { return data_ptr_; }

    /*
        The iterator funcions.
    */
    NODISCARD FORCEINLINE Iterator Begin() {
        DEBUG(data_ptr_ == nullptr, "Simple deque not exist!");
        return Iterator(data_ptr_ + begin_index_);
    }
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const { 
        DEBUG(data_ptr_ == nullptr, "Simple deque not exist!"); 
        return ConstIterator(data_ptr_ + begin_index_);
    }
    NODISCARD FORCEINLINE ReverseIterator ReverseBegin() { 
        DEBUG(data_ptr_ == nullptr, "Simple deque not exist!"); 
        return ReverseIterator(data_ptr_ + end_index_ - 1); 
    }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseBegin() const {
        DEBUG(data_ptr_ == nullptr, "Simple deque not exist!");
        return ConstReverseIterator(data_ptr_ + end_index_ - 1);
    }
    NODISCARD FORCEINLINE Iterator End() { 
        DEBUG(data_ptr_ == nullptr, "Simple deque not exist!"); 
        return Iterator(data_ptr_ + end_index_);
    }
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const { 
        DEBUG(data_ptr_ == nullptr, "Simple deque not exist!"); 
        return ConstIterator(data_ptr_ + end_index_);
    }
    NODISCARD FORCEINLINE ReverseIterator ReverseEnd() { 
        DEBUG(data_ptr_ == nullptr, "Simple deque not exist!"); 
        return ReverseIterator(data_ptr_ + begin_index_ - 1);
    }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseEnd() const {
        DEBUG(data_ptr_ == nullptr, "Simple deque not exist!");
        return ConstReverseIterator(data_ptr_ + begin_index_ - 1);
    }

    NODISCARD FORCEINLINE Bool Empty() { return begin_index_ == end_index_; }
    NODISCARD FORCEINLINE Bool Exist() { return data_ptr_ != nullptr; }

    /*
        Resize the deque, If the given size is smaller then the current size,
        the function will cut remove the extra objects. If the given size is
        bigger then the current size, the function will fill the extra size with
        the object constructed by the arguements.
    */
    template<typename... ArgsType>
    inline Void Resize(IndexType size, ArgsType&&... args) noexcept;

    /*
        Will extend the capacity by the given capacity, if the smaller then the
        given capacity, this function will do nothing, otherwise the capacity will
        entend to a size that equals or bigger then the given capacity.
    */
    FORCEINLINE Void Reserve(IndexType capacity);

    /*
        Shrinks the deque to the minimum capacity that can fit the current size.
    */
    FORCEINLINE Void ShrinkToFit();

    /*
        Remove the object at the front of the deque.
    */
    FORCEINLINE Void PopFront();
    /*
        Remove the object at the front of the deque.
        Give the authority to the given address.
    */
    FORCEINLINE Void PopFront(ObjectType* object_ptr);

    /*
        Create an object at the front of the deque by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the deque.
    */
    template<typename... ArgsType>
    FORCEINLINE Void PushFront(ArgsType&&... args);
    /*
        Create objects at the front of the deque by calling the constructor with
        the arguements. If kIfUnique is false and no arguements,
        will only add the size of the deque.
    */
    template<typename... ArgsType>
    inline Void PushFronts(IndexType num, ArgsType&&... args) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the deque.
    */
    FORCEINLINE Void PushFronts(Iterator src_begin, Iterator src_end) {
        PushFrontsP(src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the deque.
    */
    FORCEINLINE Void PushFronts(ConstIterator src_begin, ConstIterator src_end) {
        PushFrontsP(src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the deque.
    */
    FORCEINLINE Void PushFronts(ReverseIterator src_begin, ReverseIterator src_end) {
        PushFrontsP(src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the deque.
    */
    FORCEINLINE Void PushFronts(ConstReverseIterator src_begin, ConstReverseIterator src_end) {
        PushFrontsP(src_begin, src_end);
    }

    /*
        Replace the front object with the object constructed by the arguements.
    */
    template<typename... ArgsType>
    inline Void EmplaceFront(ArgsType&&... args) noexcept;

    /*
        Remove the object at the back of the deque.
    */
    FORCEINLINE Void PopBack();
    /*
        Remove the object at the back of the deque. 
        Give the authority to the given address.
    */
    FORCEINLINE Void PopBack(ObjectType* object_ptr);

    /*
        Create an object at the back of the deque by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the deque.
    */
    template<typename... ArgsType>
    FORCEINLINE Void PushBack(ArgsType&&... args);
    /*
        Create objects at the back of the deque by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, 
        will only add the size of the deque.
    */
    template<typename... ArgsType>
    inline Void PushBacks(IndexType num, ArgsType&&... args) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the deque.
    */
    FORCEINLINE Void PushBacks(Iterator src_begin, Iterator src_end) {
        PushBacksP(src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the deque.
    */
    FORCEINLINE Void PushBacks(ConstIterator src_begin, ConstIterator src_end) {
        PushBacksP(src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the deque.
    */
    FORCEINLINE Void PushBacks(ReverseIterator src_begin, ReverseIterator src_end) {
        PushBacksP(src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the deque.
    */
    FORCEINLINE Void PushBacks(ConstReverseIterator src_begin, ConstReverseIterator src_end) {
        PushBacksP(src_begin, src_end);
    }

    /*
        Replace the back object with the object constructed by the arguements.
    */
    template<typename... ArgsType>
    inline Void EmplaceBack(ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    FORCEINLINE Iterator Insert(IndexType index, ArgsType&&... args) { 
        return InsertP(Iterator(data_ptr_ + index), std::forward<ArgsType>(args)...);
    }
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE Iterator Insert(Iterator dst, ArgsType&&... args) {
        return InsertP(dst, std::forward<ArgsType>(args)...);
    }
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE ReverseIterator Insert(ReverseIterator dst, ArgsType&&... args) {
        return InsertP(dst, std::forward<ArgsType>(args)...);;
    }

    /*
        Inserts before the index. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    FORCEINLINE Iterator Inserts(IndexType index, IndexType num, ArgsType&&... args) {
        return InsertsP(Iterator(data_ptr_ + index), num, std::forward<ArgsType>(args)...);
    }
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE Iterator Inserts(Iterator dst, IndexType num, ArgsType&&... args) {
        return InsertsP(dst, num, std::forward<ArgsType>(args)...);
    }
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE ReverseIterator Inserts(ReverseIterator dst, IndexType num, ArgsType&&... args) {
        return InsertsP(dst, num, std::forward<ArgsType>(args)...);
    }

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE Iterator Inserts(IndexType index, Iterator src_begin, Iterator src_end) {
        return InsertsP(Iterator(data_ptr_ + index), src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE Iterator Inserts(IndexType index, ConstIterator src_begin, ConstIterator src_end) {
        return InsertsP(Iterator(data_ptr_ + index), src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE Iterator Inserts(IndexType index, ReverseIterator src_begin,ReverseIterator src_end) {
        return InsertsP(Iterator(data_ptr_ + index), src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE Iterator Inserts(IndexType index, ConstReverseIterator src_begin, ConstReverseIterator src_end) {
        return InsertsP(Iterator(data_ptr_ + index), src_begin, src_end);
    }

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE Iterator Inserts(Iterator dst, Iterator src_begin, Iterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE Iterator Inserts(Iterator dst, ConstIterator src_begin, ConstIterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE Iterator Inserts(Iterator dst, ReverseIterator src_begin, ReverseIterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE Iterator Inserts(Iterator dst, ConstReverseIterator src_begin, ConstReverseIterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE ReverseIterator Inserts(ReverseIterator dst, Iterator src_begin, Iterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE ReverseIterator Inserts(ReverseIterator dst, ConstIterator src_begin, ConstIterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE ReverseIterator Inserts(ReverseIterator dst, ReverseIterator src_begin, ReverseIterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE ReverseIterator Inserts(ReverseIterator dst, 
                                        ConstReverseIterator src_begin, ConstReverseIterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }

    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    FORCEINLINE Iterator Erase(IndexType index) { return EraseP(Iterator(data_ptr_ + index)); }
    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    FORCEINLINE Iterator Erase(Iterator dst) { return EraseP(dst); }
    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    FORCEINLINE ReverseIterator Erase(ReverseIterator dst) { return EraseP(dst); }

    /*
        Erases the num of objects that starts at the given index.
        Returns the iterator that points at the next object.
    */
    FORCEINLINE Iterator Erases(IndexType index, IndexType num) {
        return ErasesP(Iterator(data_ptr_ + index), Iterator(data_ptr_ + (index + num)));
    }
    /*
        Erases the num of objects that starts at the given iterator.
        Returns the iterator that points at the next object.
    */
    FORCEINLINE Iterator Erases(Iterator dst, IndexType num) { return ErasesP(dst, dst + num); }
    /*
        Erases the num of objects that starts at the given iterator.
        Returns the iterator that points at the next object.
    */
    FORCEINLINE ReverseIterator Erases(ReverseIterator dst, IndexType num) { return ErasesP(dst, dst + num); }
    /*
        Erases the object between begin and end, involves begin, but don't involves end.
        Returns the iterator that points at the next object.
    */
    FORCEINLINE Iterator Erases(Iterator dst_begin, Iterator dst_end) { return ErasesP(dst_begin, dst_end); }
    /*
        Erases the object between begin and end, involves begin, but don't involves end.
        Returns the iterator that points at the next object.
    */
    FORCEINLINE ReverseIterator Erases(ReverseIterator dst_begin, ReverseIterator dst_end) {
        return ErasesP(dst_begin, dst_end);
    }

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    FORCEINLINE Void Emplace(IndexType index, ArgsType&&... args) {
        EmplaceP(Iterator(data_ptr_ + index), std::forward<ArgsType>(args)...);
    }
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    FORCEINLINE Void Emplace(Iterator dst, ArgsType&&... args) {
        EmplaceP(dst, std::forward<ArgsType>(args)...);
    }
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    FORCEINLINE Void Emplace(ReverseIterator dst, ArgsType&&... args) {
        EmplaceP(dst, std::forward<ArgsType>(args)...);
    }


    /*
        Replace the certain num of objects that starts at the given place with 
        the object constructed by the given arguements.
    */
    template<typename... ArgsType>
    FORCEINLINE Void Emplaces(IndexType index, IndexType num, ArgsType&&... args) {
        EmplacesP(Iterator(data_ptr_ + index), num, std::forward<ArgsType>(args)...);
    }
    /*
        Replace the certain num of objects that starts at the given place with
        the object constructed by the given arguements.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE Void Emplaces(Iterator dst, IndexType num, ArgsType&&... args) {
        EmplacesP(dst, num, std::forward<ArgsType>(args)...);
    }
    /*
        Replace the certain num of objects that starts at the given place with
        the object constructed by the given arguements.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE Void Emplaces(ReverseIterator dst, IndexType num, ArgsType&&... args) {
        EmplacesP(dst, num, std::forward<ArgsType>(args)...);
    }

    /*
        Replace the objects that starts at the given place with the other objects given.
    */
    FORCEINLINE Void Emplaces(IndexType index, Iterator src_begin, Iterator src_end) {
        EmplacesP(Iterator(data_ptr_ + index), src_begin, src_end);
    }
    /*
        Replace the objects that starts at the given place with the other objects given.
    */
    FORCEINLINE Void Emplaces(IndexType index, ConstIterator src_begin, ConstIterator src_end) {
        EmplacesP(Iterator(data_ptr_ + index), src_begin, src_end);
    }
    /*
        Replace the objects that starts at the given place with the other objects given.
    */
    FORCEINLINE Void Emplaces(IndexType index, ReverseIterator src_begin,ReverseIterator src_end) {
        EmplacesP(Iterator(data_ptr_ + index), src_begin, src_end);
    }
    /*
        Replace the objects that starts at the given place with the other objects given.
    */
    FORCEINLINE Void Emplaces(IndexType index, ConstReverseIterator src_begin, ConstReverseIterator src_end) {
        EmplacesP(Iterator(data_ptr_ + index), src_begin, src_end);
    }

    /*
        Replace the objects that starts at the given place with the other objects given.
    */
    FORCEINLINE Void Emplaces(Iterator dst, Iterator src_begin, Iterator src_end) {
        EmplacesP(dst, src_begin, src_end);
    }
    /*
        Replace the objects that starts at the given place with the other objects given.
    */
    FORCEINLINE Void Emplaces(Iterator dst, ConstIterator src_begin, ConstIterator src_end) {
        EmplacesP(dst, src_begin, src_end);
    }
    /*
        Replace the objects that starts at the given place with the other objects given.
    */
    FORCEINLINE Void Emplaces(Iterator dst, ReverseIterator src_begin, ReverseIterator src_end) {
        EmplacesP(dst, src_begin, src_end);
    }
    /*
        Replace the objects that starts at the given place with the other objects given.
    */
    FORCEINLINE Void Emplaces(Iterator dst, ConstReverseIterator src_begin, ConstReverseIterator src_end) {
        EmplacesP(dst, src_begin, src_end);
    }

    /*
        Replace the objects that starts at the given place with the other objects given.
    */
    FORCEINLINE ReverseIterator Emplaces(ReverseIterator dst, Iterator src_begin, Iterator src_end) {
        EmplacesP(dst, src_begin, src_end);
    }
    /*
        Replace the objects that starts at the given place with the other objects given.
    */
    FORCEINLINE Void Emplaces(ReverseIterator dst, ConstIterator src_begin, ConstIterator src_end) {
        EmplacesP(dst, src_begin, src_end);
    }
    /*
        Replace the objects that starts at the given place with the other objects given.
    */
    FORCEINLINE Void Emplaces(ReverseIterator dst, ReverseIterator src_begin, ReverseIterator src_end) {
        EmplacesP(dst, src_begin, src_end);
    }
    /*
        Replace the objects that starts at the given place with the other objects given.
    */
    FORCEINLINE Void Emplaces(ReverseIterator dst, 
                                        ConstReverseIterator src_begin, ConstReverseIterator src_end) {
        EmplacesP(dst, src_begin, src_end);
    }


    /*
        Construct the deque by filling it with the given amount of objects.
        The object is constructed by the arguements.
        If kIfUnique is false and no arguements, will only add the 
        size of the deque.
    */
    template<typename... ArgsType>
    Void Assign(IndexType num, ArgsType&&... args) noexcept;
    /*
        Construct the deque by filling it objects between the iterators.
    */
    FORCEINLINE Void Assign(Iterator src_begin, Iterator src_end) noexcept { AssignP(src_begin, src_end); }
    /*
        Construct the deque by filling it objects between the iterators.
    */
    FORCEINLINE Void Assign(ConstIterator src_begin, ConstIterator src_end) noexcept { AssignP(src_begin, src_end); }
    /*
        Construct the deque by filling it objects between the iterators.
    */
    FORCEINLINE Void Assign(ReverseIterator src_begin, ReverseIterator src_end) noexcept { 
        AssignP(src_begin, src_end); 
    }
    /*
        Construct the deque by filling it objects between the iterators.
    */
    FORCEINLINE Void Assign(ConstReverseIterator src_begin, ConstReverseIterator src_end) noexcept {
        AssignP(src_begin, src_end);
    }

    /*
        The small object on the front.
    */
    Void Sort() noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(IndexType begin, IndexType end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(IndexType begin, IndexType end, CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(Iterator begin, Iterator& end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(Iterator begin, Iterator end, CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(ReverseIterator begin, ReverseIterator end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(ReverseIterator begin, ReverseIterator end, CompareFunction&& compare_function) noexcept;

    /*
        Destroys all the objects in the deque, does not release the memory.
    */
    Void Clear() noexcept;
    /*
        Destroys all the objects in the deque, release the memory.
    */
    FORCEINLINE Void Destroy();

protected:
    using SuperType = ZObject;

private:

    static constexpr Bool kCreateObjects = true;
    static constexpr Bool kDestroyObjects = false;
    static constexpr Bool kCreateAndCopyObjects = true;
    static constexpr Bool kCopyObjects = false;
    static constexpr Bool kCreateAndCopyObjectsReverse = true;
    static constexpr Bool kCopyObjectsReverse = false;

    /*
        Creates an object at the certain place. Will call the Constrctor if needed.
    */
    template<typename DstIteratorType, typename... ArgsType>
    requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
    FORCEINLINE static Void CreateObjectP(DstIteratorType dst, ArgsType&&... args);

    /*
        Destroys an object at the certain place. Will Call Destrctor if needed.
    */
    template<typename DstIteratorType>
    requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
    FORCEINLINE static Void DestroyObjectP(DstIteratorType dst);

    /*
        The base function of create and destroy objests.
    */
    template<Bool kIfCreate, typename DstIteratorType, typename... ArgsType>
    requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
    FORCEINLINE static Void CreateDestroyObjectsBaseP(DstIteratorType dst_begin, DstIteratorType dst_end, 
                                                      ArgsType&&... args);

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the Constrctor if needed.
    */
    template<typename DstIteratorType, typename... ArgsType>
    requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
    inline static Void CreateObjectsP(DstIteratorType begin, DstIteratorType end, ArgsType&&... args) noexcept {
        CreateDestroyObjectsBaseP<kCreateObjects>(begin, end, std::forward<ArgsType>(args)...);
    }

    /*
        Destroy the objects by the given arguements([begin, end)).
        Will call the destrctor if this object class's member kIfUnique is true.
    */
    template<typename DstIteratorType>
    requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
    inline static Void DestroyObjectsP(DstIteratorType begin, DstIteratorType end) noexcept {
        CreateDestroyObjectsBaseP<kDestroyObjects>(begin, end);
    }

    /*
        The base function of copy objests.
    */
    template<Bool kIfCreate, typename DstIteratorType, typename SrcIteratorType>
    requires (internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType> &&
              internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>)
    FORCEINLINE static Void CopyObjectsBaseP(DstIteratorType dst, SrcIteratorType src_begin, SrcIteratorType src_end);

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the copy constructor.
    */
    template<typename DstIteratorType, typename SrcIteratorType>
    requires (internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType> &&
              internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>)
    inline static Void CreateAndCopyObjectsP(DstIteratorType dst, SrcIteratorType src_begin, 
                                             SrcIteratorType src_end) noexcept {
        CopyObjectsBaseP<kCreateAndCopyObjects>(dst, src_begin, src_end);
    }

    /*
        Copy objects by the given iterator. Will call the copy assignment operator
        if this object class's member kIfUnique is true.
    */
    template<typename DstIteratorType, typename SrcIteratorType>
    requires (internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType> &&
              internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>)
    inline static Void CopyObjectsP(DstIteratorType dst, SrcIteratorType src_begin,
                                    SrcIteratorType src_end) noexcept {
        CopyObjectsBaseP<kCopyObjects>(dst, src_begin, src_end);
    }

    /*
        Creates the capacity by the given capacity, the final capacity might
        not equal the given capacity.
    */
    Void CreateContainerP(IndexType capacity) noexcept;
    /*
        Extends the capacity by the given capacity, the final capacity might
        not equal the given capacity.
    */
    Void ExtendContainerP(IndexType capacity) noexcept;
    /*
        Shrinks the container to the minimum capacity that can fit the current size.
    */
    Void ShrinkContainerP() noexcept;
    /*
        Destroys the container.
    */
    Void DestroyContainerP() noexcept;

    /*
        Container copy function.
    */
    Void CopyP(const ZSimpleDeque& deque) noexcept;

    /*
        Container move function.
    */
    FORCEINLINE Void MoveP(ZSimpleDeque&& deque);

    /*
        Constructor with two order iterators.
    */
    template<typename SrcIteratorType>
    requires internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>
    FORCEINLINE Void ZSimpleDequeP(SrcIteratorType src_begin, SrcIteratorType src_end);

    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the deque.
    */
    template<typename SrcIteratorType>
    requires internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>
    Void PushBacksP(SrcIteratorType src_begin, SrcIteratorType src_end) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the newest object.
    */
    template<typename DstIteratorType, typename... ArgsType>
    requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
    NODISCARD DstIteratorType InsertP(DstIteratorType dst, ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the first new object.
    */
    template<typename DstIteratorType, typename... ArgsType>
    requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
    NODISCARD DstIteratorType InsertsP(DstIteratorType dst, IndexType num, ArgsType&&... args) noexcept;

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    template<typename DstIteratorType, typename SrcIteratorType>
    requires (internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType> &&
              internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>)
    NODISCARD DstIteratorType InsertsP(DstIteratorType dst, SrcIteratorType src_begin, SrcIteratorType src_end) noexcept;

    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    template<typename DstIteratorType, typename... ArgsType>
    requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
    NODISCARD inline DstIteratorType EraseP(DstIteratorType dst) noexcept;

    /*
        Erases the num of objects that starts at the given index.
        Returns the iterator that points at the next object.
    */
    template<typename DstIteratorType, typename... ArgsType>
    requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
    NODISCARD inline DstIteratorType ErasesP(DstIteratorType dst_begin, DstIteratorType dst_end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename DstIteratorType, typename... ArgsType>
    requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
    inline Void EmplaceP(DstIteratorType dst, ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the first new object.
    */
    template<typename DstIteratorType, typename... ArgsType>
    requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
    inline Void EmplacesP(DstIteratorType dst, IndexType num, ArgsType&&... args) noexcept;

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    template<typename DstIteratorType, typename SrcIteratorType>
    requires (internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType> &&
              internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>)
    inline Void EmplacesP(DstIteratorType dst, SrcIteratorType src_begin, SrcIteratorType src_end) noexcept;


    /*
        Construct the deque by filling it objects between the iterators.
    */
    template<typename SrcIteratorType>
    requires internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>
    Void AssignP(SrcIteratorType src_begin, SrcIteratorType src_end) noexcept;

    ObjectType* data_ptr_;
    IndexType capacity_;

    IndexType begin_index_;
    IndexType end_index_;
};

template<typename ObjectType, Bool kIfUnique>
ZSimpleDeque<ObjectType, kIfUnique>::ZSimpleDeque() noexcept
    : SuperType()
    , data_ptr_(nullptr)
    , capacity_(0)
    , size_(0)
{}

template<typename ObjectType, Bool kIfUnique>
ZSimpleDeque<ObjectType, kIfUnique>::ZSimpleDeque(const ZSimpleDeque& deque) noexcept
    : SuperType(deque)
{
    DEBUG(&deque == this, "The source and the target of the copy is the same!");
    CreateContainerP(deque.size_);
    CreateAndCopyObjectsP(Iterator(data_ptr_), Iterator(deque.data_ptr_), Iterator(deque.data_ptr_ + deque.size_));
    size_ = deque.size_;
}

template<typename ObjectType, Bool kIfUnique>
ZSimpleDeque<ObjectType, kIfUnique>::ZSimpleDeque(ZSimpleDeque&& deque) noexcept
    : SuperType(std::forward<ZSimpleDeque>(deque))
{
    DEBUG(&deque == this, "The source and the target of the copy is the same!");
    MoveP(std::forward<ZSimpleDeque>(deque));
}

template<typename ObjectType, Bool kIfUnique>
ZSimpleDeque<ObjectType, kIfUnique>::ZSimpleDeque(IndexType capacity) noexcept
    : SuperType()
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainerP(capacity);
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
ZSimpleDeque<ObjectType, kIfUnique>::ZSimpleDeque(IndexType capacity, ArgsType&&... args) noexcept
    : SuperType()
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainerP(capacity);
    CreateObjectsP(Iterator(data_ptr_), Iterator(data_ptr_ + capacity), std::forward<ArgsType>(args)...);
    size_ = capacity;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE ZSimpleDeque<ObjectType, kIfUnique>& ZSimpleDeque<ObjectType, kIfUnique>::operator=(const ZSimpleDeque& deque) {
    DEBUG(&deque == this, "The source and the target of the copy is the same!");
    SuperType::operator=(deque);
    CopyP(deque);
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
inline ZSimpleDeque<ObjectType, kIfUnique>& ZSimpleDeque<ObjectType, kIfUnique>::operator=(ZSimpleDeque&& deque) noexcept {
    DEBUG(&deque == this, "The source and the target of the copy is the same!");
    SuperType::operator=(std::forward<ZSimpleDeque>(deque));
    DestroyObjectsP(Iterator(data_ptr_), Iterator(data_ptr_ + size_));
    MoveP(std::forward<ZSimpleDeque>(deque));
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZSimpleDeque<ObjectType, kIfUnique>::~ZSimpleDeque() noexcept {
    DestroyContainerP();
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
inline Void ZSimpleDeque<ObjectType, kIfUnique>::Resize(IndexType size, ArgsType&&... args) noexcept {
    DEBUG(size < 0, "Negaive size is not valid!");
    if (size_ < size) {
        if (size > capacity_) {
            ExtendContainerP(size);
        }
        CreateObjectsP(Iterator(data_ptr_ + size_), Iterator(data_ptr_ + size), std::forward<ArgsType>(args)...);
    }
    else {
        DestroyObjectsP(Iterator(data_ptr_ + size), Iterator(data_ptr_ + size_));
    }
    size_ = size;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZSimpleDeque<ObjectType, kIfUnique>::Reserve(IndexType capacity) {
    if (capacity > capacity_) {
        ExtendContainerP(capacity);
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZSimpleDeque<ObjectType, kIfUnique>::ShrinkToFit() {
    ShrinkContainerP();
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZSimpleDeque<ObjectType, kIfUnique>::PopBack() {               
    DEBUG(size_ == 0, "No existing object to pop!");
    DestroyObjectP(Iterator(data_ptr_ + --size_));
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZSimpleDeque<ObjectType, kIfUnique>::PopBack(ObjectType* object_ptr) {
    DEBUG(size_ == 0, "No existing object to pop!");
    *object_ptr = std::move(data_ptr_[--size_]);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZSimpleDeque<ObjectType, kIfUnique>::PushBack(ArgsType&&... args) {
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjectP(Iterator(data_ptr_ + size_), std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
inline Void ZSimpleDeque<ObjectType, kIfUnique>::PushBacks(IndexType num, ArgsType&&... args) noexcept {
    DEBUG(num < 0, "Negative pushing num not valid!");
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjectsP(Iterator(data_ptr_ + size_), Iterator(data_ptr_ + new_size), std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
inline Void ZSimpleDeque<ObjectType, kIfUnique>::EmplaceBack(ArgsType&&... args) noexcept {
    DEBUG(size_ == 0, "No existing object to emplace!");
    Iterator dst(data_ptr_ + (size_ - 1));
    DestroyObjectP(dst);
    CreateObjectP(dst, std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZSimpleDeque<ObjectType, kIfUnique>::Assign(IndexType num, ArgsType&&... args) noexcept {
    DEBUG(num < 0, "Negative assign num not valid!");
    IndexType new_size = num;
    if (new_size > capacity_) {
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    DestroyObjectsP(Iterator(data_ptr_), Iterator(data_ptr_ + size_));
    CreateObjectsP(Iterator(data_ptr_), Iterator(data_ptr_ + num), std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZSimpleDeque<ObjectType, kIfUnique>::Clear() noexcept {
    DestroyObjectsP(Iterator(data_ptr_), Iterator(data_ptr_ + size_));
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZSimpleDeque<ObjectType, kIfUnique>::Destroy() {
    DestroyContainerP();
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename... ArgsType>
requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
FORCEINLINE Void ZSimpleDeque<ObjectType, kIfUnique>::CreateObjectP(DstIteratorType dst, ArgsType&&... args) {
    if constexpr (sizeof...(args) == 0) {
        if constexpr (kIfUnique) {
            new(reinterpret_cast<Void*>(dst.object_ptr())) ObjectType();
        }
    }
    else {
        new(reinterpret_cast<Void*>(dst.object_ptr())) ObjectType(std::forward<ArgsType>(args)...);
    }
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType>
requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
FORCEINLINE Void ZSimpleDeque<ObjectType, kIfUnique>::DestroyObjectP(DstIteratorType dst) {
    if constexpr (kIfUnique) {
        dst->~ObjectType();
    }
}

template<typename ObjectType, Bool kIfUnique>
template<Bool kIfCreate, typename DstIteratorType, typename... ArgsType>
requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
FORCEINLINE static Void ZSimpleDeque<ObjectType, kIfUnique>::CreateDestroyObjectsBaseP(DstIteratorType dst_begin, 
                                                                                  DstIteratorType dst_end,
                                                                                  ArgsType&&... args) {
    if constexpr (sizeof...(args) != 0 || kIfUnique) {
        for (; dst_begin < dst_end; ++dst_begin) {
            if constexpr (kIfCreate) {
                new(reinterpret_cast<Void*>(dst_begin.object_ptr())) ObjectType(std::forward<ArgsType>(args)...);
            }
            else {
                dst_begin->~ObjectType();
            }
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
template<Bool kIfCreate, typename DstIteratorType, typename SrcIteratorType>
requires (internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType> &&
          internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>)
FORCEINLINE Void ZSimpleDeque<ObjectType, kIfUnique>::CopyObjectsBaseP(DstIteratorType dst, 
                                                                  SrcIteratorType src_begin, 
                                                                  SrcIteratorType src_end) {
    if constexpr (kIfUnique || (internal::kIsOrderSimpleDequeIterator<DstIteratorType, ObjectType> != 
                                internal::kIsOrderSimpleDequeIterator<SrcIteratorType, ObjectType>)) {
        for(; src_begin < src_end; ++dst, ++src_begin) {
            if constexpr (kIfCreate) {
                new(reinterpret_cast<Void*>(dst.object_ptr())) ObjectType(*src_begin);
            }
            else {
                *dst = *src_begin;
            }
        }
    }
    else {
        if constexpr (internal::kIsOrderSimpleDequeIterator<DstIteratorType, ObjectType>) {
            memcpy(reinterpret_cast<Void*>(dst.object_ptr()), 
                   reinterpret_cast<Void*>(const_cast<ObjectType*>(src_begin.object_ptr())),
                   static_cast<SizeType>(src_end - src_begin) * sizeof(ObjectType));
        }
        else {
            IndexType length = src_end - src_begin;
            memcpy(reinterpret_cast<Void*>(dst.object_ptr() - (length - 1)), 
                   reinterpret_cast<Void*>(const_cast<ObjectType*>(src_end.object_ptr() + 1)),
                   length * sizeof(ObjectType));
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
Void ZSimpleDeque<ObjectType, kIfUnique>::CreateContainerP(IndexType capacity) noexcept {
    MemoryType need_memory_size = capacity * sizeof(ObjectType);
    MemoryType apply_mrmory_size;
    data_ptr_ = reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));
    capacity_ = apply_mrmory_size / sizeof(ObjectType);
}

template<typename ObjectType, Bool kIfUnique>
Void ZSimpleDeque<ObjectType, kIfUnique>::ExtendContainerP(IndexType capacity) noexcept {
    MemoryType current_memory_size = capacity_ * sizeof(ObjectType);
    MemoryType need_memory_size = capacity * sizeof(ObjectType);
    MemoryType apply_mrmory_size;
    //If the current memory piece doesn't have enough memory.
    if (!memory_pool::CheckMemory(reinterpret_cast<Void*>(data_ptr_), need_memory_size, &apply_mrmory_size)) {
        ObjectType* temp_data_ptr = 
            reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));
        memcpy(reinterpret_cast<Void*>(temp_data_ptr), reinterpret_cast<Void*>(data_ptr_),
               size_ * sizeof(ObjectType));
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(data_ptr_));
        data_ptr_ = temp_data_ptr;
    }
    capacity_ = apply_mrmory_size / sizeof(ObjectType);  
}

template<typename ObjectType, Bool kIfUnique>
Void ZSimpleDeque<ObjectType, kIfUnique>::ShrinkContainerP() noexcept {
    MemoryType need_memory_size = size_ * sizeof(ObjectType);
    MemoryType min_satisfied_memory_size = memory_pool::CalculateMemory(need_memory_size);
    MemoryType min_satisfied_capacity = min_satisfied_memory_size / sizeof(ObjectType);
    if (capacity_ != min_satisfied_capacity) {
        ObjectType* temp_data_ptr = reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory(min_satisfied_memory_size));
        memcpy(reinterpret_cast<Void*>(temp_data_ptr), reinterpret_cast<Void*>(data_ptr_),
               size_ * sizeof(ObjectType));
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(data_ptr_));
        data_ptr_ = temp_data_ptr;
        capacity_ = min_satisfied_capacity;
    }
}

template<typename ObjectType, Bool kIfUnique>
Void ZSimpleDeque<ObjectType, kIfUnique>::DestroyContainerP() noexcept {
    DestroyObjectsP(Iterator(data_ptr_), Iterator(data_ptr_ + size_));
    memory_pool::ReleaseMemory(reinterpret_cast<Void*>(data_ptr_));
    data_ptr_ = nullptr;
    capacity_ = 0;
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
Void ZSimpleDeque<ObjectType, kIfUnique>::CopyP(const ZSimpleDeque& deque) noexcept {
    if (deque.size_ > capacity_) {
        ExtendContainerP(deque.size_);
    }
    if constexpr (kIfUnique) {
        if (deque.size_ > size_) {
            CopyObjectsP(Iterator(data_ptr_), Iterator(deque.data_ptr_), Iterator(deque.data_ptr_ + size_));
            CreateAndCopyObjectsP(Iterator(data_ptr_ + size_), 
                                  Iterator(deque.data_ptr_ + size_), 
                                  Iterator(deque.data_ptr_ + deque.size_));
        }
        else {
            CopyObjectsP(Iterator(data_ptr_), Iterator(deque.data_ptr_), Iterator(deque.data_ptr_ + deque.size_));
            DestroyObjectsP(Iterator(data_ptr_ + deque.size_), Iterator(data_ptr_ + size_));
        }
    }
    else {
        CopyObjectsP(Iterator(data_ptr_), Iterator(deque.data_ptr_), Iterator(deque.data_ptr_ + deque.size_));
    }
    size_ = deque.size_;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZSimpleDeque<ObjectType, kIfUnique>::MoveP(ZSimpleDeque&& deque) {
    data_ptr_ = deque.data_ptr_;
    capacity_ = deque.capacity_;
    size_ = deque.size_;
    deque.data_ptr_ = nullptr;
    deque.capacity_ = 0;
    deque.size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename SrcIteratorType>
requires internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>
FORCEINLINE Void ZSimpleDeque<ObjectType, kIfUnique>::ZSimpleDequeP(SrcIteratorType src_begin, SrcIteratorType src_end) {
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    size_ = src_end - src_begin;
    CreateContainerP(size_);
    CreateAndCopyObjectsP(Iterator(data_ptr_), src_begin, src_end);
}

template<typename ObjectType, Bool kIfUnique>
template<typename SrcIteratorType>
requires internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>
Void ZSimpleDeque<ObjectType, kIfUnique>::PushBacksP(SrcIteratorType src_begin, SrcIteratorType src_end) noexcept {
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType new_size = size_ + (src_end - src_begin);
    if (new_size > capacity_) {
        if (src_begin.object_ptr() >= data_ptr_ && src_begin.object_ptr() <= (data_ptr_ + size_)) {
            IndexType begin_index = static_cast<IndexType>(src_begin.object_ptr() - data_ptr_);
            IndexType end_index = static_cast<IndexType>(src_end.object_ptr() - data_ptr_);
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
            src_begin = data_ptr_ + begin_index;
            src_end = data_ptr_ + end_index;
        }
        else {
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
        }
    }
    CreateAndCopyObjectsP(Iterator(data_ptr_ + size_), src_begin, src_end);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename... ArgsType>
requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
NODISCARD DstIteratorType ZSimpleDeque<ObjectType, kIfUnique>::InsertP(DstIteratorType dst, ArgsType&&... args) noexcept {
    if constexpr (internal::kIsReverseSimpleDequeIterator<DstIteratorType, ObjectType>) {
        --dst;
    }
    DEBUG(dst.object_ptr() < data_ptr_ || dst.object_ptr() > data_ptr_ + size_, "Insert place out of bounds!");
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        IndexType index = static_cast<IndexType>(dst.object_ptr() - data_ptr_);
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
        dst = data_ptr_ + index;

    }
    memmove(reinterpret_cast<Void*>(dst.object_ptr() + 1), reinterpret_cast<Void*>(dst.object_ptr()),
            (size_ - static_cast<SizeType>(dst.object_ptr() - data_ptr_)) * sizeof(ObjectType));
    CreateObjectP(dst, std::forward<ArgsType>(args)...);
    size_ = new_size;
    return dst;
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename... ArgsType>
requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
NODISCARD DstIteratorType ZSimpleDeque<ObjectType, kIfUnique>::InsertsP(DstIteratorType dst, IndexType num,
                                                                   ArgsType&&... args) noexcept {
    if constexpr (internal::kIsReverseSimpleDequeIterator<DstIteratorType, ObjectType>) {
        --dst;
    }
    DEBUG(dst.object_ptr() < data_ptr_ || dst.object_ptr() > data_ptr_ + size_, "Insert place out of bounds!");
    DEBUG(num < 0, "Negative insert num not valid!");
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        IndexType index = static_cast<IndexType>(dst.object_ptr() - data_ptr_);
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
        dst = data_ptr_ + index;
    }
    memmove(reinterpret_cast<Void*>(dst.object_ptr() + num), reinterpret_cast<Void*>(dst.object_ptr()),
            (size_ - static_cast<SizeType>(dst.object_ptr() - data_ptr_)) * sizeof(ObjectType));
    if constexpr (internal::kIsReverseSimpleDequeIterator<DstIteratorType, ObjectType>) {
        dst -= num - 1;
    }
    CreateObjectsP(dst, dst + num, std::forward<ArgsType>(args)...);
    size_ = new_size;
    return dst;
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename SrcIteratorType>
requires (internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>&&
          internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>)
NODISCARD DstIteratorType ZSimpleDeque<ObjectType, kIfUnique>::InsertsP(DstIteratorType dst,
                                                                   SrcIteratorType src_begin, 
                                                                   SrcIteratorType src_end) noexcept {
    if constexpr (internal::kIsReverseSimpleDequeIterator<DstIteratorType, ObjectType>) {
        --dst;
    }
    DEBUG(dst.object_ptr() < data_ptr_ || dst.object_ptr() > data_ptr_ + size_, "Insert place out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    if (src_begin.object_ptr() >= data_ptr_ && src_begin.object_ptr() <= (data_ptr_ + size_)) {
        IndexType begin_index = static_cast<IndexType>(src_begin.object_ptr() - data_ptr_);
        IndexType end_index = static_cast<IndexType>(src_end.object_ptr() - data_ptr_);
        IndexType index = static_cast<IndexType>(dst.object_ptr() - data_ptr_);
        if (new_size > capacity_) {
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
            src_begin = data_ptr_ + begin_index;
            src_end = data_ptr_ + end_index;
            dst = data_ptr_ + index;
        }
        memmove(reinterpret_cast<Void*>(dst.object_ptr() + num), reinterpret_cast<Void*>(dst.object_ptr()),
                (size_ - static_cast<SizeType>(dst.object_ptr() - data_ptr_)) * sizeof(ObjectType));
        if constexpr (internal::kIsReverseSimpleDequeIterator<DstIteratorType, ObjectType>) {
            dst -= num - 1;
        }
        if constexpr (internal::kIsOrderSimpleDequeIterator<SrcIteratorType, ObjectType>) {
            if (index >= end_index) {
                CreateAndCopyObjectsP(dst, src_begin, src_end);
            }
            else if (index < begin_index) {
                CreateAndCopyObjectsP(dst, src_begin + num, src_end + num);
            }
            else {
                IndexType part_1_num = index - begin_index;
                IndexType part_2_num = num - part_1_num;
                SrcIteratorType temp_src = src_begin + part_1_num;
                CreateAndCopyObjectsP(dst, src_begin, temp_src);
                CreateAndCopyObjectsP(dst + part_1_num, temp_src + num, src_end + num);
            }
        }
        else {
            if (index > begin_index) {
                CreateAndCopyObjectsP(dst, src_begin, src_end);
            }
            else if (index <= end_index) {
                CreateAndCopyObjectsP(dst, src_begin - num, src_end - num);
            }
            else {
                IndexType part_1_num = begin_index - index + 1;
                IndexType part_2_num = num - part_1_num;
                SrcIteratorType temp_src = src_begin + part_1_num;
                CreateAndCopyObjectsP(dst, src_begin - num, temp_src - num);
                CreateAndCopyObjectsP(dst + part_1_num, temp_src, src_end);
            }
        }
    }
    else {
        if (new_size > capacity_) {
            IndexType index = static_cast<IndexType>(dst.object_ptr() - data_ptr_);
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
            dst = data_ptr_ + index;
        }
        memmove(reinterpret_cast<Void*>(dst.object_ptr() + num), reinterpret_cast<Void*>(dst.object_ptr()),
                (size_ - static_cast<SizeType>(dst.object_ptr() - data_ptr_)) * sizeof(ObjectType));
        if constexpr (internal::kIsReverseSimpleDequeIterator<DstIteratorType, ObjectType>) {
            dst -= num - 1;
        }
        CreateAndCopyObjectsP(dst, src_begin, src_end);
    }
    size_ = new_size;
    return dst;
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename... ArgsType>
requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
NODISCARD inline DstIteratorType ZSimpleDeque<ObjectType, kIfUnique>::EraseP(DstIteratorType dst) noexcept {
    DEBUG(dst.object_ptr() < data_ptr_ || dst.object_ptr() >= data_ptr_ + size_, "Erase place out of bounds!");
    DestroyObjectP(dst);
    memmove(reinterpret_cast<Void*>(dst.object_ptr()), reinterpret_cast<Void*>(dst.object_ptr() + 1),
            (size_ - 1 - static_cast<SizeType>(dst.object_ptr() - data_ptr_)) * sizeof(ObjectType));
    --size_;
    if constexpr (internal::kIsOrderSimpleDequeIterator<DstIteratorType, ObjectType>) {
        --dst;
    }
    return dst;
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename... ArgsType>
requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
NODISCARD inline DstIteratorType ZSimpleDeque<ObjectType, kIfUnique>::ErasesP(DstIteratorType dst_begin, 
                                                                         DstIteratorType dst_end) noexcept {
    DEBUG(dst_begin.object_ptr() < data_ptr_ || dst_begin.object_ptr() >= data_ptr_ + size_, 
          "Erase index out of bounds!");
    DEBUG(dst_end.object_ptr() < data_ptr_ || dst_end.object_ptr() > data_ptr_ + size_, "Erase index out of bounds!");
    DEBUG(dst_begin > dst_end, "Begin iterator after end iterator!");
    DestroyObjectsP(dst_begin, dst_end);
    if constexpr (internal::kIsOrderSimpleDequeIterator<DstIteratorType, ObjectType>) {
        memmove(reinterpret_cast<Void*>(dst_begin.object_ptr()), reinterpret_cast<Void*>(dst_end.object_ptr()),
                (size_ - static_cast<SizeType>(dst_end.object_ptr() - data_ptr_)) * sizeof(ObjectType));
    }
    else {
        memmove(reinterpret_cast<Void*>(dst_end.object_ptr() + 1), reinterpret_cast<Void*>(dst_begin.object_ptr() + 1),
                (size_ - static_cast<SizeType>(dst_end.object_ptr() - data_ptr_)) * sizeof(ObjectType));
    }

    size_ -= dst_end - dst_begin;
    if constexpr (internal::kIsOrderSimpleDequeIterator<DstIteratorType, ObjectType>) {
        return dst_begin - 1;
    }
    else {
        return dst_end - 1;
    }
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename... ArgsType>
requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
inline Void ZSimpleDeque<ObjectType, kIfUnique>::EmplaceP(DstIteratorType dst, ArgsType&&... args) noexcept {
    DEBUG(dst.object_ptr() < data_ptr_ || dst.object_ptr() >= data_ptr_ + size_, "Emplace place out of bounds!");
    DestroyObjectP(dst);
    CreateObjectP(dst, std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename... ArgsType>
requires internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>
inline Void ZSimpleDeque<ObjectType, kIfUnique>::EmplacesP(DstIteratorType dst, IndexType num, ArgsType&&... args) noexcept {
    DEBUG(dst.object_ptr() < data_ptr_ || dst.object_ptr() >= data_ptr_ + size_, "Emplace place out of bounds!");
    DEBUG((dst + num - 1).object_ptr() < data_ptr_ || (dst + num - 1).object_ptr() >= data_ptr_ + size_,
          "Emplace place out of bounds!");
    DEBUG(num < 0, "Negative insert num not valid!");
    DestroyObjectsP(dst, dst + num);
    CreateObjectsP(dst, dst + num, std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename SrcIteratorType>
requires (internal::kIsNonConstSimpleDequeIterator<DstIteratorType, ObjectType>&&
          internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>)
inline Void ZSimpleDeque<ObjectType, kIfUnique>::EmplacesP(DstIteratorType dst, 
                                                      SrcIteratorType src_begin, 
                                                      SrcIteratorType src_end) noexcept {
    DEBUG(dst.object_ptr() < data_ptr_ || dst.object_ptr() >= data_ptr_ + size_, "Emplace place out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    CopyObjectsP(dst, src_begin, src_end);
}

template<typename ObjectType, Bool kIfUnique>
template<typename SrcIteratorType>
requires internal::kIsSimpleDequeIterator<SrcIteratorType, ObjectType>
Void ZSimpleDeque<ObjectType, kIfUnique>::AssignP(SrcIteratorType src_begin, SrcIteratorType src_end) noexcept {
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType new_size = src_end - src_begin;
    if (new_size > capacity_) {
        ExtendContainerP(new_size);
    }
    //If the iterator is from this.
    if (src_begin.object_ptr() >= data_ptr_ && src_begin.object_ptr() < (data_ptr_ + size_)) {
        if constexpr (internal::kIsOrderSimpleDequeIterator<SrcIteratorType, ObjectType>) {
            DestroyObjectsP(Iterator(data_ptr_), 
                            Iterator(const_cast<ObjectType*>(src_begin.object_ptr())));
            DestroyObjectsP(Iterator(const_cast<ObjectType*>(src_end.object_ptr())),
                            Iterator(data_ptr_ + size_));
            //Move the objects to the front.
            memmove(reinterpret_cast<Void*>(data_ptr_),
                    reinterpret_cast<Void*>(const_cast<ObjectType*>(src_begin.object_ptr())),
                    new_size * sizeof(ObjectType));
        }
        else {
            DestroyObjectsP(Iterator(data_ptr_ + (size_ - 1)), 
                            Iterator(const_cast<ObjectType*>(src_begin.object_ptr())));
            DestroyObjectsP(Iterator(const_cast<ObjectType*>(src_end.object_ptr())),
                            Iterator(data_ptr_ - 1));
            //Move the objects to the front.
            memmove(reinterpret_cast<Void*>(data_ptr_),
                    reinterpret_cast<Void*>(const_cast<ObjectType*>(src_end.object_ptr() + 1)),
                    new_size * sizeof(ObjectType));
            //Reverses the objects.
            ObjectType* begin_ptr = data_ptr_;
            ObjectType* end_ptr = begin_ptr + (new_size - 1);
            for (; begin_ptr < end_ptr; ++begin_ptr, --end_ptr) {
                Swap(begin_ptr, end_ptr);
            }
        }
    }
    else {
        if (new_size > size_) {
            CopyObjectsP(Iterator(data_ptr_), src_begin, src_begin + size_);
            CreateAndCopyObjectsP(Iterator(data_ptr_ + size_), src_begin + size_, src_end);
        }
        else {
            Iterator temp(data_ptr_);
            CopyObjectsP(Iterator(data_ptr_), src_begin, src_end);
            DestroyObjectsP(Iterator(data_ptr_ + new_size), Iterator(data_ptr_ + size_));
        }
    }
    size_ = new_size;
}

}//zengine

#endif // !Z_CORE_Z_DEQUE_H_