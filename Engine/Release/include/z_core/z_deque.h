#ifndef Z_CORE_Z_DEQUE_H_
#define Z_CORE_Z_DEQUE_H_

#include "internal/drive.h"

#include "m_error_message.h"
#include "z_object.h"

namespace zengine {

template<typename ObjectType, Bool kIfUnique = kIsClass<ObjectType>>
class ZDeque;

namespace internal {

template<typename ObjectType>
class DequeIteratorBase {
public:
    FORCEINLINE DequeIteratorBase(ObjectType* object_ptr) : object_ptr_(object_ptr) {}
    FORCEINLINE DequeIteratorBase(const DequeIteratorBase& iterator) : object_ptr_(iterator.object_ptr_) {}
    FORCEINLINE DequeIteratorBase(const DequeIteratorBase&& iterator) : object_ptr_(iterator.object_ptr_) {
        iterator.MoveDestroy();
    }

    FORCEINLINE DequeIteratorBase& operator=(ObjectType* object_ptr) {
        object_ptr_ = object_ptr;
        return *this;
    }
    FORCEINLINE DequeIteratorBase& operator=(const DequeIteratorBase& iterator) {
        object_ptr_ = iterator.object_ptr_;
        return *this;
    }
    FORCEINLINE DequeIteratorBase& operator=(DequeIteratorBase&& iterator) {
        object_ptr_ = iterator.object_ptr_;
        iterator.MoveDestroy();
        return *this;
    }

    NODISCARD FORCEINLINE const Bool operator==(const DequeIteratorBase& iterator) const {
        return object_ptr_ == iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator!=(const DequeIteratorBase& iterator) const {
        return object_ptr_ != iterator.object_ptr_;
    }

    FORCEINLINE ~DequeIteratorBase() {}

    FORCEINLINE Void MoveDestroy() { object_ptr_ = nullptr; }

protected:
     ObjectType* object_ptr_;
     ZDeque<ObjectType>* deque_ptr_;
};

template<typename ObjectType>
class DequeIterator : public DequeIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index) const { return SuperType::object_ptr_[index]; }
    NODISCARD FORCEINLINE ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE DequeIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE DequeIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }

