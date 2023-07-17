#ifndef Z_CORE_Z_VECTOR_H_
#define Z_CORE_Z_VECTOR_H_

#include "internal/drive.h"

#include "m_error_message.h"
#include "z_object.h"

namespace zengine {

namespace internal {

template<typename ObjectType>
class VectorIteratorBase {
public:
    FORCEINLINE VectorIteratorBase(ObjectType* object_ptr) : object_ptr_(object_ptr) {}
    FORCEINLINE VectorIteratorBase(const VectorIteratorBase& iterator) : object_ptr_(iterator.object_ptr_) {}
    FORCEINLINE VectorIteratorBase(const VectorIteratorBase&& iterator) : object_ptr_(iterator.object_ptr_) {
        iterator.MoveDestroy();
    }

    FORCEINLINE VectorIteratorBase& operator=(ObjectType* object_ptr) {
        object_ptr_ = object_ptr;
        return *this;
    }
    FORCEINLINE VectorIteratorBase& operator=(const VectorIteratorBase& iterator) {
        object_ptr_ = iterator.object_ptr_;
        return *this;
    }
    FORCEINLINE VectorIteratorBase& operator=(VectorIteratorBase&& iterator) {
        object_ptr_ = iterator.object_ptr_;
        iterator.MoveDestroy();
        return *this;
    }

    NODISCARD FORCEINLINE const Bool operator==(const VectorIteratorBase& iterator) const {
        return object_ptr_ == iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator!=(const VectorIteratorBase& iterator) const {
        return object_ptr_ != iterator.object_ptr_;
    }

    FORCEINLINE ~VectorIteratorBase() {}

    FORCEINLINE Void MoveDestroy() { object_ptr_ = nullptr; }

protected:
     ObjectType* object_ptr_;
};

template<typename ObjectType>
class VectorIterator : public VectorIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index) const { return SuperType::object_ptr_[index]; }
    NODISCARD FORCEINLINE ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE VectorIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE VectorIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }

