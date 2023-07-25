#ifndef Z_CORE_Z_VECTOR_H_
#define Z_CORE_Z_VECTOR_H_

#include "internal/drive.h"

#include "m_error_message.h"
#include "z_object.h"

namespace zengine {

template<typename ObjectType, Bool kIfUnique>
class ZVector;

namespace internal {

template<typename ObjectType>
class ZVectorIteratorBase {
public:
    FORCEINLINE ZVectorIteratorBase(ObjectType* object_ptr) : object_ptr_(object_ptr) {}
    FORCEINLINE ZVectorIteratorBase(const ZVectorIteratorBase& iterator) : object_ptr_(iterator.object_ptr_) {}
    FORCEINLINE ZVectorIteratorBase(ZVectorIteratorBase&& iterator) { 
        MoveP(std::forward<ZVectorIteratorBase>(iterator)); 
    }

    FORCEINLINE ZVectorIteratorBase& operator=(const ZVectorIteratorBase& iterator) {
        object_ptr_ = iterator.object_ptr_;
        return *this;
    }
    FORCEINLINE ZVectorIteratorBase& operator=(ZVectorIteratorBase&& iterator) {
        MoveP(std::forward<ZVectorIteratorBase>(iterator));
        return *this;
    }

    NODISCARD FORCEINLINE Bool operator==(const ZVectorIteratorBase& iterator) const {
        return object_ptr_ == iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const ZVectorIteratorBase& iterator) const {
        return object_ptr_ != iterator.object_ptr_;
    }

    NODISCARD FORCEINLINE ObjectType& operator*() const { return *object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* operator->() const { return object_ptr_; }

    FORCEINLINE ~ZVectorIteratorBase() {}

    NODISCARD FORCEINLINE ObjectType* object_ptr() const { return object_ptr_; }

protected:
    ObjectType* object_ptr_;

private:
    FORCEINLINE Void MoveP(ZVectorIteratorBase&& iterator) {
        object_ptr_ = iterator.object_ptr_;
        iterator.object_ptr_ = nullptr;
    }
};

template<typename ObjectType>
class ZVectorIterator : public ZVectorIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) const { return SuperType::object_ptr_[index]; }