    FORCEINLINE DequeIterator& operator++() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE DequeIterator& operator--() {
        --SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE DequeIterator operator+(const IndexType data_num) const {
        return DequeIterator(SuperType::object_ptr_ + data_num);
    }
    NODISCARD FORCEINLINE DequeIterator operator-(const IndexType data_num) const {
        return DequeIterator(SuperType::object_ptr_ - data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const DequeIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const DequeIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const DequeIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const DequeIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const DequeIterator& iterator) const {
        return static_cast<IndexType>(SuperType::object_ptr_ - iterator.SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = DequeIteratorBase<ObjectType>;
};

template<typename ObjectType>
class DequeConstIterator : public DequeIteratorBase<ObjectType> {
public:
    FORCEINLINE DequeConstIterator(const ObjectType* object_ptr) : SuperType(const_cast<ObjectType*>(object_ptr)) {}

    FORCEINLINE DequeConstIterator& operator=(const ObjectType* object_ptr) {
        SuperType::object_ptr_ = const_cast<ObjectType*>(object_ptr);
        return *this;
    }

    NODISCARD FORCEINLINE const ObjectType& operator[](const IndexType index) const { 
        return SuperType::object_ptr_[-index]; 
    }
    NODISCARD FORCEINLINE const ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE DequeConstIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE DequeConstIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }

    FORCEINLINE DequeConstIterator& operator++() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE DequeConstIterator& operator--() {
        --SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE DequeConstIterator operator+(const IndexType data_num) const {
        return DequeConstIterator(SuperType::object_ptr_ + data_num);
    }
    NODISCARD FORCEINLINE DequeConstIterator operator-(const IndexType data_num) const {
        return DequeConstIterator(SuperType::object_ptr_ - data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const DequeConstIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const DequeConstIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const DequeConstIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const DequeConstIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const DequeConstIterator& iterator) const {
        return static_cast<IndexType>(SuperType::object_ptr_ - iterator.SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE const ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = DequeIteratorBase<ObjectType>;
};

template<typename ObjectType>
class DequeReverseIterator : public DequeIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index) const { return SuperType::object_ptr_[-index]; }
    NODISCARD FORCEINLINE ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE DequeReverseIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE DequeReverseIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }

    FORCEINLINE DequeReverseIterator& operator++() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE DequeReverseIterator& operator--() {
        ++SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE DequeReverseIterator operator+(const IndexType data_num) const {
        return DequeReverseIterator(SuperType::object_ptr_ - data_num);
    }
    NODISCARD FORCEINLINE DequeReverseIterator operator-(const IndexType data_num) const {
        return DequeReverseIterator(SuperType::object_ptr_ + data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const DequeReverseIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const DequeReverseIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const DequeReverseIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const DequeReverseIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const DequeReverseIterator& iterator) const {
        return static_cast<IndexType>(iterator.SuperType::object_ptr_ - SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = DequeIteratorBase<ObjectType>;
};

template<typename ObjectType>
class DequeConstReverseIterator : public DequeIteratorBase<ObjectType> {
public:
    FORCEINLINE DequeConstReverseIterator(const ObjectType* object_ptr)
        : SuperType(const_cast<ObjectType*>(object_ptr)) {}

    FORCEINLINE DequeConstReverseIterator& operator=(const ObjectType* object_ptr) {
        SuperType::object_ptr_ = const_cast<ObjectType*>(object_ptr);
        return *this;
    }

    NODISCARD FORCEINLINE const ObjectType& operator[](const IndexType index) const { 
        return SuperType::object_ptr_[-index]; 
    }
    NODISCARD FORCEINLINE const ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE DequeConstReverseIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE DequeConstReverseIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }

    FORCEINLINE DequeConstReverseIterator& operator++() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE DequeConstReverseIterator& operator--() {
        ++SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE DequeConstReverseIterator operator+(const IndexType data_num) const {
        return DequeConstReverseIterator(SuperType::object_ptr_ - data_num);
    }
    NODISCARD FORCEINLINE DequeConstReverseIterator operator-(const IndexType data_num) const {
        return DequeConstReverseIterator(SuperType::object_ptr_ + data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const DequeConstReverseIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const DequeConstReverseIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const DequeConstReverseIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const DequeConstReverseIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const DequeConstReverseIterator& iterator) const {
        return static_cast<IndexType>(iterator.SuperType::object_ptr_ - SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE const ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = DequeIteratorBase<ObjectType>;
};

}//internal

/*
    Double end queue caintainer.
    If kIfUnique is true, will call the constuctor when the object is created.
*/
template<typename ObjectType, Bool kIfUnique = kIsClass<ObjectType>>
class ZDeque : public ZObject {
private:
    static constexpr Float32 kAutoExtendMulFactor = 1.5F;

public:
    using IteratorType = internal::DequeIterator<ObjectType>;
    using ConstIteratorType = internal::DequeConstIterator<ObjectType>;
    using ReverseIteratorType = internal::DequeReverseIterator<ObjectType>;
    using ConstReverseIteratorType = internal::DequeConstReverseIterator<ObjectType>;

    ZDeque() noexcept;
    ZDeque(const IndexType capacity) noexcept;
    /*
        Fills the container by the object constructed by the arguements.
    */
    template<typename... ArgsType>
    ZDeque(const IndexType capacity, ArgsType&&... args) noexcept;    
    ZDeque(const IteratorType& begin, const IteratorType& end) noexcept;
    ZDeque(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept;
    ZDeque(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;
    ZDeque(const ConstReverseIteratorType& begin, const ConstReverseIteratorType& end) noexcept;
    ZDeque(const ZDeque& queue) noexcept;
    ZDeque(ZDeque&& queue) noexcept;

    ZDeque& operator=(const ZDeque& queue) noexcept;
    ZDeque& operator=(ZDeque&& queue) noexcept;

    NODISCARD FORCEINLINE const Bool operator==(const ZDeque& queue) noexcept;
    NODISCARD FORCEINLINE const Bool operator!=(const ZDeque& queue) noexcept;

    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index);
    NODISCARD FORCEINLINE const ObjectType& operator[](const IndexType index) const;

    ~ZDeque() noexcept;

    NODISCARD FORCEINLINE ObjectType& At(const IndexType index);
    NODISCARD FORCEINLINE const ObjectType& At(const IndexType index) const;
    NODISCARD FORCEINLINE ObjectType& Front();
    NODISCARD FORCEINLINE const ObjectType& Front() const;
    NODISCARD FORCEINLINE ObjectType& Back();
    NODISCARD FORCEINLINE const ObjectType& Back() const;

    NODISCARD FORCEINLINE const IndexType size() const { return size_; }
    NODISCARD FORCEINLINE const IndexType capacity() const { return capacity_; }

    NODISCARD FORCEINLINE const Bool IsEmpty();

    /*
        Resize the queue, If the given size is smaller then the current size,
        the function will cut remove the extra objects. If the given size is
        bigger then the current size, the function will fill the extra size with
        the default value of ObjectType.
    */
    Void Resize(const IndexType size) noexcept;
    /*
        Resize the queue, If the given size is smaller then the current size,
        the function will cut remove the extra objects. If the given size is
        bigger then the current size, the function will fill the extra size with
        the object constructed by the arguements.
    */
    template<typename... ArgsType>
    Void Resize(const IndexType size, ArgsType&&... args) noexcept;

    /*
        Will extend the capacity by the given capacity, if the smaller then the
        given capacity, this function will do nothing, otherwise the capacity will
        entend to a size that equals or bigger then the given capacity.
    */
    Void Reserve(const IndexType capacity) noexcept;

    /*
        Shrinks the queue to the minimum capacity that can fit the current size.
    */
    Void ShrinkToFit() noexcept;

    /*
        Remove the object at the front of the queue.
    */
    Void PopFront() noexcept;
    /*
        Remove the object at the front of the queue.. Give the authority to the given address.
    */
    Void PopFront(ObjectType* object_ptr) noexcept;

    /*
        Create an object at the front of the queue by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the queue.
    */
    template<typename... ArgsType>
    Void PushFront(ArgsType&&... args) noexcept;
    /*
        Create objects at the front of the queue by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the queue.
    */
    template<typename... ArgsType>
    Void PushFronts(const IndexType num, ArgsType&&... args) noexcept;

    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the queue.
    */
    Void PushFronts(const IteratorType& begin, const IteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the queue.
    */
    Void PushFronts(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the queue.
    */
    Void PushFronts(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the queue.
    */
    Void PushFronts(const ConstReverseIteratorType& begin, const ConstReverseIteratorType& end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void EmplaceFront(ArgsType&&... args) noexcept;

    /*
        Remove the object at the back of the queue.
    */
    Void PopBack() noexcept;
    /*
        Remove the object at the back of the queue.. Give the authority to the given address.
    */
    Void PopBack(ObjectType* object_ptr) noexcept;

    /*
        Create an object at the back of the queue by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the queue.
    */
    template<typename... ArgsType>
    Void PushBack(ArgsType&&... args) noexcept;
    /*
        Create objects at the back of the queue by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the queue.
    */
    template<typename... ArgsType>
    Void PushBacks(const IndexType num, ArgsType&&... args) noexcept;

    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the queue.
    */
    Void PushBacks(const IteratorType& begin, const IteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the queue.
    */
    Void PushBacks(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the queue.
    */
    Void PushBacks(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the queue.
    */
    Void PushBacks(const ConstReverseIteratorType& begin, const ConstReverseIteratorType& end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void EmplaceBack(ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    IteratorType Insert(const IndexType index, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    IteratorType Insert(const IteratorType& iterator, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    ReverseIteratorType Insert(const ReverseIteratorType& iterator, ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    IteratorType Inserts(const IndexType index, IndexType num, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    IteratorType Inserts(const IteratorType& iterator, IndexType num, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    ReverseIteratorType Inserts(const ReverseIteratorType& iterator, IndexType num, ArgsType&&... args) noexcept;

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IndexType index,
        const IteratorType& src_begin, const IteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IndexType index,
        const ConstIteratorType& src_begin, const ConstIteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IndexType index,
        const ReverseIteratorType& src_begin, const ReverseIteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IndexType index,
        const ConstReverseIteratorType& src_begin, const ConstReverseIteratorType& src_end) noexcept;

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IteratorType& iterator,
        const IteratorType& src_begin, const IteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IteratorType& iterator,
        const ConstIteratorType& src_begin, const ConstIteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IteratorType& iterator,
        const ReverseIteratorType& src_begin, const ReverseIteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IteratorType& iterator,
        const ConstReverseIteratorType& src_begin, const ConstReverseIteratorType& src_end) noexcept;

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    ReverseIteratorType Inserts(const ReverseIteratorType& iterator,
        const IteratorType& src_begin, const IteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    ReverseIteratorType Inserts(const ReverseIteratorType& iterator,
        const ConstIteratorType& src_begin, const ConstIteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    ReverseIteratorType Inserts(const ReverseIteratorType& iterator,
        const ReverseIteratorType& src_begin, const ReverseIteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    ReverseIteratorType Inserts(const ReverseIteratorType& iterator,
        const ConstReverseIteratorType& src_begin,
        const ConstReverseIteratorType& src_end) noexcept;

    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    IteratorType Erase(const IndexType index) noexcept;
    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    IteratorType Erase(const IteratorType& iterator) noexcept;
    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    ReverseIteratorType Erase(const ReverseIteratorType& iterator) noexcept;

    /*
        Erases the num of objects that starts at the given index.
        Returns the iterator that points at the next object.
    */
    IteratorType Erases(const IndexType index, const IndexType num) noexcept;
    /*
        Erases the num of objects that starts at the given iterator.
        Returns the iterator that points at the next object.
    */
    IteratorType Erases(const IteratorType& iterator, const IndexType num) noexcept;
    /*
        Erases the num of objects that starts at the given iterator.
        Returns the iterator that points at the next object.
    */
    ReverseIteratorType Erases(const ReverseIteratorType& iterator, const IndexType num) noexcept;
    /*
        Erases the object between begin and end, involves begin, but don't involves end.
        Returns the iterator that points at the next object.
    */
    IteratorType Erases(const IteratorType& begin, const IteratorType& end) noexcept;
    /*
        Erases the object between begin and end, involves begin, but don't involves end.
        Returns the iterator that points at the next object.
    */
    ReverseIteratorType Erases(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const IndexType index, ArgsType&&... args) noexcept;
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const IteratorType& iterator, ArgsType&&... args) noexcept;
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const ReverseIteratorType& iterator, ArgsType&&... args) noexcept;

    /*
        Construct the queue by filling it with the given amount of objects.
        The object is constructed by the arguements.
        If kIfUnique is false and no arguements, will only add the 
        size of the queue.
    */
    template<typename... ArgsType>
    Void Assign(const IndexType num, ArgsType&&... args) noexcept;
    /*
        Construct the queue by filling it objects between the iterators.
    */
    Void Assign(const IteratorType& begin, const IteratorType& end) noexcept;
    /*
        Construct the queue by filling it objects between the iterators.
    */
    Void Assign(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept;
    /*
        Construct the queue by filling it objects between the iterators.
    */
    Void Assign(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;
    /*
        Construct the queue by filling it objects between the iterators.
    */
    Void Assign(const ConstReverseIteratorType& begin, const ConstReverseIteratorType& end) noexcept;
    
    /*
        Destroys all the objects in the queue, does not release the memory.
    */
    Void Clear() noexcept;
    /*
        Destroys all the objects in the queue, release the memory.
    */
    Void Destroy() noexcept;

protected:
    using SuperType = ZObject;

private:    
    struct DataNode {
    public:
        DataNode* next_node_ptr;
        DataNode* previous_node_ptr;
        IndexType capacity;
        IndexType size;
    };

    /*
        Creates the capacity by the given capacity, the final capacity might
        not equal the given capacity.
    */
    FORCEINLINE Void CreateContainer(const IndexType capacity) noexcept;
    /*
        Extends the capacity by the given capacity, the final capacity might
        not equal the given capacity.
    */
    Void ExtendContainer(const IndexType capacity) noexcept;
    /*
        Shrinks the container to the minimum capacity that can fit the current size.
    */
    FORCEINLINE Void ShrinkContainer() noexcept;
    /*
        Destroys the container.
    */
    FORCEINLINE Void DestroyContainer() noexcept;

    /*
        Called when the container is moved.
    */
    FORCEINLINE Void MoveDestroy();

    /*
        Creates an object at the certain index. Will call the Constrctor if needed.
    */
    template<typename... ArgsType>
    FORCEINLINE Void CreateObject(const IndexType index, ArgsType&&... args);
    /*
        Creates an object at the certain place. Will call the Constrctor if needed.
    */
    template<typename... ArgsType>
    FORCEINLINE Void CreateObject(ObjectType* const object_ptr, ArgsType&&... args);

    /*
        Destroys an object at the certain index. Will Call Destrctor if needed.
    */
    FORCEINLINE Void DestroyObject(const IndexType index);
    /*
        Destroys an object at the certain place. Will Call Destrctor if needed.
    */
    FORCEINLINE Void DestroyObject(ObjectType* const object_ptr);

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the Constrctor if needed.
    */
    template<typename... ArgsType>
    FORCEINLINE Void CreateObjects(ObjectType* begin_ptr, ObjectType* const end_ptr, ArgsType&&... args);

    /*
        Initialize the memory by the given arguements([begin, begin + num)),
        Will call the Constrctor if needed.
    */
    template<typename... ArgsType>
    FORCEINLINE Void CreateObjects(ObjectType* begin_ptr, const IndexType num, ArgsType&&... args);

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the copy constructor.
    */
    FORCEINLINE Void CreateAndCopyObjects(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
                                          const ObjectType* const src_end_ptr);

    /*
        Initialize the memory by the given arguements([begin + num)).
        Will call the copy constructor.
    */
    FORCEINLINE Void CreateAndCopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr, const IndexType num);

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the copy constructor.
    */
    FORCEINLINE Void CreateAndCopyObjectsReverse(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
                                                 const ObjectType* const src_end_ptr);

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the copy constructor.
    */
    FORCEINLINE Void CreateAndCopyObjectsReverse(ObjectType* dst_ptr, const ObjectType* src_ptr, const IndexType num);

    /*
        Copy objects by the given pointer. Will call the copy assignment operator
        if this object class's member kIfUnique is true.
    */
    FORCEINLINE Void CopyObjects(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
                                 const ObjectType* const src_end_ptr);

    /*
        Copy objects by the given pointer. Will call the copy assignment operator
        if this object class's member kIfUnique is true. 
    */
    FORCEINLINE Void CopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr, const IndexType num);

    /*
        Copy objects by the given pointer. Will call the copy assignment operator
        if this object class's member kIfUnique is true.
    */
    FORCEINLINE Void CopyObjectsReverse(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
                                        const ObjectType* const src_end_ptr);

    /*
        Copy objects by the given pointer. Will call the copy assignment operator
        if this object class's member kIfUnique is true.
    */
    FORCEINLINE Void CopyObjectsReverse(ObjectType* dst_ptr, const ObjectType* src_ptr, const IndexType num);

    /*
        Destroy the objects by the given arguements([begin, end)). 
        Will call the destrctor if this object class's member kIfUnique is true.
    */
    FORCEINLINE Void DestroyObjects(ObjectType* begin_ptr, ObjectType* const end_ptr);

    /*
        Destroy the objects by the given arguements([begin, begin + num)).
        Will call the destrctor if this object class's member kIfUnique is true.
    */
    FORCEINLINE Void DestroyObjects(ObjectType* begin_ptr, const IndexType num);

    DataNode* front_node_ptr_;
    DataNode* back_node_ptr_;
    DataNode* emptr_node_ptr_;
    IndexType front_index_;
    IndexType back_index_;
    IndexType capacity_;
    IndexType size_;
};

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::ZDeque() noexcept
    : SuperType()
    , front_node_ptr_(nullptr)
    , back_node_ptr_(nullptr)
    , emptr_node_ptr_(nullptr)
    , front_index_(0)
    , back_index_(0)
    , capacity_(0)
    , size_(0)
{}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::ZDeque(const IndexType capacity) noexcept
    : SuperType()
    , front_ptr_(nullptr)
    , back_ptr_(nullptr)
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainer(capacity);
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
ZDeque<ObjectType, kIfUnique>::ZDeque(const IndexType capacity, ArgsType&&... args) noexcept
    : SuperType() 
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainer(capacity);
    CreateObjects(data_ptr_, capacity, std::forward<ArgsType>(args)...);
    size_ = capacity;
    front_ptr_ = data_ptr_;
    back_ptr_ = data_ptr_ + size_ - 1;
}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::ZDeque(const ZDeque& queue) noexcept 
    : SuperType()
{
    CreateContainer(queue.size_);
    CreateAndCopyObjects(data_ptr_, queue.data_ptr_, queue.size_);
    size_ = queue.size_;
    front_ptr_ = data_ptr_;
    back_ptr_ = data_ptr_ + size_ - 1;
} 

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::ZDeque(ZDeque&& queue) noexcept 
    : SuperType()
    , data_ptr_(queue.data_ptr_)
    , capacity_(queue.capacity_)
    , front_ptr_(queue.front_ptr_)
    , back_ptr_(queue.back_ptr_)
    , size_(queue.size_)
{
    queue.MoveDestroy();
}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>& ZDeque<ObjectType, kIfUnique>::operator=(
        const ZDeque& queue) noexcept {
    DEBUG(&queue == this, "The source and the target of the copy is the same!");
    SuperType::operator=(queue);
    if (queue.size_ > capacity_) {
        ExtendContainer(queue.size_);
    }
    CopyObjects(data_ptr_, queue.data_ptr_, queue.size_);
    size_ = queue.size_;
    front_ptr_ = data_ptr_;
    back_ptr_ = data_ptr_ + size_ - 1;
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>& ZDeque<ObjectType, kIfUnique>::operator=(
        ZDeque&& queue) noexcept {
    DEBUG(&queue == this, "The source and the target of the copy is the same!");
    SuperType::operator=(std::forward<ZDeque>(queue));
    DestroyObjects(data_ptr_, size_);
    data_ptr_ = queue.data_ptr_;
    front_ptr_ = queue.front_ptr_;
    back_ptr_ = queue.back_ptr_;
    size_ = queue.size_;
    capacity_ = queue.capacity_;
    queue.MoveDestroy();
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::~ZDeque() noexcept {
    DestroyContainer();
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::Resize(const IndexType size) noexcept {
    DEBUG(size < 0, "Negaive size is not valid!");
    if (size_ < size) {
        if (size > capacity_) {
            ExtendContainer(size);
        }
        CreateObjects(data_ptr_ + size_, size - size_);
    }
    else {
        DestroyObjects(data_ptr_ + size, data_ptr_ + size_);
    }
    size_ = size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZDeque<ObjectType, kIfUnique>::Resize(const IndexType size, ArgsType&&... args) noexcept {
    DEBUG(size < 0, "Negaive size is not valid!");
    if (size_ < size) {
        if (size > capacity_) {
            ExtendContainer(size);
        }
        CreateObjects(data_ptr_ + size_, size - size_, std::forward<ArgsType>(args)...);
    }
    else {
        DestroyObjects(data_ptr_ + size, data_ptr_ + size_);
    }
    size_ = size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::Reserve(const IndexType capacity) noexcept {
    if (capacity > capacity_) {
        ExtendContainer(capacity);
    }
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::PopFront() noexcept {
    DEBUG(size_ == 0, "No existing object to pop!");
    --size_;
    DestroyObject(size_);
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::PopFront(ObjectType* object_ptr) noexcept {
    DEBUG(size_ == 0, "No existing object to pop!");
    --size_;
    *object_ptr = std::move(data_ptr_[size_]);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZDeque<ObjectType, kIfUnique>::PushBack(ArgsType&&... args) noexcept {
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObject(size_, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZDeque<ObjectType, kIfUnique>::PushBacks(const IndexType num, ArgsType&&... args) noexcept {
    DEBUG(num < 0, "Negative pushing num not valid!");
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjects(data_ptr_ + size_, num, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZDeque<ObjectType, kIfUnique>::Assign(const IndexType num, ArgsType&&... args) noexcept {
    IndexType new_size = num;
    DEBUG(num < 0, "Negative assign num not valid!");
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    DestroyObjects(data_ptr_, size_);
    CreateObjects(data_ptr_, num, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::Clear() noexcept {
    DestroyObjects(data_ptr_, data_ptr_ + size_);
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::Destroy() noexcept {
    DestroyContainer();
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CreateContainer(const IndexType capacity) noexcept {
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    MemoryType need_memory_size = capacity * sizeof(ObjectType);
    MemoryType apply_mrmory_size;
    data_ptr_ = reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));
    capacity_ = apply_mrmory_size / sizeof(ObjectType);
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::ExtendContainer(const IndexType capacity) noexcept {
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    MemoryType current_memory_size = capacity_ * sizeof(ObjectType);
    MemoryType need_memory_size = capacity * sizeof(ObjectType);
    MemoryType apply_mrmory_size;
    //If the current memory piece doesn't have enough memory.
    if (!memory_pool::CheckMemory(reinterpret_cast<Void*>(data_ptr_), need_memory_size, &apply_mrmory_size)) {
        ObjectType* temp_data_ptr = 
            reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));
        //Needs to split the copy into two times.
        if (front_ptr_ > back_ptr_) {
            IndexType part_1_num = capacity_ - static_cast<IndexType>(front_ptr_ - data_ptr_);
            memcpy(reinterpret_cast<Void*>(temp_data_ptr), reinterpret_cast<Void*>(front_ptr_),
                part_1_num * sizeof(ObjectType));
            memcpy(reinterpret_cast<Void*>(temp_data_ptr + part_1_num), reinterpret_cast<Void*>(data_ptr_),
                (size_ - part_1_num) * sizeof(ObjectType));
        }
        else {
            memcpy(reinterpret_cast<Void*>(temp_data_ptr), reinterpret_cast<Void*>(front_ptr_),
                size_ * sizeof(ObjectType));
        }
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(data_ptr_));
        data_ptr_ = temp_data_ptr;
        front_ptr_ = data_ptr_;
        back_ptr_ = data_ptr_ + size_;
    }
    else {
        //Needs to move the objects from the front to the back.
        if (front_ptr_ > back_ptr_) {
            IndexType part_2_num = size_ - capacity_ + static_cast<IndexType>(front_ptr_ - data_ptr_);
            IndexType temp_capacity = apply_mrmory_size / sizeof(ObjectType);
            IndexType addition_capacity = temp_capacity - capacity_;
            if (addition_capacity < part_2_num) {
                memcpy(reinterpret_cast<Void*>(data_ptr_ + size_), reinterpret_cast<Void*>(data_ptr_),
                    addition_capacity * sizeof(ObjectType));
                memmove(reinterpret_cast<Void*>(data_ptr_ + addition_capacity), reinterpret_cast<Void*>(data_ptr_),
                    (part_2_num - addition_capacity) * sizeof(ObjectType));
                back_ptr_ = back_ptr_ - addition_capacity;
            }
            else {
                memcpy(reinterpret_cast<Void*>(data_ptr_ + size_), reinterpret_cast<Void*>(data_ptr_),
                    part_2_num * sizeof(ObjectType));
                back_ptr_ = front_ptr_ + size_;
            }
        }
    }
    capacity_ = apply_mrmory_size / sizeof(ObjectType);
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::DestroyContainer() noexcept {
    DestroyObjects(front_ptr_, back_ptr_ + 1);
    memory_pool::ReleaseMemory(reinterpret_cast<Void*>(data_ptr_));
    data_ptr_ = nullptr;
    front_ptr_ = nullptr;
    back_ptr_ = nullptr;
    capacity_ = 0;
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::MoveDestroy() {
    data_ptr_ = nullptr;
    front_ptr_ = nullptr;
    back_ptr_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CreateObject(const IndexType index, ArgsType&&... args) {
    if constexpr (sizeof...(args) == 0) {
        if constexpr (kIfUnique) {
            new(reinterpret_cast<Void*>(&(data_ptr_[index]))) ObjectType();
        }
    }
    else {
        new(reinterpret_cast<Void*>(&(data_ptr_[index]))) ObjectType(std::forward<ArgsType>(args)...);
    }
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CreateObject(ObjectType* const object_ptr, ArgsType&&... args) {
    if constexpr (sizeof...(args) == 0) {
        if constexpr (kIfUnique) {
            new(reinterpret_cast<Void*>(object_ptr)) ObjectType();
        }
    }
    else {
        new(reinterpret_cast<Void*>(object_ptr)) ObjectType(std::forward<ArgsType>(args)...);
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::DestroyObject(const IndexType index) {
    if constexpr (kIfUnique) {
        data_ptr_[index].~ObjectType();
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::DestroyObject(ObjectType* const object_ptr) {
    if constexpr (kIfUnique) {
        (*object_ptr).~ObjectType();
    }
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CreateObjects(ObjectType* begin_ptr, ObjectType* const end_ptr,
                                                              ArgsType&&... args) {
    if constexpr (kIfUnique) {
        if constexpr (sizeof...(args) == 0) {
            new(reinterpret_cast<Void*>(begin_ptr)) ObjectType[end_ptr - begin_ptr];
        }
        else {
            while (begin_ptr < end_ptr) {
                new(reinterpret_cast<Void*>(begin_ptr++)) ObjectType(std::forward<ArgsType>(args)...);
            }
        }
    }
    else {
        if (begin_ptr == end_ptr) {
            return;
        }
        ObjectType* temp_object_ptr =
            new(reinterpret_cast<Void*>(begin_ptr++)) ObjectType(std::forward<ArgsType>(args)...);
        while (begin_ptr < end_ptr) {
            *begin_ptr = *temp_object_ptr;
            ++begin_ptr;
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CreateObjects(ObjectType* begin_ptr, const IndexType num,
                                                              ArgsType&&... args) {
    if constexpr (kIfUnique) {
        if constexpr (sizeof...(args) == 0) {
            new(reinterpret_cast<Void*>(begin_ptr)) ObjectType[num];
        }
        ObjectType* end_ptr = begin_ptr + num;
        while (begin_ptr < end_ptr) {
            new(reinterpret_cast<Void*>(begin_ptr++)) ObjectType(std::forward<ArgsType>(args)...);
        }
    }
    else {
        if (num == 0) {
            return;
        }
        ObjectType* end_ptr = begin_ptr + num;
        ObjectType* temp_object_ptr =
            new(reinterpret_cast<Void*>(begin_ptr++)) ObjectType(std::forward<ArgsType>(args)...);
        while (begin_ptr < end_ptr) {
            *begin_ptr = *temp_object_ptr;
            ++begin_ptr;
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CreateAndCopyObjects(ObjectType* dst_ptr, 
                                                                     const ObjectType* src_begin_ptr, 
                                                                     const ObjectType* const src_end_ptr) {
    if constexpr (kIfUnique) {
        while (src_begin_ptr < src_end_ptr) {
            new(reinterpret_cast<Void*>(dst_ptr)) ObjectType(*src_begin_ptr);
            ++dst_ptr;
            ++src_begin_ptr;
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(dst_ptr), reinterpret_cast<Void*>(const_cast<ObjectType*>(src_begin_ptr)),
            static_cast<SizeType>((src_end_ptr - src_begin_ptr) * sizeof(ObjectType)));
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CreateAndCopyObjects(ObjectType* dst_ptr,
                                                                     const ObjectType* src_ptr,
                                                                     const IndexType num) {
    if constexpr (kIfUnique) {
        ObjectType* const end_ptr = dst_ptr + num;
        while (dst_ptr < end_ptr) {
            new(reinterpret_cast<Void*>(dst_ptr)) ObjectType(*src_ptr);
            ++dst_ptr;
            ++src_ptr;
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(dst_ptr), reinterpret_cast<Void*>(const_cast<ObjectType*>(src_ptr)),
            static_cast<SizeType>(num * sizeof(ObjectType)));
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CreateAndCopyObjectsReverse(
        ObjectType* dst_ptr, const ObjectType* src_begin_ptr, const ObjectType* const src_end_ptr) {
    while (src_begin_ptr > src_end_ptr) {
        new(reinterpret_cast<Void*>(dst_ptr)) ObjectType(*src_begin_ptr);
        ++dst_ptr;
        --src_begin_ptr;
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CreateAndCopyObjectsReverse(ObjectType* dst_ptr, 
                                                                            const ObjectType* src_ptr, 
                                                                            const IndexType num) {
    ObjectType* const end_ptr = dst_ptr + num;
    while (dst_ptr > end_ptr) {
        new(reinterpret_cast<Void*>(dst_ptr)) ObjectType(*src_ptr);
        ++dst_ptr;
        --src_ptr;
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CopyObjects(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
                                                            const ObjectType* const src_end_ptr) {
    IndexType num = src_end_ptr - src_begin_ptr;
    if constexpr (kIfUnique) {
        if (size_ < num) {
            const ObjectType* end_ptr = src_begin_ptr + size_;
            while (src_begin_ptr < end_ptr) {
                *dst_ptr = *src_begin_ptr;
                ++dst_ptr;
                ++src_begin_ptr;
            }
            CreateAndCopyObjects(dst_ptr, src_begin_ptr, src_end_ptr);
        }
        else {
            while (src_begin_ptr < src_end_ptr) {
                *dst_ptr = *src_begin_ptr;
                ++dst_ptr;
                ++src_begin_ptr;
            }
            DestroyObjects(dst_ptr, dst_ptr + (size_ - num));
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(dst_ptr), reinterpret_cast<Void*>(const_cast<ObjectType*>(src_begin_ptr)),
            static_cast<SizeType>(num * sizeof(ObjectType)));
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr,
                                                            const IndexType num) {
    if constexpr (kIfUnique) {
        ObjectType* end_ptr;
        if (size_ < num) {
            end_ptr = dst_ptr + size_;
            while (dst_ptr < end_ptr) {
                *dst_ptr = *src_ptr;
                ++dst_ptr;
                ++src_ptr;
            }
            CreateAndCopyObjects(dst_ptr, src_ptr, num - size_);
        }
        else {
            end_ptr = dst_ptr + num;
            while (dst_ptr < end_ptr) {
                *dst_ptr = *src_ptr;
                ++dst_ptr;
                ++src_ptr;
            }
            end_ptr = end_ptr + (size_ - num);
            DestroyObjects(dst_ptr, dst_ptr + (size_ - num));
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(dst_ptr), reinterpret_cast<Void*>(const_cast<ObjectType*>(src_ptr)),
               static_cast<SizeType>(num * sizeof(ObjectType)));
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CopyObjectsReverse(ObjectType* dst_ptr,
                                                                   const ObjectType* src_begin_ptr,
                                                                   const ObjectType* const src_end_ptr) {
    IndexType num = static_cast<IndexType>(src_begin_ptr - src_end_ptr);
    if (size_ < num) {
        const ObjectType* end_ptr = src_begin_ptr - size_;
        while (src_begin_ptr > end_ptr) {
            *dst_ptr = *src_begin_ptr;
            ++dst_ptr;
            --src_begin_ptr;
        }
        CreateAndCopyObjectsReverse(dst_ptr, src_begin_ptr, src_end_ptr);
    }
    else {
        while (src_begin_ptr > src_end_ptr) {
            *dst_ptr = *src_begin_ptr;
            ++dst_ptr;
            --src_begin_ptr;
        }
        DestroyObjects(dst_ptr, dst_ptr + (size_ - num));
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CopyObjectsReverse(ObjectType* dst_ptr, const ObjectType* src_ptr, 
                                                                   const IndexType num) {
    const ObjectType* src_end_ptr = src_ptr - num;
    if (size_ < num) {
        const ObjectType* end_ptr = src_ptr - size_;
        while (src_ptr > end_ptr) {
            *dst_ptr = *src_ptr;
            ++dst_ptr;
            --src_ptr;
        }
        CreateAndCopyObjectsReverse(dst_ptr, src_ptr, src_end_ptr);
    }
    else {
        while (src_ptr > src_end_ptr) {
            *dst_ptr = *src_ptr;
            ++dst_ptr;
            --src_ptr;
        }
        DestroyObjects(dst_ptr, dst_ptr + (size_ - num));
    }
}


template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::DestroyObjects(ObjectType* begin_ptr, ObjectType* const end_ptr) {
    if constexpr (kIfUnique) {
        while (begin_ptr < end_ptr) {
            begin_ptr->~ObjectType();
            ++begin_ptr;
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::DestroyObjects(ObjectType* begin_ptr, const IndexType num) {
    if constexpr (kIfUnique) {
        ObjectType* end_ptr = begin_ptr + num;
        while (begin_ptr < end_ptr) {
            begin_ptr->~ObjectType();
            ++begin_ptr;
        }
    }
}

}//zengine


#endif // !Z_CORE_Z_DEQUE_H_