    FORCEINLINE VectorIterator& operator++() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE VectorIterator& operator--() {
        --SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE VectorIterator operator+(const IndexType data_num) const {
        return VectorIterator(SuperType::object_ptr_ + data_num);
    }
    NODISCARD FORCEINLINE VectorIterator operator-(const IndexType data_num) const {
        return VectorIterator(SuperType::object_ptr_ - data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const VectorIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const VectorIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const VectorIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const VectorIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const VectorIterator& iterator) const {
        return static_cast<IndexType>(SuperType::object_ptr_ - iterator.SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = VectorIteratorBase<ObjectType>;
};

template<typename ObjectType>
class VectorConstIterator : public VectorIteratorBase<ObjectType> {
public:
    FORCEINLINE VectorConstIterator(const ObjectType* object_ptr) : SuperType(const_cast<ObjectType*>(object_ptr)) {}

    FORCEINLINE VectorConstIterator& operator=(const ObjectType* object_ptr) {
        SuperType::object_ptr_ = const_cast<ObjectType*>(object_ptr);
        return *this;
    }

    NODISCARD FORCEINLINE const ObjectType& operator[](const IndexType index) const { 
        return SuperType::object_ptr_[-index]; 
    }
    NODISCARD FORCEINLINE const ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE VectorConstIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE VectorConstIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }

    FORCEINLINE VectorConstIterator& operator++() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE VectorConstIterator& operator--() {
        --SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE VectorConstIterator operator+(const IndexType data_num) const {
        return VectorConstIterator(SuperType::object_ptr_ + data_num);
    }
    NODISCARD FORCEINLINE VectorConstIterator operator-(const IndexType data_num) const {
        return VectorConstIterator(SuperType::object_ptr_ - data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const VectorConstIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const VectorConstIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const VectorConstIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const VectorConstIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const VectorConstIterator& iterator) const {
        return static_cast<IndexType>(SuperType::object_ptr_ - iterator.SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE const ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = VectorIteratorBase<ObjectType>;
};

template<typename ObjectType>
class VectorReverseIterator : public VectorIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index) const { return SuperType::object_ptr_[-index]; }
    NODISCARD FORCEINLINE ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE VectorReverseIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE VectorReverseIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }

    FORCEINLINE VectorReverseIterator& operator++() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE VectorReverseIterator& operator--() {
        ++SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE VectorReverseIterator operator+(const IndexType data_num) const {
        return VectorReverseIterator(SuperType::object_ptr_ - data_num);
    }
    NODISCARD FORCEINLINE VectorReverseIterator operator-(const IndexType data_num) const {
        return VectorReverseIterator(SuperType::object_ptr_ + data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const VectorReverseIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const VectorReverseIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const VectorReverseIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const VectorReverseIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const VectorReverseIterator& iterator) const {
        return static_cast<IndexType>(iterator.SuperType::object_ptr_ - SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = VectorIteratorBase<ObjectType>;
};

template<typename ObjectType>
class VectorConstReverseIterator : public VectorIteratorBase<ObjectType> {
public:
    FORCEINLINE VectorConstReverseIterator(const ObjectType* object_ptr)
        : SuperType(const_cast<ObjectType*>(object_ptr)) {}

    FORCEINLINE VectorConstReverseIterator& operator=(const ObjectType* object_ptr) {
        SuperType::object_ptr_ = const_cast<ObjectType*>(object_ptr);
        return *this;
    }

    NODISCARD FORCEINLINE const ObjectType& operator[](const IndexType index) const { 
        return SuperType::object_ptr_[-index]; 
    }
    NODISCARD FORCEINLINE const ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE VectorConstReverseIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE VectorConstReverseIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }

    FORCEINLINE VectorConstReverseIterator& operator++() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE VectorConstReverseIterator& operator--() {
        ++SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE VectorConstReverseIterator operator+(const IndexType data_num) const {
        return VectorConstReverseIterator(SuperType::object_ptr_ - data_num);
    }
    NODISCARD FORCEINLINE VectorConstReverseIterator operator-(const IndexType data_num) const {
        return VectorConstReverseIterator(SuperType::object_ptr_ + data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const VectorConstReverseIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const VectorConstReverseIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const VectorConstReverseIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const VectorConstReverseIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const VectorConstReverseIterator& iterator) const {
        return static_cast<IndexType>(iterator.SuperType::object_ptr_ - SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE const ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = VectorIteratorBase<ObjectType>;
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
    using IteratorType = internal::VectorIterator<ObjectType>;
    using ConstIteratorType = internal::VectorConstIterator<ObjectType>;
    using ReverseIteratorType = internal::VectorReverseIterator<ObjectType>;
    using ConstReverseIteratorType = internal::VectorConstReverseIterator<ObjectType>;

    ZVector() noexcept;
    ZVector(const IndexType capacity) noexcept;
    /*
        Fills the container by the object constructed by the arguements.
    */
    template<typename... ArgsType>
    ZVector(const IndexType capacity, ArgsType&&... args) noexcept;    
    ZVector(const IteratorType& begin, const IteratorType& end) noexcept;
    ZVector(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept;
    ZVector(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;
    ZVector(const ConstReverseIteratorType& begin, const ConstReverseIteratorType& end) noexcept;
    ZVector(const ZVector& vector) noexcept;
    ZVector(ZVector&& vector) noexcept;

    ZVector& operator=(const ZVector& vector) noexcept;
    ZVector& operator=(ZVector&& vector) noexcept;

    NODISCARD FORCEINLINE const Bool operator==(const ZVector& vector) noexcept { 
        return data_ptr_ == vector.data_ptr_; 
    }    
    NODISCARD FORCEINLINE const Bool operator!=(const ZVector& vector) noexcept { 
        return data_ptr_ != vector.data_ptr_; 
    }

    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index) { 
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_[index]; 
    }
    NODISCARD FORCEINLINE const ObjectType& operator[](const IndexType index) const { 
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_[index]; 
    }

    ~ZVector() noexcept;


    NODISCARD FORCEINLINE ObjectType& At(const IndexType index) {
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_[index];
    }
    NODISCARD FORCEINLINE const ObjectType& At(const IndexType index) const {
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_[index];
    }
    NODISCARD FORCEINLINE ObjectType& Front() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[0]; 
    }
    NODISCARD FORCEINLINE const ObjectType& Front() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[0];
    }
    NODISCARD FORCEINLINE ObjectType& Back() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[size_ - 1];
    }
    NODISCARD FORCEINLINE const ObjectType& Back() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[size_ - 1];
    }

    NODISCARD FORCEINLINE const IndexType size() const { return size_; }
    NODISCARD FORCEINLINE const IndexType capacity() const { return capacity_; }
    NODISCARD FORCEINLINE const ObjectType* data_ptr() const { return data_ptr_; }

    /*
        The iterator funcions.
    */
    NODISCARD FORCEINLINE IteratorType Begin() { return IteratorType(data_ptr_); }
    NODISCARD FORCEINLINE ConstIteratorType ConstBegin() const { return ConstIteratorType(data_ptr_); }
    NODISCARD FORCEINLINE ReverseIteratorType ReverseBegin() { return ReverseIteratorType(data_ptr_ + size_ - 1); }
    NODISCARD FORCEINLINE ConstReverseIteratorType ConstReverseBegin() const {
        return ConstReverseIteratorType(data_ptr_ + size_ - 1);
    }
    NODISCARD FORCEINLINE IteratorType End() { return IteratorType(data_ptr_ + size_); }
    NODISCARD FORCEINLINE ConstIteratorType ConstEnd() const { return ConstIteratorType(data_ptr_ + size_); }
    NODISCARD FORCEINLINE ReverseIteratorType ReverseEnd() { return ReverseIteratorType(data_ptr_ - 1); }
    NODISCARD FORCEINLINE ConstReverseIteratorType ConstReverseEnd() const {
        return ConstReverseIteratorType(data_ptr_ - 1);
    }

    NODISCARD FORCEINLINE const Bool IsEmpty() { return size_ == 0; }

    /*
        Resize the vector, If the given size is smaller then the current size,
        the function will cut remove the extra objects. If the given size is
        bigger then the current size, the function will fill the extra size with
        the default value of ObjectType.
    */
    Void Resize(const IndexType size) noexcept;
    /*
        Resize the vector, If the given size is smaller then the current size,
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
        Remove the object at the back of the vector.
    */
    Void PopBack() noexcept;
    /*
        Remove the object at the back of the vector. 
        Give the authority to the given address.
    */
    Void PopBack(ObjectType* object_ptr) noexcept;

    /*
        Create an object at the back of the vector by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the vector.
    */
    template<typename... ArgsType>
    Void PushBack(ArgsType&&... args) noexcept;
    /*
        Create objects at the back of the vector by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the vector.
    */
    template<typename... ArgsType>
    Void PushBacks(const IndexType num, ArgsType&&... args) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the vector.
    */
    Void PushBacks(const IteratorType& begin, const IteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the vector.
    */
    Void PushBacks(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the vector.
    */
    Void PushBacks(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the vector.
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
        Construct the vector by filling it with the given amount of objects.
        The object is constructed by the arguements.
        If kIfUnique is false and no arguements, will only add the 
        size of the vector.
    */
    template<typename... ArgsType>
    Void Assign(const IndexType num, ArgsType&&... args) noexcept;
    /*
        Construct the vector by filling it objects between the iterators.
    */
    Void Assign(const IteratorType& begin, const IteratorType& end) noexcept;
    /*
        Construct the vector by filling it objects between the iterators.
    */
    Void Assign(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept;
    /*
        Construct the vector by filling it objects between the iterators.
    */
    Void Assign(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;
    /*
        Construct the vector by filling it objects between the iterators.
    */
    Void Assign(const ConstReverseIteratorType& begin, const ConstReverseIteratorType& end) noexcept;

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
    Void Sort(const IndexType begin, const IndexType end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const IndexType begin, const IndexType end, CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(const IteratorType begin, const IteratorType end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const IteratorType begin, const IteratorType end, CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(const ReverseIteratorType begin, const ReverseIteratorType end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const ReverseIteratorType begin, const ReverseIteratorType end, 
              CompareFunction&& compare_function) noexcept;

    /*
        Destroys all the objects in the vector, does not release the memory.
    */
    Void Clear() noexcept;
    /*
        Destroys all the objects in the vector, release the memory.
    */
    Void Destroy() noexcept;

protected:
    using SuperType = ZObject;

private:
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
ZVector<ObjectType, kIfUnique>::ZVector(const IndexType capacity) noexcept
    : SuperType()
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainer(capacity);
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
ZVector<ObjectType, kIfUnique>::ZVector(const IndexType capacity, ArgsType&&... args) noexcept
    : SuperType() 
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainer(capacity);
    CreateObjects(data_ptr_, capacity, std::forward<ArgsType>(args)...);
    size_ = capacity;
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector(const IteratorType& begin, const IteratorType& end) noexcept
    : SuperType()
{
    DEBUG(begin > end, "Begin iterator after end iterator!");
    IndexType size = end - begin;
    CreateContainer(size);
    CopyObjects(data_ptr_, begin.object_ptr(), end.object_ptr());
    size_ = size;

}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept
    : SuperType()
{
    DEBUG(begin > end, "Begin iterator after end iterator!");
    IndexType size = end - begin;
    CreateContainer(size);
    CopyObjects(data_ptr_, begin.object_ptr(), end.object_ptr());
    size_ = size;

}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector(const ReverseIteratorType& begin, 
                                                  const ReverseIteratorType& end) noexcept
    : SuperType()
{
    DEBUG(begin > end, "Begin iterator after end iterator!");
    IndexType size = end - begin;
    CreateContainer(size);
    CopyObjectsReverse(data_ptr_, begin.object_ptr(), end.object_ptr());
    size_ = size;
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector(const ConstReverseIteratorType& begin, 
                                                  const ConstReverseIteratorType& end) noexcept
    : SuperType()
{
    DEBUG(begin > end, "Begin iterator after end iterator!");
    IndexType size = end - begin;
    CreateContainer(size);
    CopyObjectsReverse(data_ptr_, begin.object_ptr(), end.object_ptr());
    size_ = size;
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector(const ZVector& vector) noexcept 
    : SuperType()
{
    CreateContainer(vector.size_);
    CreateAndCopyObjects(data_ptr_, vector.data_ptr_, vector.size_);
    size_ = vector.size_;
} 

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector(ZVector&& vector) noexcept 
    : SuperType()
    , data_ptr_(vector.data_ptr_)
    , capacity_(vector.capacity_)
    , size_(vector.size_)
{
    vector.MoveDestroy();
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>& ZVector<ObjectType, kIfUnique>::operator=(
        const ZVector& vector) noexcept {
    DEBUG(&vector == this, "The source and the target of the copy is the same!");
    SuperType::operator=(vector);
    if (vector.size_ > capacity_) {
        ExtendContainer(vector.size_);
    }
    CopyObjects(data_ptr_, vector.data_ptr_, vector.size_);
    size_ = vector.size_;
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>& ZVector<ObjectType, kIfUnique>::operator=(
        ZVector&& vector) noexcept {
    DEBUG(&vector == this, "The source and the target of the copy is the same!");
    SuperType::operator=(std::forward<ZVector>(vector));
    DestroyObjects(data_ptr_, size_);
    data_ptr_ = vector.data_ptr_;
    size_ = vector.size_;
    capacity_ = vector.capacity_;
    vector.MoveDestroy();
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::~ZVector() noexcept {
    DestroyContainer();
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::Resize(const IndexType size) noexcept {
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
Void ZVector<ObjectType, kIfUnique>::Resize(const IndexType size, ArgsType&&... args) noexcept {
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
Void ZVector<ObjectType, kIfUnique>::Reserve(const IndexType capacity) noexcept {
    if (capacity > capacity_) {
        ExtendContainer(capacity);
    }
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::PopBack() noexcept {
    DEBUG(size_ == 0, "No existing object to pop!");
    --size_;
    DestroyObject(size_);
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::PopBack(ObjectType* object_ptr) noexcept {
    DEBUG(size_ == 0, "No existing object to pop!");
    --size_;
    *object_ptr = std::move(data_ptr_[size_]);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfUnique>::PushBack(ArgsType&&... args) noexcept {
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObject(size_, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfUnique>::PushBacks(const IndexType num, ArgsType&&... args) noexcept {
    DEBUG(num < 0, "Negative pushing num not valid!");
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjects(data_ptr_ + size_, num, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::PushBacks(const IteratorType& begin, const IteratorType& end) noexcept {
    DEBUG(begin > end, "Begin iterator after end iterator!");
    IndexType new_size = size_ + (end - begin);
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CopyObjects(data_ptr_ + size_, begin.object_ptr(), end.object_ptr());
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::PushBacks(const ConstIteratorType& begin,
                                                         const ConstIteratorType& end) noexcept {
    DEBUG(begin > end, "Begin iterator after end iterator!");
    IndexType new_size = size_ + (end - begin);
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CopyObjects(data_ptr_ + size_, begin.object_ptr(), end.object_ptr());
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::PushBacks(const ReverseIteratorType& begin, 
                                                         const ReverseIteratorType& end) noexcept {
    DEBUG(begin > end, "Begin iterator after end iterator!");
    IndexType new_size = size_ + (end - begin);
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CopyObjectsReverse(data_ptr_, begin.object_ptr(), end.object_ptr());
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::PushBacks(const ConstReverseIteratorType& begin, 
                                                         const ConstReverseIteratorType& end) noexcept {
    DEBUG(begin > end, "Begin iterator after end iterator!");
    IndexType new_size = size_ + (end - begin);
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CopyObjectsReverse(data_ptr_, begin.object_ptr(), end.object_ptr());
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfUnique>::EmplaceBack(ArgsType&&... args) noexcept {
    DEBUG(size_ == 0, "No existing object to emplace!");
    DestroyObject(size_ - 1);
    CreateObject(size_ - 1, std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Insert(
        const IndexType index, ArgsType&&... args) noexcept {
    DEBUG(index < 0 || index > size_, "Insert index out of bounds!");
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + 1)), reinterpret_cast<Void*>(data_ptr_ + index),
            (size_ - index) * sizeof(ObjectType));
    CreateObject(index, std::forward<ArgsType>(args)...);
    size_ = new_size;
    return IteratorType(data_ptr_ + index);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Insert(
        const IteratorType& iterator, ArgsType&&... args) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() > data_ptr_ + size_, 
          "Insert iterator out of bounds!");
    IndexType new_size = size_ + 1;
    IndexType index = static_cast<IndexType>(iterator.object_ptr() - data_ptr_);
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + 1)), reinterpret_cast<Void*>(data_ptr_ + index),
            (size_ - index) * sizeof(ObjectType));
    CreateObject(index, std::forward<ArgsType>(args)...);
    size_ = new_size;
    return IteratorType(data_ptr_ + index);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
ZVector<ObjectType, kIfUnique>::ReverseIteratorType ZVector<ObjectType, kIfUnique>::Insert(
    const ReverseIteratorType& iterator, ArgsType&&... args) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() > data_ptr_ + size_, 
          "Insert iterator out of bounds!");
    IndexType new_size = size_ + 1;
    IndexType index = static_cast<IndexType>(iterator.object_ptr() - data_ptr_) + 1;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + 1)), reinterpret_cast<Void*>(data_ptr_ + index),
            (size_ - index) * sizeof(ObjectType));
    CreateObject(index, std::forward<ArgsType>(args)...);
    size_ = new_size;
    return ReverseIteratorType(data_ptr_ + index);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const IndexType index, IndexType num, ArgsType&&... args) noexcept {
    DEBUG(index < 0 || index > size_, "Insert index out of bounds!");
    DEBUG(num < 0, "Negative insert num not valid!");
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
            (size_ - index) * sizeof(ObjectType));
    CreateObjects(data_ptr_ + index, data_ptr_ + (index + num), std::forward<ArgsType>(args)...);
    size_ = new_size;
    return IteratorType(data_ptr_ + index);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const IteratorType& iterator, IndexType num, ArgsType&&... args) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() > data_ptr_ + size_, 
          "Insert iterator out of bounds!");
    DEBUG(num < 0, "Negative insert num not valid!");
    IndexType new_size = size_ + num;
    IndexType index = iterator.object_ptr() - data_ptr_;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
            (size_ - index) * sizeof(ObjectType));
    CreateObjects(data_ptr_ + index, data_ptr_ + (index + num), std::forward<ArgsType>(args)...);
    size_ = new_size;
    return IteratorType(data_ptr_ + index);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
ZVector<ObjectType, kIfUnique>::ReverseIteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const ReverseIteratorType& iterator, IndexType num, ArgsType&&... args) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() > data_ptr_ + size_, 
          "Insert iterator out of bounds!");
    DEBUG(num < 0, "Negative insert num not valid!");
    IndexType new_size = size_ + num;
    IndexType index = iterator.object_ptr() - data_ptr_ + 1;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
            (size_ - index) * sizeof(ObjectType));
    CreateObjects(data_ptr_ + index, data_ptr_ + (index + num), std::forward<ArgsType>(args)...);
    size_ = new_size;
    return ReverseIteratorType(data_ptr_ + (index + num - 1));
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const IndexType index, const IteratorType& src_begin, const IteratorType& src_end) noexcept {
    DEBUG(index < 0 || index > size_, "Insert index out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
            (size_ - index) * sizeof(ObjectType));
    CreateAndCopyObjects(data_ptr_ + index, src_begin.object_ptr(), src_end.object_ptr());
    size_ = new_size;
    return IteratorType(data_ptr_ + index);
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const IndexType index, const ConstIteratorType& src_begin, const ConstIteratorType& src_end) noexcept {
    DEBUG(index < 0 || index > size_, "Insert index out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
        (size_ - index) * sizeof(ObjectType));
    CreateAndCopyObjects(data_ptr_ + index, src_begin.object_ptr(), src_end.object_ptr());
    size_ = new_size;
    return IteratorType(data_ptr_ + index);
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const IndexType index, const ReverseIteratorType& src_begin, const ReverseIteratorType& src_end) noexcept {
    DEBUG(index < 0 || index > size_, "Insert index out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
        (size_ - index) * sizeof(ObjectType));
    CreateAndCopyObjectsReverse(data_ptr_ + index, src_begin.object_ptr(), src_end.object_ptr());
    size_ = new_size;
    return IteratorType(data_ptr_ + index);
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const IndexType index, 
        const ConstReverseIteratorType& src_begin, const ConstReverseIteratorType& src_end) noexcept {
    DEBUG(index < 0 || index > size_, "Insert index out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
        (size_ - index) * sizeof(ObjectType));
    CreateAndCopyObjectsReverse(data_ptr_ + index, src_begin.object_ptr(), src_end.object_ptr());
    size_ = new_size;
    return IteratorType(data_ptr_ + index);
}
template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const IteratorType& iterator,
        const IteratorType& src_begin, const IteratorType& src_end) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() > data_ptr_ + size_, 
          "Insert iterator out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    IndexType index = iterator.object_ptr() - data_ptr_;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
        (size_ - index) * sizeof(ObjectType));
    CreateAndCopyObjects(data_ptr_ + index, src_begin.object_ptr(), src_end.object_ptr());
    size_ = new_size;
    return IteratorType(data_ptr_ + index);
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const IteratorType& iterator,
        const ConstIteratorType& src_begin, const ConstIteratorType& src_end) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() > data_ptr_ + size_, 
          "Insert iterator out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    IndexType index = iterator.object_ptr() - data_ptr_;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
        (size_ - index) * sizeof(ObjectType));
    CreateAndCopyObjects(data_ptr_ + index, src_begin.object_ptr(), src_end.object_ptr());
    size_ = new_size;
    return IteratorType(data_ptr_ + index);
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const IteratorType& iterator,
        const ReverseIteratorType& src_begin, const ReverseIteratorType& src_end) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() > data_ptr_ + size_, 
          "Insert iterator out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    IndexType index = iterator.object_ptr() - data_ptr_;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
        (size_ - index) * sizeof(ObjectType));
    CreateAndCopyObjectsReverse(data_ptr_ + index, src_begin.object_ptr(), src_end.object_ptr());
    size_ = new_size;
    return IteratorType(data_ptr_ + index);
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const IteratorType& iterator,
        const ConstReverseIteratorType& src_begin, const ConstReverseIteratorType& src_end) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() > data_ptr_ + size_, 
          "Insert iterator out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    IndexType index = iterator.object_ptr() - data_ptr_;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
        (size_ - index) * sizeof(ObjectType));
    CreateAndCopyObjectsReverse(data_ptr_ + index, src_begin.object_ptr(), src_end.object_ptr());
    size_ = new_size;
    return IteratorType(data_ptr_ + index);
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ReverseIteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const ReverseIteratorType& iterator,
        const IteratorType& src_begin, const IteratorType& src_end) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() > data_ptr_ + size_,
          "Insert iterator out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    IndexType index = iterator.object_ptr() - data_ptr_ + 1;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
        (size_ - index) * sizeof(ObjectType));
    CreateAndCopyObjectsReverse(data_ptr_ + index, src_end.object_ptr() - 1, src_begin.object_ptr() - 1);
    size_ = new_size;
    return ReverseIteratorType(data_ptr_ + (index + num - 1));
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ReverseIteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const ReverseIteratorType& iterator,
        const ConstIteratorType& src_begin, const ConstIteratorType& src_end) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() > data_ptr_ + size_,
        "Insert iterator out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    IndexType index = iterator.object_ptr() - data_ptr_ + 1;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
        (size_ - index) * sizeof(ObjectType));
    CreateAndCopyObjectsReverse(data_ptr_ + index, src_end.object_ptr() - 1, src_begin.object_ptr() - 1);
    size_ = new_size;
    return ReverseIteratorType(data_ptr_ + (index + num - 1));
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ReverseIteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const ReverseIteratorType& iterator,
        const ReverseIteratorType& src_begin, const ReverseIteratorType& src_end) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() > data_ptr_ + size_,
        "Insert iterator out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    IndexType index = iterator.object_ptr() - data_ptr_ + 1;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
        (size_ - index) * sizeof(ObjectType));
    CreateAndCopyObjects(data_ptr_ + index, src_end.object_ptr() - 1, src_begin.object_ptr() - 1);
    size_ = new_size;
    return ReverseIteratorType(data_ptr_ + (index + num - 1));
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ReverseIteratorType ZVector<ObjectType, kIfUnique>::Inserts(
        const ReverseIteratorType& iterator,
        const ConstReverseIteratorType& src_begin,
        const ConstReverseIteratorType& src_end) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() > data_ptr_ + size_,
        "Insert iterator out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    IndexType index = iterator.object_ptr() - data_ptr_ + 1;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(data_ptr_ + (index + num)), reinterpret_cast<Void*>(data_ptr_ + index),
        (size_ - index) * sizeof(ObjectType));
    CreateAndCopyObjects(data_ptr_ + index, src_end.object_ptr() - 1, src_begin.object_ptr() - 1);
    size_ = new_size;
    return ReverseIteratorType(data_ptr_ + (index + num - 1));
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Erase(
        IndexType index) noexcept {
    DEBUG(index < 0 || index >= size_, "Erase index out of bounds!");
    DestroyObject(index);
    memmove(reinterpret_cast<Void*>(&data_ptr_[index]), reinterpret_cast<Void*>(&data_ptr_[index + 1]),
            (size_ - index - 1) * sizeof(ObjectType));
    --size_;
    return IteratorType(&data_ptr_[index]);
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Erase(
        const IteratorType& iterator) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() >= data_ptr_ + size_, 
          "Erase iterator out of bounds!");
    DestroyObject(iterator.object_ptr());
    memmove(reinterpret_cast<Void*>(iterator.object_ptr()),
            reinterpret_cast<Void*>(iterator.object_ptr() + 1),
            (size_ - static_cast<IndexType>(iterator.object_ptr() - data_ptr_) - 1) * sizeof(ObjectType));
    --size_;
    return iterator;
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ReverseIteratorType ZVector<ObjectType, kIfUnique>::Erase(
        const ReverseIteratorType& iterator) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() >= data_ptr_ + size_, 
          "Erase iterator out of bounds!");
    DestroyObject(iterator.object_ptr());
    memmove(reinterpret_cast<Void*>(iterator.object_ptr()),
            reinterpret_cast<Void*>(iterator.object_ptr() + 1),
            (size_ - static_cast<IndexType>(iterator.object_ptr() - data_ptr_) - 1) * sizeof(ObjectType));
    --size_;
    return ReverseIteratorType(iterator.object_ptr() - 1);
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Erases(
        const IndexType index, const IndexType num) noexcept {
    DEBUG(index < 0 || index >= size_, "Erase index out of bounds!");
    DEBUG(num < 0, "Negative erase num not valid!");
    DEBUG(index + num > size_, "Erasing not existing objects!");
    DestroyObjects(&data_ptr_[index], num);
    memmove(reinterpret_cast<Void*>(&data_ptr_[index]), reinterpret_cast<Void*>(&data_ptr_[index + num]),
            (size_ - index - num) * sizeof(ObjectType));
    size_ -= num;
    return IteratorType(&data_ptr_[index]);
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Erases(
        const IteratorType& iterator, const IndexType num) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() >= data_ptr_ + size_, 
          "Erase iterator out of bounds!");
    DEBUG(num < 0, "Negative erase num not valid!");
    DEBUG(iterator.object_ptr() + num > data_ptr_ + size_, "Erasing not existing objects!");
    DestroyObjects(iterator.object_ptr(), num);
    memmove(reinterpret_cast<Void*>(iterator.object_ptr()),
            reinterpret_cast<Void*>(iterator.object_ptr() + num),
            (size_ - static_cast<IndexType>(iterator.object_ptr() - data_ptr_) - num) * sizeof(ObjectType));
    size_ -= num;
    return iterator;
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ReverseIteratorType ZVector<ObjectType, kIfUnique>::Erases(
        const ReverseIteratorType& iterator, const IndexType num) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() >= data_ptr_ + size_, 
          "Erase iterator out of bounds!");
    DEBUG(num < 0, "Negative erase num not valid!");
    DEBUG(iterator.object_ptr() +- num < data_ptr_, "Erasing not existing objects!");
    DestroyObjects(iterator.object_ptr() - num + 1, iterator.object_ptr() + 1);
    memmove(reinterpret_cast<Void*>(iterator.object_ptr() - num + 1),
            reinterpret_cast<Void*>(iterator.object_ptr() + 1),
            (size_ - static_cast<IndexType>(iterator.object_ptr() - data_ptr_) - num) * sizeof(ObjectType));
    size_ -= num;
    return ReverseIteratorType(iterator.object_ptr() - num);
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::IteratorType ZVector<ObjectType, kIfUnique>::Erases(
        const IteratorType& begin, const IteratorType& end) noexcept {
    DEBUG(begin.object_ptr() < data_ptr_ || begin.object_ptr() >= data_ptr_ + size_,
          "Erase iterator out of bounds!");
    DEBUG(end.object_ptr() < data_ptr_ || end.object_ptr() >= data_ptr_ + size_,
          "Erase iterator out of bounds!");
    DEBUG(begin > end, "Begin iterator after end iterator!");
    DestroyObjects(begin.object_ptr(), end.object_ptr());
    memmove(reinterpret_cast<Void*>(begin.object_ptr()), reinterpret_cast<Void*>(end.object_ptr()),
            (size_ - static_cast<IndexType>(end.object_ptr() - data_ptr_)) * sizeof(ObjectType));
    size_ -= end - begin;
    return begin;
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ReverseIteratorType ZVector<ObjectType, kIfUnique>::Erases(
        const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept {
    DEBUG(begin.object_ptr() < data_ptr_ || begin.object_ptr() >= data_ptr_ + size_,
          "Erase iterator out of bounds!");
    DEBUG(end.object_ptr() < data_ptr_ || end.object_ptr() >= data_ptr_ + size_,
          "Erase iterator out of bounds!");
    DEBUG(begin > end, "Begin iterator after end iterator!");
    DestroyObjects(end.object_ptr() + 1, begin.object_ptr() + 1);
    memmove(reinterpret_cast<Void*>(end.object_ptr() + 1), reinterpret_cast<Void*>(begin.object_ptr() + 1),
            (size_ - static_cast<IndexType>(end.object_ptr() - data_ptr_)) * sizeof(ObjectType));
    size_ -= static_cast<IndexType>(end.object_ptr() - begin.object_ptr());
    return end;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfUnique>::Emplace(const IndexType index, ArgsType&&... args) noexcept {
    DEBUG(index < 0 || index >= size_, "Emplace index out of bounds!");
    DestroyObject(index);
    CreateObject(index, std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfUnique>::Emplace(const IteratorType& iterator, ArgsType&&... args) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() >= data_ptr_ + size_,
          "Emplace iterator out of bounds!");
    DestroyObject(iterator.object_ptr());
    CreateObject(iterator.object_ptr(), std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfUnique>::Emplace(const ReverseIteratorType& iterator, 
                                                       ArgsType&&... args) noexcept {
    DEBUG(iterator.object_ptr() < data_ptr_ || iterator.object_ptr() >= data_ptr_ + size_,
          "Emplace iterator out of bounds!");
    DestroyObject(iterator.object_ptr());
    CreateObject(iterator.object_ptr(), std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfUnique>::Assign(const IndexType num, ArgsType&&... args) noexcept {
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
Void ZVector<ObjectType, kIfUnique>::Assign(const IteratorType& begin, const IteratorType& end) noexcept {
    DEBUG(begin > end, "Begin iterator after end iterator!");
    IndexType new_size = end - begin;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    //If the iterator is from this.
    if (begin.object_ptr() >= data_ptr_ && begin.object_ptr() <= (data_ptr_ + size_)) {
        DestroyObjects(data_ptr_, begin.object_ptr());
        DestroyObjects(end.object_ptr(), data_ptr_ + size_);
        //Move the objects to the front.
        memmove(reinterpret_cast<Void*>(data_ptr_), reinterpret_cast<Void*>(begin.object_ptr()),
                new_size * sizeof(ObjectType));
    }
    else {
        CopyObjects(data_ptr_, begin.object_ptr(), end.object_ptr());
    }
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::Assign(const ConstIteratorType& begin, 
                                                      const ConstIteratorType& end) noexcept {
    DEBUG(begin > end, "Begin iterator after end iterator!");
    IndexType new_size = end - begin;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    //If the iterator is from this.
    if (begin.object_ptr() >= data_ptr_ && begin.object_ptr() <= (data_ptr_ + size_)) {
        DestroyObjects(data_ptr_, begin.object_ptr());
        DestroyObjects(end.object_ptr(), data_ptr_ + size_);
        //Move the objects to the front.
        memmove(reinterpret_cast<Void*>(data_ptr_), reinterpret_cast<Void*>(begin.object_ptr()),
            new_size * sizeof(ObjectType));
    }
    else {
        CopyObjects(data_ptr_, begin.object_ptr(), end.object_ptr());
    }
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::Assign(const ReverseIteratorType& begin,
                                                      const ReverseIteratorType& end) noexcept {
    DEBUG(begin > end, "Begin iterator after end iterator!");
    IndexType new_size = end - begin;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    //If the iterator is from this.
    if (begin.object_ptr() >= data_ptr_ && begin.object_ptr() <= (data_ptr_ + size_)) {
        DestroyObjects(data_ptr_, end.object_ptr() + 1);
        DestroyObjects(begin.object_ptr() + 1, data_ptr_ + size_);
        //Reverses the objects.
        ObjectType* begin_ptr = end.object_ptr() + 1;
        ObjectType* end_ptr = begin.object_ptr();
        while (begin_ptr < end_ptr) {
            Swap(begin_ptr, end_ptr);
            ++begin_ptr;
            --end_ptr;
        }
        //Move the objects to the first front.
        memmove(reinterpret_cast<Void*>(data_ptr_), reinterpret_cast<Void*>(end.object_ptr() + 1),
                new_size * sizeof(ObjectType));
    }
    else {
        CopyObjectsReverse(data_ptr_, begin.object_ptr(), end.object_ptr());
    }
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::Assign(const ConstReverseIteratorType& begin,
                                                      const ConstReverseIteratorType& end) noexcept {
    DEBUG(begin > end, "Begin iterator after end iterator!");
    IndexType new_size = end - begin;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    //If the iterator is from this.
    if (begin.object_ptr() >= data_ptr_ && begin.object_ptr() <= (data_ptr_ + size_)) {
        DestroyObjects(data_ptr_, end.object_ptr() + 1);
        DestroyObjects(begin.object_ptr() + 1, data_ptr_ + size_);
        //Reverses the objects.
        ObjectType* begin_ptr = end.object_ptr() + 1;
        ObjectType* end_ptr = begin.object_ptr();
        while (begin_ptr < end_ptr) {
            Swap(begin_ptr, end_ptr);
            ++begin_ptr;
            --end_ptr;
        }
        //Move the objects to the first front.
        memmove(reinterpret_cast<Void*>(data_ptr_), reinterpret_cast<Void*>(end.object_ptr() + 1),
                new_size * sizeof(ObjectType));
    }
    else {
        CopyObjectsReverse(data_ptr_, begin.object_ptr(), end.object_ptr());
    }
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::Clear() noexcept {
    DestroyObjects(data_ptr_, data_ptr_ + size_);
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::Destroy() noexcept {
    DestroyContainer();
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CreateContainer(const IndexType capacity) noexcept {
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    MemoryType need_memory_size = capacity * sizeof(ObjectType);
    MemoryType apply_mrmory_size;
    data_ptr_ = reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));
    capacity_ = apply_mrmory_size / sizeof(ObjectType);
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::ExtendContainer(const IndexType capacity) noexcept {
    DEBUG(capacity < 0, "Negaive capacity not valid!");
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
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::DestroyContainer() noexcept {
    DestroyObjects(data_ptr_, data_ptr_ + size_);
    memory_pool::ReleaseMemory(reinterpret_cast<Void*>(data_ptr_));
    data_ptr_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::MoveDestroy() {
    data_ptr_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CreateObject(const IndexType index, ArgsType&&... args) {
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
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CreateObject(ObjectType* const object_ptr, ArgsType&&... args) {
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
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::DestroyObject(const IndexType index) {
    if constexpr (kIfUnique) {
        data_ptr_[index].~ObjectType();
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::DestroyObject(ObjectType* const object_ptr) {
    if constexpr (kIfUnique) {
        (*object_ptr).~ObjectType();
    }
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CreateObjects(ObjectType* begin_ptr, ObjectType* const end_ptr,
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
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CreateObjects(ObjectType* begin_ptr, const IndexType num,
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
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CreateAndCopyObjects(ObjectType* dst_ptr, 
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
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CreateAndCopyObjects(ObjectType* dst_ptr,
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
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CreateAndCopyObjectsReverse(
        ObjectType* dst_ptr, const ObjectType* src_begin_ptr, const ObjectType* const src_end_ptr) {
    while (src_begin_ptr > src_end_ptr) {
        new(reinterpret_cast<Void*>(dst_ptr)) ObjectType(*src_begin_ptr);
        ++dst_ptr;
        --src_begin_ptr;
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CreateAndCopyObjectsReverse(ObjectType* dst_ptr, 
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
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CopyObjects(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
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
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr,
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
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CopyObjectsReverse(ObjectType* dst_ptr,
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
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CopyObjectsReverse(ObjectType* dst_ptr, const ObjectType* src_ptr, 
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
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::DestroyObjects(ObjectType* begin_ptr, ObjectType* const end_ptr) {
    if constexpr (kIfUnique) {
        while (begin_ptr < end_ptr) {
            begin_ptr->~ObjectType();
            ++begin_ptr;
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::DestroyObjects(ObjectType* begin_ptr, const IndexType num) {
    if constexpr (kIfUnique) {
        ObjectType* end_ptr = begin_ptr + num;
        while (begin_ptr < end_ptr) {
            begin_ptr->~ObjectType();
            ++begin_ptr;
        }
    }
}

}//zengine

#endif // !Z_CORE_Z_VECTOR_H_