    FORCEINLINE ZVectorIterator& operator+=(IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE ZVectorIterator& operator-=(IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }

    FORCEINLINE ZVectorIterator& operator++() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZVectorIterator& operator++(IndexType) {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZVectorIterator& operator--() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZVectorIterator& operator--(IndexType) {
        --SuperType::object_ptr_;
        return *this;
    }
    NODISCARD FORCEINLINE ZVectorIterator operator+(IndexType data_num) const {
        return ZVectorIterator(SuperType::object_ptr_ + data_num);
    }
    NODISCARD FORCEINLINE ZVectorIterator operator-(IndexType data_num) const {
        return ZVectorIterator(SuperType::object_ptr_ - data_num);
    }

    NODISCARD FORCEINLINE Bool operator>(const ZVectorIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator>=(const ZVectorIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator<(const ZVectorIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator<=(const ZVectorIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE IndexType operator-(const ZVectorIterator& iterator) const {
        return static_cast<IndexType>(SuperType::object_ptr_ - iterator.SuperType::object_ptr_);
    }

protected:
    using SuperType = ZVectorIteratorBase<ObjectType>;
};

template<typename ObjectType>
class ZVectorReverseIterator : public ZVectorIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) const { return SuperType::object_ptr_[-index]; }

    FORCEINLINE ZVectorReverseIterator& operator+=(IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE ZVectorReverseIterator& operator-=(IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }

    FORCEINLINE ZVectorReverseIterator& operator++() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZVectorReverseIterator& operator++(IndexType) {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZVectorReverseIterator& operator--() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZVectorReverseIterator& operator--(IndexType) {
        ++SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE ZVectorReverseIterator operator+(IndexType data_num) const {
        return ZVectorReverseIterator(SuperType::object_ptr_ - data_num);
    }
    NODISCARD FORCEINLINE ZVectorReverseIterator operator-(IndexType data_num) const {
        return ZVectorReverseIterator(SuperType::object_ptr_ + data_num);
    }

    NODISCARD FORCEINLINE Bool operator>(const ZVectorReverseIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator>=(const ZVectorReverseIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator<(const ZVectorReverseIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator<=(const ZVectorReverseIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE IndexType operator-(const ZVectorReverseIterator& iterator) const {
        return static_cast<IndexType>(iterator.SuperType::object_ptr_ - SuperType::object_ptr_);
    }

protected:
    using SuperType = ZVectorIteratorBase<ObjectType>;
};

}//internal

/*
    Vector caintainer.
    If kIfUnique is true, will call the constuctor when the object is created.
*/
template<typename ObjectType, Bool kIfUnique = kIsClass<ObjectType>>
class ZVector : public ZObject {
private:
    static constexpr Float32 kAutoExtendMulFactor = 1.5F;

public:
    using IteratorType = internal::ZVectorIterator<ObjectType>;
    using ConstIteratorType = internal::ZVectorIterator<const ObjectType>;
    using ReverseIteratorType = internal::ZVectorReverseIterator<ObjectType>;
    using ConstReverseIteratorType = internal::ZVectorReverseIterator<const ObjectType>;

    ZVector() noexcept;
    ZVector(IndexType capacity) noexcept;
    /*
        Fills the container by the object constructed by the arguements.
    */
    template<typename... ArgsType>
    ZVector(IndexType capacity, ArgsType&&... args) noexcept;    
    ZVector(const IteratorType& begin, const IteratorType& end) noexcept : SuperType() { 
        ZVectorOrderP(begin.object_ptr(), end.object_ptr());
    }
    ZVector(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept : SuperType() {
        ZVectorOrderP(begin.object_ptr(), end.object_ptr());
    }
    ZVector(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept : SuperType() {
        ZVectorReverseP(begin.object_ptr(), end.object_ptr());
    }
    ZVector(const ConstReverseIteratorType& begin, const ConstReverseIteratorType& end) noexcept : SuperType() {
        ZVectorReverseP(begin.object_ptr(), end.object_ptr());
    }
    ZVector(const ZVector& vector) noexcept;
    ZVector(ZVector&& vector) noexcept;

    ZVector& operator=(const ZVector& vector) noexcept;
    ZVector& operator=(ZVector&& vector) noexcept;

    NODISCARD FORCEINLINE Bool operator==(const ZVector& vector) noexcept { 
        return data_ptr_ == vector.data_ptr_; 
    }    
    NODISCARD FORCEINLINE Bool operator!=(const ZVector& vector) noexcept { 
        return data_ptr_ != vector.data_ptr_; 
    }

    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) { 
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_[index]; 
    }
    NODISCARD FORCEINLINE const ObjectType& operator[](IndexType index) const { 
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_[index]; 
    }

    ~ZVector() noexcept;


    NODISCARD FORCEINLINE ObjectType& At(IndexType index) {
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_[index];
    }
    NODISCARD FORCEINLINE const ObjectType& At(IndexType index) const {
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_[index];
    }
    NODISCARD FORCEINLINE ObjectType* AtPtr(IndexType index) {
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_ + index;
    }
    NODISCARD FORCEINLINE const ObjectType* AtPtr(IndexType index) const {
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_ + index;
    }
    NODISCARD FORCEINLINE ObjectType& Front() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[0]; 
    }
    NODISCARD FORCEINLINE const ObjectType& Front() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[0];
    }
    NODISCARD FORCEINLINE ObjectType* FrontPtr() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_;
    }
    NODISCARD FORCEINLINE const ObjectType* FrontPtr() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_;
    }
    NODISCARD FORCEINLINE ObjectType& Back() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[size_ - 1];
    }
    NODISCARD FORCEINLINE const ObjectType& Back() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[size_ - 1];
    }
    NODISCARD FORCEINLINE ObjectType* BackPtr() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_ + (size_ - 1);
    }
    NODISCARD FORCEINLINE const ObjectType* BackPtr() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_ + (size_ - 1);
    }

    NODISCARD FORCEINLINE IndexType size() const { return size_; }
    NODISCARD FORCEINLINE IndexType capacity() const { return capacity_; }
    NODISCARD FORCEINLINE const ObjectType* data_ptr() const { return data_ptr_; }

    /*
        The iterator funcions.
    */
    NODISCARD FORCEINLINE IteratorType Begin() {
        DEBUG(data_ptr_ == nullptr, "Vector not exist!");
        return IteratorType(data_ptr_); 
    }
    NODISCARD FORCEINLINE ConstIteratorType ConstBegin() const { 
        DEBUG(data_ptr_ == nullptr, "Vector not exist!"); 
        return ConstIteratorType(data_ptr_); 
    }
    NODISCARD FORCEINLINE ReverseIteratorType ReverseBegin() { 
        DEBUG(data_ptr_ == nullptr, "Vector not exist!"); 
        return ReverseIteratorType(data_ptr_ + size_ - 1); 
    }
    NODISCARD FORCEINLINE ConstReverseIteratorType ConstReverseBegin() const {
        DEBUG(data_ptr_ == nullptr, "Vector not exist!");
        return ConstReverseIteratorType(data_ptr_ + size_ - 1);
    }
    NODISCARD FORCEINLINE IteratorType End() { 
        DEBUG(data_ptr_ == nullptr, "Vector not exist!"); 
        return IteratorType(data_ptr_ + size_); 
    }
    NODISCARD FORCEINLINE ConstIteratorType ConstEnd() const { 
        DEBUG(data_ptr_ == nullptr, "Vector not exist!"); 
        return ConstIteratorType(data_ptr_ + size_); 
    }
    NODISCARD FORCEINLINE ReverseIteratorType ReverseEnd() { 
        DEBUG(data_ptr_ == nullptr, "Vector not exist!"); 
        return ReverseIteratorType(data_ptr_ - 1); 
    }
    NODISCARD FORCEINLINE ConstReverseIteratorType ConstReverseEnd() const {
        DEBUG(data_ptr_ == nullptr, "Vector not exist!");
        return ConstReverseIteratorType(data_ptr_ - 1);
    }

    NODISCARD FORCEINLINE Bool IsEmpty() { return size_ == 0; }

    /*
        Resize the vector, If the given size is smaller then the current size,
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
        Shrinks the vector to the minimum capacity that can fit the current size.
    */
    FORCEINLINE Void ShrinkToFit();

    /*
        Remove the object at the back of the vector.
    */
    FORCEINLINE Void PopBack();
    /*
        Remove the object at the back of the vector. 
        Give the authority to the given address.
    */
    FORCEINLINE Void PopBack(ObjectType* object_ptr);

    /*
        Create an object at the back of the vector by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the vector.
    */
    template<typename... ArgsType>
    inline Void PushBack(ArgsType&&... args) noexcept;
    /*
        Create objects at the back of the vector by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the vector.
    */
    template<typename... ArgsType>
    inline Void PushBacks(IndexType num, ArgsType&&... args) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the vector.
    */
    FORCEINLINE Void PushBacks(const IteratorType& src_begin, const IteratorType& src_end) {
        PushBacksOrderP(src_begin.object_ptr(), src_end.object_ptr());
    }
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the vector.
    */
    FORCEINLINE Void PushBacks(const ConstIteratorType& src_begin, const ConstIteratorType& src_end) {
        PushBacksOrderP(src_begin.object_ptr(), src_end.object_ptr());
    }
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the vector.
    */
    FORCEINLINE Void PushBacks(const ReverseIteratorType& src_begin, const ReverseIteratorType& src_end) {
        PushBacksReverseP(src_begin.object_ptr(), src_end.object_ptr());
    }
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the vector.
    */
    FORCEINLINE Void PushBacks(const ConstReverseIteratorType& src_begin, const ConstReverseIteratorType& src_end) {
        PushBacksReverseP(src_begin.object_ptr(), src_end.object_ptr());
    }

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    inline Void EmplaceBack(ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    FORCEINLINE IteratorType Insert(IndexType index, ArgsType&&... args) { 
        return IteratorType(InsertP(index, std::forward<ArgsType>(args)...));
    }
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE IteratorType Insert(const IteratorType& iterator, ArgsType&&... args) {
        return IteratorType(InsertP(iterator.object_ptr() - data_ptr_, std::forward<ArgsType>(args)...));
    }
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE ReverseIteratorType Insert(const ReverseIteratorType& iterator, ArgsType&&... args) {
        return ReverseIteratorType(InsertP(iterator.object_ptr() - data_ptr_ + 1, std::forward<ArgsType>(args)...));
    }

    /*
        Inserts before the index. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    FORCEINLINE IteratorType Inserts(IndexType index, IndexType num, ArgsType&&... args) {
        return IteratorType(InsertsP(index, num, std::forward<ArgsType>(args)...));
    }
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE IteratorType Inserts(const IteratorType& iterator, IndexType num, ArgsType&&... args) {
        return IteratorType(InsertsP(iterator.object_ptr() - data_ptr_, num, std::forward<ArgsType>(args)...));
    }
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE ReverseIteratorType Inserts(const ReverseIteratorType & iterator, IndexType num, 
                                                      ArgsType&&... args) {
        return ReverseIteratorType(
            InsertsP(iterator.object_ptr() - data_ptr_ + 1, num, std::forward<ArgsType>(args)...) + (num - 1));
    }

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE IteratorType Inserts(IndexType index, 
                                     const IteratorType& src_begin, 
                                     const IteratorType& src_end) {
        return IteratorType(InsertsOrderP(index, src_begin.object_ptr(), src_end.object_ptr()));
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE IteratorType Inserts(IndexType index,
                                     const ConstIteratorType& src_begin, 
                                     const ConstIteratorType& src_end) {
        return IteratorType(InsertsOrderP(index, src_begin.object_ptr(), src_end.object_ptr()));
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE IteratorType Inserts(IndexType index,
                                     const ReverseIteratorType& src_begin, 
                                     const ReverseIteratorType& src_end) {
        return IteratorType(InsertsReverseP(index, src_begin.object_ptr(), src_end.object_ptr()));
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE IteratorType Inserts(IndexType index,
                                     const ConstReverseIteratorType& src_begin, 
                                     const ConstReverseIteratorType& src_end) {
        return IteratorType(InsertsReverseP(index, src_begin.object_ptr(), src_end.object_ptr()));
    }

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE IteratorType Inserts(const IteratorType& iterator,
                                               const IteratorType& src_begin, 
                                               const IteratorType& src_end) {
        return IteratorType(
            InsertsOrderP(iterator.object_ptr() - data_ptr_, src_begin.object_ptr(), src_end.object_ptr()));
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE IteratorType Inserts(const IteratorType& iterator,
                                               const ConstIteratorType& src_begin, 
                                               const ConstIteratorType& src_end) {
        return IteratorType(
            InsertsOrderP(iterator.object_ptr() - data_ptr_, src_begin.object_ptr(), src_end.object_ptr()));
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE IteratorType Inserts(const IteratorType& iterator,
                                               const ReverseIteratorType& src_begin, 
                                               const ReverseIteratorType& src_end) {
        return IteratorType(
            InsertsReverseP(iterator.object_ptr() - data_ptr_, src_begin.object_ptr(), src_end.object_ptr()));
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE IteratorType Inserts(const IteratorType& iterator,
                                               const ConstReverseIteratorType& src_begin, 
                                               const ConstReverseIteratorType& src_end) {
        return IteratorType(
            InsertsReverseP(iterator.object_ptr() - data_ptr_, src_begin.object_ptr(), src_end.object_ptr()));
    }

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE ReverseIteratorType Inserts(const ReverseIteratorType& iterator,
                                                      const IteratorType& src_begin, 
                                                      const IteratorType& src_end) {
        return ReverseIteratorType(
            InsertsReverseP(iterator.object_ptr() - data_ptr_ + 1,
                            src_end.object_ptr() - 1, src_begin.object_ptr() - 1) + ((src_end - src_begin) - 1));
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE ReverseIteratorType Inserts(const ReverseIteratorType& iterator,
                                                      const ConstIteratorType& src_begin, 
                                                      const ConstIteratorType& src_end) {
        return ReverseIteratorType(
            InsertsReverseP(iterator.object_ptr() - data_ptr_ + 1,
                            src_end.object_ptr() - 1, src_begin.object_ptr() - 1) + ((src_end - src_begin) - 1));
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE ReverseIteratorType Inserts(const ReverseIteratorType& iterator,
                                                      const ReverseIteratorType& src_begin, 
                                                      const ReverseIteratorType& src_end) {
        return ReverseIteratorType(
            InsertsOrderP(iterator.object_ptr() - data_ptr_ + 1,
                          src_end.object_ptr() + 1, src_begin.object_ptr() + 1) + ((src_end - src_begin) - 1));
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE ReverseIteratorType Inserts(const ReverseIteratorType& iterator,
                                                      const ConstReverseIteratorType& src_begin,
                                                      const ConstReverseIteratorType& src_end) {
        return ReverseIteratorType(
            InsertsOrderP(iterator.object_ptr() - data_ptr_ + 1,
                          src_end.object_ptr() + 1, src_begin.object_ptr() + 1) + ((src_end - src_begin) - 1));
    }

    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    FORCEINLINE IteratorType Erase(IndexType index) {
        return IteratorType(EraseP(data_ptr_ + index));
    }
    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE IteratorType Erase(const IteratorType& iterator) {
        return IteratorType(EraseP(iterator.object_ptr()));
    }
    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE ReverseIteratorType Erase(const ReverseIteratorType& iterator) {
        return ReverseIteratorType(EraseP(iterator.object_ptr()) - 1);
    }

    /*
        Erases the num of objects that starts at the given index.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE IteratorType Erases(IndexType index, IndexType num) {
        return IteratorType(ErasesP(data_ptr_ + index, data_ptr_ + (index + num)));
    }
    /*
        Erases the num of objects that starts at the given iterator.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE IteratorType Erases(const IteratorType& iterator, IndexType num) {
        return IteratorType(ErasesP(iterator.object_ptr(), iterator.object_ptr() + num));
    }
    /*
        Erases the num of objects that starts at the given iterator.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE ReverseIteratorType Erases(const ReverseIteratorType& iterator, IndexType num) {
        return ReverseIteratorType(ErasesP(iterator.object_ptr() - (num - 1), iterator.object_ptr() + 1) - 1);
    }
    /*
        Erases the object between begin and end, involves begin, but don't involves end.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE IteratorType Erases(const IteratorType& begin, const IteratorType& end) {
        return IteratorType(ErasesP(begin.object_ptr(), end.object_ptr()));
    }
    /*
        Erases the object between begin and end, involves begin, but don't involves end.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE ReverseIteratorType Erases(const ReverseIteratorType& begin, 
                                                     const ReverseIteratorType& end) {
        return ReverseIteratorType(ErasesP(end.object_ptr() + 1, begin.object_ptr() + 1) - 1);
    }

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    FORCEINLINE Void Emplace(IndexType index, ArgsType&&... args) {
        EmplaceP(data_ptr_ + index, std::forward<ArgsType>(args)...);
    }
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    FORCEINLINE Void Emplace(const IteratorType& iterator, ArgsType&&... args) {
        EmplaceP(iterator.object_ptr(), std::forward<ArgsType>(args)...);
    }
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    FORCEINLINE Void Emplace(const ReverseIteratorType& iterator, ArgsType&&... args) {
        EmplaceP(iterator.object_ptr(), std::forward<ArgsType>(args)...);
    }

    /*
        Construct the vector by filling it with the given amount of objects.
        The object is constructed by the arguements.
        If kIfUnique is false and no arguements, will only add the 
        size of the vector.
    */
    template<typename... ArgsType>
    Void Assign(IndexType num, ArgsType&&... args) noexcept;
    /*
        Construct the vector by filling it objects between the iterators.
    */
    FORCEINLINE Void Assign(const IteratorType& begin, const IteratorType& end) noexcept {
        AssignOrderP(begin.object_ptr(), end.object_ptr());
    }
    /*
        Construct the vector by filling it objects between the iterators.
    */
    FORCEINLINE Void Assign(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept {
        AssignOrderP(begin.object_ptr(), end.object_ptr());
    }
    /*
        Construct the vector by filling it objects between the iterators.
    */
    FORCEINLINE Void Assign(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept {
        AssignReverseP(begin.object_ptr(), end.object_ptr());
    }
    /*
        Construct the vector by filling it objects between the iterators.
    */
    FORCEINLINE Void Assign(const ConstReverseIteratorType& begin, const ConstReverseIteratorType& end) noexcept {
        AssignReverseP(begin.object_ptr(), end.object_ptr());
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
    Void Sort(const IteratorType& begin, const IteratorType& end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const IteratorType& begin, const IteratorType& end, CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const ReverseIteratorType& begin, const ReverseIteratorType& end, 
              CompareFunction&& compare_function) noexcept;

    /*
        Destroys all the objects in the vector, does not release the memory.
    */
    Void Clear() noexcept;
    /*
        Destroys all the objects in the vector, release the memory.
    */
    FORCEINLINE Void Destroy();

protected:
    using SuperType = ZObject;

private:
    /*
        Creates an object at the certain place. Will call the Constrctor if needed.
    */
    template<typename... ArgsType>
    FORCEINLINE static Void CreateObjectP(ObjectType* object_ptr, ArgsType&&... args);

    /*
        Destroys an object at the certain place. Will Call Destrctor if needed.
    */
    FORCEINLINE static Void DestroyObjectP(ObjectType* object_ptr);

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the Constrctor if needed.
    */
    template<typename... ArgsType>
    inline static Void CreateObjectsP(ObjectType* begin_ptr, ObjectType* end_ptr, ArgsType&&... args) noexcept;

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the copy constructor.
    */
    inline static Void CreateAndCopyObjectsP(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
                                             const ObjectType* src_end_ptr) noexcept;

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the copy constructor.
    */
    static Void CreateAndCopyObjectsReverseP(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
                                             const ObjectType* src_end_ptr) noexcept;

    /*
        Copy objects by the given pointer. Will call the copy assignment operator
        if this object class's member kIfUnique is true.
    */
    inline static Void CopyObjectsP(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
                                    const ObjectType* src_end_ptr) noexcept;

    /*
        Copy objects by the given pointer. Will call the copy assignment operator
        if this object class's member kIfUnique is true.
    */
    static Void CopyObjectsReverseP(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
                                    const ObjectType* src_end_ptr) noexcept;

    /*
        Destroy the objects by the given arguements([begin, end)). 
        Will call the destrctor if this object class's member kIfUnique is true.
    */
    inline static Void DestroyObjectsP(ObjectType* begin_ptr, ObjectType* end_ptr) noexcept;

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
        Constructor with two order iterators.
    */
    FORCEINLINE Void ZVectorOrderP(const ObjectType* begin_ptr, const ObjectType* end_ptr) noexcept;

    /*
        Constructor with two reverse iterators.
    */
    FORCEINLINE Void ZVectorReverseP(const ObjectType* begin_ptr, const ObjectType* end_ptr) noexcept;

    /*
        Container copy function.
    */
    Void CopyP(const ZVector& vector) noexcept;

    /*
        Container move function.
    */
    FORCEINLINE Void MoveP(ZVector&& vector);

    /*
        Makes a copy of the objects between the pointers and push them to the
        back of the vector.
    */
    Void PushBacksOrderP(const ObjectType* src_begin_ptr, const ObjectType* src_end_ptr) noexcept;

    /*
        Makes a copy of the objects between the pointers and push them to the
        back of the vector.
    */
    Void PushBacksReverseP(const ObjectType* src_begin_ptr, const ObjectType* src_end_ptr) noexcept;

    /*
        Inserts before the index. Returns the pointer that points at the newest object.
    */
    template<typename... ArgsType>
    NODISCARD ObjectType* InsertP(IndexType index, ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the pointer that points at the first new object.
    */
    template<typename... ArgsType>
    NODISCARD ObjectType* InsertsP(IndexType index, IndexType num, ArgsType&&... args) noexcept;

    /*
        Makes a copy of the objects between the pointers and insert them to the
        given place. Returns the pointer that points at the first new object.
    */
    NODISCARD ObjectType* InsertsOrderP(IndexType index,
                                        const ObjectType* src_begin_ptr, 
                                        const ObjectType* src_end_ptr) noexcept;

    /*
        Makes a copy of the objects between the pointers and insert them to the
        given place. Returns the pointer that points at the first new object.
    */
    NODISCARD ObjectType* InsertsReverseP(IndexType index,
                                          const ObjectType* src_begin_ptr, 
                                          const ObjectType* src_end_ptr) noexcept;

    /*
        Erases the object by the index.
        Returns the pointer that points at the next object.
    */
    NODISCARD inline ObjectType* EraseP(ObjectType* object_ptr) noexcept;

    /*
        Erases the num of objects that starts at the given index.
        Returns the pointer that points at the next object.
    */
    NODISCARD inline ObjectType* ErasesP(ObjectType* begin_ptr, ObjectType* end_ptr) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    inline Void EmplaceP(ObjectType* object_ptr, ArgsType&&... args) noexcept;

    /*
        Construct the vector by filling it objects between the pointers.
    */
    Void AssignOrderP(ObjectType* begin_ptr, ObjectType* end_ptr) noexcept;

    /*
        Construct the vector by filling it objects between the pointers.
    */
    Void AssignReverseP(ObjectType* begin_ptr, ObjectType* end_ptr) noexcept;

    ObjectType* data_ptr_;
    IndexType capacity_;

    IndexType size_;
};

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector() noexcept
    : SuperType()
    , data_ptr_(nullptr)
    , capacity_(0)
    , size_(0)
{}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector(IndexType capacity) noexcept
    : SuperType()
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainerP(capacity);
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
ZVector<ObjectType, kIfUnique>::ZVector(IndexType capacity, ArgsType&&... args) noexcept
    : SuperType() 
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainerP(capacity);
    CreateObjectsP(data_ptr_, data_ptr_ + capacity, std::forward<ArgsType>(args)...);
    size_ = capacity;
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector(const ZVector& vector) noexcept 
    : SuperType(vector)
{
    DEBUG(&vector == this, "The source and the target of the copy is the same!");
    CreateContainerP(vector.size_);
    CreateAndCopyObjectsP(data_ptr_, vector.data_ptr_, vector.data_ptr_ + vector.size_);
    size_ = vector.size_;
} 

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector(ZVector&& vector) noexcept 
    : SuperType(std::forward<ZVector>(vector))
{
    DEBUG(&vector == this, "The source and the target of the copy is the same!");
    MoveP(std::forward<ZVector>(vector));
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>& ZVector<ObjectType, kIfUnique>::operator=(const ZVector& vector) noexcept {
    DEBUG(&vector == this, "The source and the target of the copy is the same!");
    SuperType::operator=(vector);
    CopyP(vector);
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>& ZVector<ObjectType, kIfUnique>::operator=(ZVector&& vector) noexcept {
    DEBUG(&vector == this, "The source and the target of the copy is the same!");
    SuperType::operator=(std::forward<ZVector>(vector));
    DestroyObjectsP(data_ptr_, data_ptr_ + size_);
    MoveP(std::forward<ZVector>(vector));
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::~ZVector() noexcept {
    DestroyContainerP();
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
inline Void ZVector<ObjectType, kIfUnique>::Resize(IndexType size, ArgsType&&... args) noexcept {
    DEBUG(size < 0, "Negaive size is not valid!");
    if (size_ < size) {
        if (size > capacity_) {
            ExtendContainerP(size);
        }
        CreateObjectsP(data_ptr_ + size_, data_ptr_ + size, std::forward<ArgsType>(args)...);
    }
    else {
        DestroyObjectsP(data_ptr_ + size, data_ptr_ + size_);
    }
    size_ = size;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::Reserve(IndexType capacity) {
    if (capacity > capacity_) {
        ExtendContainerP(capacity);
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::ShrinkToFit() {
    ShrinkContainerP();
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::PopBack() {
    DEBUG(size_ == 0, "No existing object to pop!");
    DestroyObjectP(data_ptr_ + size_--);
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::PopBack(ObjectType* object_ptr) {
    DEBUG(size_ == 0, "No existing object to pop!");
    *object_ptr = std::move(data_ptr_[size_--]);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfUnique>::PushBack(ArgsType&&... args) noexcept {
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjectP(data_ptr_ + size_, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
inline Void ZVector<ObjectType, kIfUnique>::PushBacks(IndexType num, ArgsType&&... args) noexcept {
    DEBUG(num < 0, "Negative pushing num not valid!");
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjectsP(data_ptr_ + size_, data_ptr_ + new_size, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
inline Void ZVector<ObjectType, kIfUnique>::EmplaceBack(ArgsType&&... args) noexcept {
    DEBUG(size_ == 0, "No existing object to emplace!");
    DestroyObjectP(data_ptr_ + (size_ - 1));
    CreateObjectP(data_ptr_ + (size_ - 1), std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfUnique>::Assign(IndexType num, ArgsType&&... args) noexcept {
    DEBUG(num < 0, "Negative assign num not valid!");
    IndexType new_size = num;
    if (new_size > capacity_) {
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    DestroyObjectsP(data_ptr_, data_ptr_ + size_);
    CreateObjectsP(data_ptr_, data_ptr_ + num, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::Clear() noexcept {
    DestroyObjectsP(data_ptr_, data_ptr_ + size_);
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::Destroy() {
    DestroyContainerP();
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CreateObjectP(ObjectType* object_ptr, ArgsType&&... args) {
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
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::DestroyObjectP(ObjectType* object_ptr) {
    if constexpr (kIfUnique) {
        (*object_ptr).~ObjectType();
    }
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
inline Void ZVector<ObjectType, kIfUnique>::CreateObjectsP(ObjectType* begin_ptr, ObjectType* end_ptr,
                                                          ArgsType&&... args) noexcept {
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
        if constexpr (sizeof...(args) != 0) {
            while (begin_ptr < end_ptr) {
                new(reinterpret_cast<Void*>(begin_ptr++)) ObjectType(std::forward<ArgsType>(args)...);
            }
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
inline Void ZVector<ObjectType, kIfUnique>::CreateAndCopyObjectsP(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
                                                                  const ObjectType* src_end_ptr) noexcept {
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
Void ZVector<ObjectType, kIfUnique>::CreateAndCopyObjectsReverseP(ObjectType* dst_ptr, const ObjectType* src_begin_ptr, 
                                                                  const ObjectType* src_end_ptr) noexcept {
    while (src_begin_ptr > src_end_ptr) {
        new(reinterpret_cast<Void*>(dst_ptr)) ObjectType(*src_begin_ptr);
        ++dst_ptr;
        --src_begin_ptr;
    }
}

template<typename ObjectType, Bool kIfUnique>
inline Void ZVector<ObjectType, kIfUnique>::CopyObjectsP(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
                                                         const ObjectType* src_end_ptr) noexcept {
    if constexpr (kIfUnique) {
        while (src_begin_ptr < src_end_ptr) {
            *dst_ptr = *src_begin_ptr;
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
Void ZVector<ObjectType, kIfUnique>::CopyObjectsReverseP(ObjectType* dst_ptr,
                                                         const ObjectType* src_begin_ptr,
                                                         const ObjectType* src_end_ptr) noexcept {
    while (src_begin_ptr > src_end_ptr) {
        *dst_ptr = *src_begin_ptr;
        ++dst_ptr;
        --src_begin_ptr;
    }
}

template<typename ObjectType, Bool kIfUnique>
inline Void ZVector<ObjectType, kIfUnique>::DestroyObjectsP(ObjectType* begin_ptr, ObjectType* end_ptr) noexcept {
    if constexpr (kIfUnique) {
        while (begin_ptr < end_ptr) {
            begin_ptr->~ObjectType();
            ++begin_ptr;
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::CreateContainerP(IndexType capacity) noexcept {
    MemoryType need_memory_size = capacity * sizeof(ObjectType);
    MemoryType apply_mrmory_size;
    data_ptr_ = reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));
    capacity_ = apply_mrmory_size / sizeof(ObjectType);
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::ExtendContainerP(IndexType capacity) noexcept {
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
Void ZVector<ObjectType, kIfUnique>::ShrinkContainerP() noexcept {
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
Void ZVector<ObjectType, kIfUnique>::DestroyContainerP() noexcept {
    DestroyObjectsP(data_ptr_, data_ptr_ + size_);
    memory_pool::ReleaseMemory(reinterpret_cast<Void*>(data_ptr_));
    memset(reinterpret_cast<Void*>(this), 0, sizeof(ZVector));
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::ZVectorOrderP(const ObjectType* begin_ptr,
                                                               const ObjectType* end_ptr) noexcept {
    DEBUG(begin_ptr > end_ptr, "Begin pointer after end pointer!");
    size_ = end_ptr - begin_ptr;
    CreateContainerP(size_);
    CreateAndCopyObjectsP(data_ptr_, begin_ptr, end_ptr);
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::ZVectorReverseP(const ObjectType* begin_ptr,
                                                                 const ObjectType* end_ptr) noexcept {
    DEBUG(begin_ptr < end_ptr, "Begin pointer after end pointer!");
    size_ = begin_ptr - end_ptr;
    CreateContainerP(size_);
    CreateAndCopyObjectsReverseP(data_ptr_, begin_ptr, end_ptr);
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::CopyP(const ZVector& vector) noexcept {
    if (vector.size_ > capacity_) {
        ExtendContainerP(vector.size_);
    }
    if constexpr (kIfUnique) {
        if (vector.size_ > size_) {
            CopyObjectsP(data_ptr_, vector.data_ptr_, vector.data_ptr_ + size_);
            CreateAndCopyObjectsP(data_ptr_ + size_, vector.data_ptr_ + size_, vector.data_ptr_ + vector.size_);
        }
        else {
            CopyObjectsP(data_ptr_, vector.data_ptr_, vector.data_ptr_ + vector.size_);
            DestroyObjectsP(data_ptr_ + vector.size_, data_ptr_ + size_);
        }
    }
    else {
        CopyObjectsP(data_ptr_, vector.data_ptr_, vector.data_ptr_ + vector.size_);
    }
    size_ = vector.size_;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::MoveP(ZVector&& vector) {
    memcpy(reinterpret_cast<Void*>(this), reinterpret_cast<Void*>(&vector), sizeof(ZVector));
    memset(reinterpret_cast<Void*>(&vector), 0, sizeof(ZVector));
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::PushBacksOrderP(const ObjectType* src_begin_ptr, 
                                                     const ObjectType* src_end_ptr) noexcept {
    DEBUG(src_begin_ptr > src_end_ptr, "Begin pointer after end pointer!");
    IndexType new_size = size_ + static_cast<IndexType>(src_end_ptr - src_begin_ptr);
    if (new_size > capacity_) {
        if (src_begin_ptr >= data_ptr_ && src_begin_ptr <= (data_ptr_ + size_)) {
            IndexType begin_index = static_cast<IndexType>(src_begin_ptr - data_ptr_);
            IndexType end_index = src_end_ptr - data_ptr_;
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
            src_begin_ptr = data_ptr_ + begin_index;
            src_end_ptr = data_ptr_ + end_index;
        }
        else {
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
        }
    }
    CreateAndCopyObjectsP(data_ptr_ + size_, src_begin_ptr, src_end_ptr);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::PushBacksReverseP(const ObjectType* src_begin_ptr, 
                                                       const ObjectType* src_end_ptr) noexcept {
    DEBUG(src_begin_ptr < src_end_ptr, "Begin pointer after end pointer!");
    IndexType new_size = size_ + static_cast<IndexType>(src_begin_ptr - src_end_ptr);
    if (new_size > capacity_) {
        if (src_begin_ptr >= data_ptr_ && src_begin_ptr <= (data_ptr_ + size_)) {
            IndexType begin_index = static_cast<IndexType>(src_begin_ptr - data_ptr_);
            IndexType end_index = src_end_ptr - data_ptr_;
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
            src_begin_ptr = data_ptr_ + begin_index;
            src_end_ptr = data_ptr_ + end_index;
        }
        else {
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
        }
    }
    CreateAndCopyObjectsReverseP(data_ptr_ + size_, src_begin_ptr, src_end_ptr);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
NODISCARD ObjectType* ZVector<ObjectType, kIfUnique>::InsertP(IndexType index, ArgsType&&... args) noexcept {
    DEBUG(index < 0 || index > size_, "Insert index out of bounds!");
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + 1)), reinterpret_cast<Void*>(data_ptr_ + index),
        (size_ - index) * sizeof(ObjectType));
    CreateObjectP(data_ptr_ + index, std::forward<ArgsType>(args)...);
    size_ = new_size;
    return data_ptr_ + index;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
NODISCARD ObjectType* ZVector<ObjectType, kIfUnique>::InsertsP(IndexType index, IndexType num,
                                                               ArgsType&&... args) noexcept {
    DEBUG(index < 0 || index > size_, "Insert index out of bounds!");
    DEBUG(num < 0, "Negative insert num not valid!");
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
        (size_ - index) * sizeof(ObjectType));
    CreateObjectsP(data_ptr_ + index, data_ptr_ + (index + num), std::forward<ArgsType>(args)...);
    size_ = new_size;
    return data_ptr_ + index;
}

template<typename ObjectType, Bool kIfUnique>
NODISCARD ObjectType* ZVector<ObjectType, kIfUnique>::InsertsOrderP(IndexType index,
                                                                    const ObjectType* src_begin_ptr, 
                                                                    const ObjectType* src_end_ptr) noexcept {
    DEBUG(index < 0 || index > size_, "Insert index out of bounds!");
    DEBUG(src_begin_ptr > src_end_ptr, "Begin pointer after end pointer!");
    IndexType num = static_cast<IndexType>(src_end_ptr - src_begin_ptr);
    IndexType new_size = size_ + num;
    if (src_begin_ptr >= data_ptr_ && src_begin_ptr <= (data_ptr_ + size_)) {
        IndexType begin_index = static_cast<IndexType>(src_begin_ptr - data_ptr_);
        IndexType end_index = static_cast<IndexType>(src_end_ptr - data_ptr_);
        if (new_size > capacity_) {
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
        }
        memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
                (size_ - index) * sizeof(ObjectType));
        if (index >= end_index) {
            src_begin_ptr = data_ptr_ + begin_index;
            src_end_ptr = data_ptr_ + end_index;
            CreateAndCopyObjectsP(data_ptr_ + index, src_begin_ptr, src_end_ptr);
        }
        else if (index < begin_index) {
            src_begin_ptr = data_ptr_ + begin_index + num;
            src_end_ptr = data_ptr_ + end_index + num;
            CreateAndCopyObjectsP(data_ptr_ + index, src_begin_ptr, src_end_ptr);
        }
        else {
            IndexType part_1_num = index - begin_index;
            IndexType part_2_num = num - part_1_num;
            ObjectType* dst_ptr = data_ptr_ + index;
            src_begin_ptr = data_ptr_ + begin_index;
            src_end_ptr = src_begin_ptr + part_1_num;
            CreateAndCopyObjectsP(dst_ptr, src_begin_ptr, src_end_ptr);
            src_begin_ptr = dst_ptr + num;
            src_end_ptr = src_begin_ptr + part_2_num;
            CreateAndCopyObjectsP(dst_ptr + part_1_num, src_begin_ptr, src_end_ptr);
        }
    }
    else {
        if (new_size > capacity_) {
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
        }
        memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
                (size_ - index) * sizeof(ObjectType));
        CreateAndCopyObjectsP(data_ptr_ + index, src_begin_ptr, src_end_ptr);
    }
    size_ = new_size;
    return data_ptr_ + index;
}

template<typename ObjectType, Bool kIfUnique>
NODISCARD ObjectType* ZVector<ObjectType, kIfUnique>::InsertsReverseP(IndexType index,
                                                                      const ObjectType* src_begin_ptr, 
                                                                      const ObjectType* src_end_ptr) noexcept {
    DEBUG(index < 0 || index > size_, "Insert index out of bounds!");
    DEBUG(src_begin_ptr < src_end_ptr, "Begin pointer after end pointer!");
    IndexType num = static_cast<IndexType>(src_begin_ptr - src_end_ptr);
    IndexType new_size = size_ + num;
    if (src_begin_ptr >= data_ptr_ && src_begin_ptr <= (data_ptr_ + size_)) {
        IndexType begin_index = static_cast<IndexType>(src_begin_ptr - data_ptr_);
        IndexType end_index = static_cast<IndexType>(src_end_ptr - data_ptr_);
        if (new_size > capacity_) {
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
        }
        memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
                (size_ - index) * sizeof(ObjectType));
        if (index > begin_index) {
            src_begin_ptr = data_ptr_ + begin_index;
            src_end_ptr = data_ptr_ + end_index;
            CreateAndCopyObjectsReverseP(data_ptr_ + index, src_begin_ptr, src_end_ptr);
        }
        else if (index <= end_index) {
            src_begin_ptr = data_ptr_ + begin_index + num;
            src_end_ptr = data_ptr_ + end_index + num;
            CreateAndCopyObjectsReverseP(data_ptr_ + index, src_begin_ptr, src_end_ptr);
        }
        else {
            IndexType part_1_num = begin_index - index + 1;
            IndexType part_2_num = num - part_1_num;
            ObjectType* dst_ptr = data_ptr_ + index;
            src_begin_ptr = data_ptr_ + begin_index + num;
            src_end_ptr = src_begin_ptr - part_1_num;
            CreateAndCopyObjectsReverseP(dst_ptr, src_begin_ptr, src_end_ptr);
            src_begin_ptr = dst_ptr - 1;
            src_end_ptr = src_begin_ptr - part_2_num;
            CreateAndCopyObjectsReverseP(dst_ptr + part_1_num, src_begin_ptr, src_end_ptr);
        }
    }
    else {
        if (new_size > capacity_) {
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
        }
        memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
                (size_ - index) * sizeof(ObjectType));
        CreateAndCopyObjectsReverseP(data_ptr_ + index, src_begin_ptr, src_end_ptr);
    }
    size_ = new_size;
    return data_ptr_ + index;
}

template<typename ObjectType, Bool kIfUnique>
NODISCARD inline ObjectType* ZVector<ObjectType, kIfUnique>::EraseP(ObjectType* object_ptr) noexcept {
    DEBUG(object_ptr < data_ptr_ || object_ptr >= data_ptr_ + size_, "Erase index out of bounds!");
    DestroyObjectP(object_ptr);
    memmove(reinterpret_cast<Void*>(object_ptr), reinterpret_cast<Void*>(object_ptr + 1),
            (size_ - static_cast<IndexType>(object_ptr - data_ptr_) - 1) * sizeof(ObjectType));
    --size_;
    return object_ptr;
}

template<typename ObjectType, Bool kIfUnique>
NODISCARD inline ObjectType* ZVector<ObjectType, kIfUnique>::ErasesP(ObjectType* begin_ptr,
                                                                     ObjectType* end_ptr) noexcept {
    DEBUG(begin_ptr < data_ptr_ || begin_ptr >= data_ptr_ + size_, "Erase index out of bounds!");
    DEBUG(end_ptr < data_ptr_ || end_ptr > data_ptr_ + size_, "Erase index out of bounds!");
    DEBUG(begin_ptr > end_ptr, "Begin pointer after end pointer!");
    DestroyObjectsP(begin_ptr, end_ptr);
    memmove(reinterpret_cast<Void*>(begin_ptr), reinterpret_cast<Void*>(end_ptr),
            (size_ - static_cast<IndexType>(end_ptr - data_ptr_)) * sizeof(ObjectType));
    size_ -= static_cast<IndexType>(end_ptr - begin_ptr);
    return begin_ptr;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
inline Void ZVector<ObjectType, kIfUnique>::EmplaceP(ObjectType* object_ptr, ArgsType&&... args) noexcept {
    DEBUG(object_ptr < data_ptr_ || object_ptr >= data_ptr_ + size_, "Emplace index out of bounds!");
    DestroyObjectP(object_ptr);
    CreateObjectP(object_ptr, std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::AssignOrderP(ObjectType* begin_ptr,
                                                  ObjectType* end_ptr) noexcept {
    DEBUG(begin_ptr > end_ptr, "Begin pointer after end pointer!");
    IndexType new_size = static_cast<IndexType>(end_ptr - begin_ptr);
    if (new_size > capacity_) {
        ExtendContainerP(new_size);
    }
    if constexpr (kIfUnique) {
        //If the pointer is from this.
        if (begin_ptr >= data_ptr_ && begin_ptr < (data_ptr_ + size_)) {
            DestroyObjectsP(data_ptr_, begin_ptr);
            DestroyObjectsP(end_ptr, data_ptr_ + size_);
            //Move the objects to the front.
            memmove(reinterpret_cast<Void*>(data_ptr_), reinterpret_cast<Void*>(begin_ptr),
                    new_size * sizeof(ObjectType));
        }
        else {
            if (new_size > size_) {
                CopyObjectsP(data_ptr_, begin_ptr, begin_ptr + size_);
                CreateAndCopyObjectsP(data_ptr_ + size_, begin_ptr + size_, end_ptr);
            }
            else {
                CopyObjectsP(data_ptr_, begin_ptr, end_ptr);
                DestroyObjectsP(data_ptr_ + new_size, data_ptr_ + size_);
            }
        }
    }
    else {
        CopyObjectsP(data_ptr_, begin_ptr, end_ptr);
    }
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::AssignReverseP(ObjectType* begin_ptr,
                                                    ObjectType* end_ptr) noexcept {
    DEBUG(begin_ptr < end_ptr, "Begin pointer after end pointer!");
    IndexType new_size = begin_ptr - end_ptr;
    if (new_size > capacity_) {
        ExtendContainerP(new_size);
    }
    if constexpr (kIfUnique) {
        //If the pointer is from this.
        if (begin_ptr >= data_ptr_ && begin_ptr < (data_ptr_ + size_)) {
            DestroyObjectsP(data_ptr_, end_ptr + 1);
            DestroyObjectsP(begin_ptr + 1, data_ptr_ + size_);
            //Reverses the objects.
            ObjectType* begin_ptr = end_ptr + 1;
            ObjectType* end_ptr = begin_ptr;
            while (begin_ptr < end_ptr) {
                Swap(begin_ptr, end_ptr);
                ++begin_ptr;
                --end_ptr;
            }
            //Move the objects to the first front.
            memmove(reinterpret_cast<Void*>(data_ptr_), reinterpret_cast<Void*>(end_ptr + 1),
                new_size * sizeof(ObjectType));
        }
        else {
            if (new_size > size_) {
                CopyObjectsReverseP(data_ptr_, begin_ptr, begin_ptr - size_);
                CreateAndCopyObjectsReverseP(data_ptr_ + size_, begin_ptr - size_, end_ptr);
            }
            else {
                CopyObjectsReverseP(data_ptr_, begin_ptr, end_ptr);
                DestroyObjectsP(data_ptr_ + new_size, data_ptr_ + size_);
            }
        }
    }
    else {
        CopyObjectsReverseP(data_ptr_, begin_ptr, end_ptr);
    }
    size_ = new_size;
}

}//zengine

#endif // !Z_CORE_Z_VECTOR_H_