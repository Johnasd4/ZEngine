#ifndef Z_CORE_Z_VECTOR_H_
#define Z_CORE_Z_VECTOR_H_

#include "internal/drive.h"
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
    NODISCARD FORCEINLINE const Bool operator>(const VectorIteratorBase& iterator) const {
        return object_ptr_ > iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const VectorIteratorBase& iterator) const {
        return object_ptr_ >= iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const VectorIteratorBase& iterator) const {
        return object_ptr_ < iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const VectorIteratorBase& iterator) const {
        return object_ptr_ <= iterator.object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const VectorIteratorBase iterator) const {
        return object_ptr_ - iterator.object_ptr_;
    }

    FORCEINLINE ~VectorIteratorBase() {}

    FORCEINLINE Void MoveDestroy() { object_ptr_ = nullptr; }

protected:
     ObjectType* object_ptr_;
};

template<typename ObjectType>
class VectorIterator : public VectorIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE VectorIterator& operator+(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE VectorIterator& operator-(const IndexType data_num) {
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

    NODISCARD FORCEINLINE ObjectType* const object_ptr() { return SuperType::object_ptr_; }

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

    NODISCARD FORCEINLINE const ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE VectorConstIterator& operator+(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE VectorConstIterator& operator-(const IndexType data_num) {
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

    NODISCARD FORCEINLINE const ObjectType* const object_ptr() { return SuperType::object_ptr_; }

protected:
    using SuperType = VectorIteratorBase<ObjectType>;
};

template<typename ObjectType>
class VectorReverseIterator : public VectorIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE VectorReverseIterator& operator+(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE VectorReverseIterator& operator-(const IndexType data_num) {
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

    NODISCARD FORCEINLINE ObjectType* const object_ptr() { return SuperType::object_ptr_; }

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

    NODISCARD FORCEINLINE const ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE VectorConstReverseIterator& operator+(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE VectorConstReverseIterator& operator-(const IndexType data_num) {
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

    NODISCARD FORCEINLINE const ObjectType* const object_ptr() { return SuperType::object_ptr_; }

protected:
    using SuperType = VectorIteratorBase<ObjectType>;
};

}//internal

/*
    Vector caintainer,
    If kIfInitializeObject is true, will call the constuctor when the object is created.
*/
template<typename ObjectType, Bool kIfInitializeObject = kIsClass<ObjectType>>
class ZVector : public ZObject {
private:
    static constexpr Float32 kAutoExtendMulFactor = 1.5F;

public:
    using Iterator = internal::VectorIterator<ObjectType>;
    using ConstIterator = internal::VectorConstIterator<ObjectType>;
    using ReverseIterator = internal::VectorReverseIterator<ObjectType>;
    using ConstReverseIterator = internal::VectorConstReverseIterator<ObjectType>;


    ZVector() noexcept;
    ZVector(const IndexType capacity) noexcept;
    ZVector(const ZVector& vector) noexcept;
    ZVector(ZVector&& vector) noexcept;

    ZVector& operator=(const ZVector& vector) noexcept;
    ZVector& operator=(ZVector&& vector) noexcept;

    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index) { return data_ptr_[index]; }
    NODISCARD FORCEINLINE const ObjectType& operator[](const IndexType index) const { return data_ptr_[index]; }

    ~ZVector() noexcept;

    /*
        The iterator funcions.
    */
    NODISCARD FORCEINLINE Iterator Begin() { return Iterator(data_ptr_); }
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const { return ConstIterator(data_ptr_); }
    NODISCARD FORCEINLINE ReverseIterator ReverseBegin() { return ReverseIterator(data_ptr_ + size_ - 1); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseBegin() const {
        return ConstReverseIterator(data_ptr_ + size_ - 1);
    }
    NODISCARD FORCEINLINE Iterator End() { return Iterator(data_ptr_ + size_); }
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const { return ConstIterator(data_ptr_ + size_); }
    NODISCARD FORCEINLINE ReverseIterator ReverseEnd() { return ReverseIterator(data_ptr_ - 1); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseEnd() const {
        return ConstReverseIterator(data_ptr_ - 1);
    }

    NODISCARD FORCEINLINE const IndexType size() const { return size_; }
    NODISCARD FORCEINLINE const IndexType capacity() const { return capacity_; }
    NODISCARD FORCEINLINE ObjectType* data_ptr() { return data_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* data_ptr() const { return data_ptr_; }

    NODISCARD FORCEINLINE ObjectType& At(const IndexType index) { return data_ptr_[index]; }
    NODISCARD FORCEINLINE const ObjectType& At(const IndexType index) const { return data_ptr_[index]; }
    NODISCARD FORCEINLINE ObjectType& Front() { return data_ptr_[0]; }
    NODISCARD FORCEINLINE const ObjectType& Front() const { return data_ptr_[0]; }
    NODISCARD FORCEINLINE ObjectType& Back() { return data_ptr_[size_ - 1]; }
    NODISCARD FORCEINLINE const ObjectType& Back() const { return data_ptr_[size_ - 1]; }

    NODISCARD FORCEINLINE const Bool IfEmpty() { return size_ == 0; }

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
        the given object type.
    */
    Void Resize(const IndexType size, const ObjectType& object) noexcept;

    /*
        Will extend the capacity by the given capacity, if the smaller then the
        given capacity, this function will do nothing, otherwise the capacity will
        entend to a size that equals or bigger then the given capacity.
    */
    Void Reserve(const IndexType capacity) noexcept;

    /*
        Remove the last object of the vector.
    */
    Void PopBack() noexcept;

    /*
        Create an object at the end of the vector by calling the constructor with
        the arguements. If kIfInitializeObject is false and no arguements, will
        only add the size of the victor.
    */
    template<typename... ArgsType>
    Void PushBack(ArgsType&&... args) noexcept;
    /*
        Create objects at the end of the vector by calling the constructor with
        the arguements. If kIfInitializeObject is false and no arguements, will
        only add the size of the victor.
    */
    template<typename... ArgsType>
    Void PushBacks(const IndexType num, ArgsType&&... args) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void EmplaceBack(ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    Iterator Insert(const IndexType index, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    Iterator Insert(const Iterator iterator, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    ReverseIterator Insert(const ReverseIterator iterator, ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    Iterator Inserts(const IndexType index, IndexType num, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    Iterator Inserts(const Iterator iterator, IndexType num, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    ReverseIterator Inserts(const ReverseIterator iterator, IndexType num, ArgsType&&... args) noexcept;


    Void Erase(const IndexType index) noexcept;
    Void Erase(const Iterator iterator) noexcept;
    Void Erase(const ReverseIterator iterator) noexcept;

    Void Erases(const IndexType index, const IndexType num) noexcept;
    Void Erases(const Iterator iterator, const IndexType num) noexcept;
    Void Erases(const ReverseIterator iterator, const IndexType num) noexcept;
    /*
        Erases the elements between begin and end, involves begin, but don't involves end.
    */
    Void Erases(const Iterator begin, const Iterator end) noexcept;
    Void Erases(const ReverseIterator begin, const ReverseIterator end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const IndexType index, ArgsType&&... args) noexcept;
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const Iterator iterator, ArgsType&&... args) noexcept;
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const ReverseIterator iterator, ArgsType&&... args) noexcept;

    /*
        Construct the vector by filling it with the given amount of objects.
        The object is constructed by the arguements.
        If kIfInitializeObject is false and no arguements, will only add the 
        size of the victor.
    */
    template<typename... ArgsType>
    Void Assign(const IndexType num, ArgsType&&... args) noexcept;
    /*
        Construct the vector by filling it objects between the iterators.
    */
    Void Assign(const Iterator begin, const Iterator end) noexcept;
    /*
        Construct the vector by filling it objects between the iterators.
    */
    Void Assign(const ReverseIterator begin, const ReverseIterator end) noexcept;

    /*
        The small object on the front.
    */
    Void Sort() noexcept;
    /*
        will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(const Iterator begin, const Iterator end) noexcept;
    /*
        will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const Iterator begin, const Iterator end, CompareFunction&& compare_function) noexcept;

    Void Clear() noexcept;

protected:
    using SuperType = ZObject;
private:
    /*
        Extends the capacity by the given capacity, the final capacity might
        not equal the given capacity.
    */
    Void ExtendCapacity(const IndexType capacity) noexcept;

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
    FORCEINLINE Void CreateObjects(ObjectType* dst_ptr, const ObjectType* src_begin_ptr, 
                                   const ObjectType* const src_end_ptr);

    /*
        Initialize the memory by the given arguements([begin + num)).
        Will call the copy constructor.
    */
    FORCEINLINE Void CreateObjects(ObjectType* dst_ptr, const ObjectType* src_ptr, const IndexType num);

    /*
        Copy objects by the given pointer. Will call the copy assignment operator
        if this object class's member kIfInitializeObject is true. 
    */
    FORCEINLINE Void CopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr, const IndexType num);

    /*
        Destroy the objects by the given arguements([begin, end)). 
        Will call the destrctor if this object class's member kIfInitializeObject is true.
    */
    FORCEINLINE Void DestroyObjects(ObjectType* begin_ptr, ObjectType* const end_ptr);

    /*
        Destroy the objects by the given arguements([begin, begin + num)).
        Will call the destrctor if this object class's member kIfInitializeObject is true.
    */
    FORCEINLINE Void DestroyObjects(ObjectType* begin_ptr, const IndexType num);

    ObjectType* data_ptr_;
    IndexType size_;
    IndexType capacity_;
};

template<typename ObjectType, Bool kIfInitializeObject>
ZVector<ObjectType, kIfInitializeObject>::ZVector() noexcept
    : data_ptr_(nullptr)
    , size_(0)
    , capacity_(0)
{}

template<typename ObjectType, Bool kIfInitializeObject>
ZVector<ObjectType, kIfInitializeObject>::ZVector(const IndexType capacity) noexcept
    : size_(0)
{
    ExtendCapacity(capacity);
}

template<typename ObjectType, Bool kIfInitializeObject>
ZVector<ObjectType, kIfInitializeObject>::ZVector(const ZVector& vector) noexcept {
    ExtendCapacity(vector.size_);
    CreateObjects(data_ptr_, vector.data_ptr_, vector.size_);
    size_ = vector.size_;
}

template<typename ObjectType, Bool kIfInitializeObject>
ZVector<ObjectType, kIfInitializeObject>::ZVector(ZVector&& vector) noexcept
    : data_ptr_(vector.data_ptr_)
    , size_(vector.size_)
    , capacity_(vector.capacity_)
{
    vector.MoveDestroy();
}

template<typename ObjectType, Bool kIfInitializeObject>
ZVector<ObjectType, kIfInitializeObject>& ZVector<ObjectType, kIfInitializeObject>::operator=(
        const ZVector& vector) noexcept {
    if (vector.size_ > capacity_) {
        ExtendCapacity(size_);
    }
    CopyObjects(data_ptr_, vector.data_ptr_, vector.size_);
    size_ = vector.size_;
    return *this;
}

template<typename ObjectType, Bool kIfInitializeObject>
ZVector<ObjectType, kIfInitializeObject>& ZVector<ObjectType, kIfInitializeObject>::operator=(
        ZVector&& vector) noexcept {
    data_ptr_ = vector.data_ptr_;
    size_ = vector.size_;
    capacity_ = vector.capacity_;
    vector.MoveDestroy();
    return *this;
}


template<typename ObjectType, Bool kIfInitializeObject>
ZVector<ObjectType, kIfInitializeObject>::~ZVector() noexcept {
    if (data_ptr_ != nullptr) {
        DestroyObjects(data_ptr_, data_ptr_ + size_);
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(data_ptr_));
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::Resize(const IndexType size) noexcept {
    if (size_ < size) {
        if (size > capacity_) {
            ExtendCapacity(size);
        }
        CreateObjects(data_ptr_ + size_, size - size_);
    }
    else {
        DestroyObjects(data_ptr_ + size, data_ptr_ + size_);
    }
    size_ = size;
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::Resize(const IndexType size, const ObjectType& object) noexcept {
    if (size_ < size) {
        if (size > capacity_) {
            ExtendCapacity(size);
        }
        CreateObjects(data_ptr_ + size_, size - size_, object);
    }
    else {
        DestroyObjects(data_ptr_ + size, data_ptr_ + size_);
    }
    size_ = size;
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::Reserve(const IndexType capacity) noexcept {
    if (capacity > capacity_) {
        ExtendCapacity(capacity);
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::PopBack() noexcept {
    if (size_ > 0) {
        --size_;
        DestroyObject(size_);
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfInitializeObject>::PushBack(ArgsType&&... args) noexcept {
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObject(size_, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfInitializeObject>::PushBacks(const IndexType num, ArgsType&&... args) noexcept {
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjects(data_ptr_ + size_, num, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfInitializeObject>::EmplaceBack(ArgsType&&... args) noexcept {
    DestroyObject(size_ - 1);
    CreateObject(size_ - 1, std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
ZVector<ObjectType, kIfInitializeObject>::Iterator ZVector<ObjectType, kIfInitializeObject>::Insert(
        const IndexType index, ArgsType&&... args) noexcept {
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(&data_ptr_[index + 1]), reinterpret_cast<Void*>(&data_ptr_[index]),
            (size_ - index) * sizeof(ObjectType));
    CreateObject(index, std::forward<ArgsType>(args)...);
    size_ = new_size;
    return Iterator(&data_ptr_[index]);
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
ZVector<ObjectType, kIfInitializeObject>::Iterator ZVector<ObjectType, kIfInitializeObject>::Insert(
        const Iterator iterator, ArgsType&&... args) noexcept {
    IndexType new_size = size_ + 1;
    IndexType index = iterator.object_ptr() - data_ptr_;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(&data_ptr_[index + 1]), reinterpret_cast<Void*>(&data_ptr_[index]),
            (size_ - index) * sizeof(ObjectType));
    CreateObject(index, std::forward<ArgsType>(args)...);
    size_ = new_size;
    return Iterator(&data_ptr_[index]);
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
ZVector<ObjectType, kIfInitializeObject>::ReverseIterator ZVector<ObjectType, kIfInitializeObject>::Insert(
    const ReverseIterator iterator, ArgsType&&... args) noexcept {
    IndexType new_size = size_ + 1;
    IndexType index = iterator.object_ptr() - data_ptr_ + 1;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(&data_ptr_[index + 1]), reinterpret_cast<Void*>(&data_ptr_[index]),
            (size_ - index) * sizeof(ObjectType));
    CreateObject(index, std::forward<ArgsType>(args)...);
    size_ = new_size;
    return Iterator(&data_ptr_[index]);
}


template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
ZVector<ObjectType, kIfInitializeObject>::Iterator ZVector<ObjectType, kIfInitializeObject>::Inserts(
        const IndexType index, IndexType num, ArgsType&&... args) noexcept {
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(&data_ptr_[index + num]), reinterpret_cast<Void*>(&data_ptr_[index]),
            (size_ - index) * sizeof(ObjectType));
    CreateObjects(&data_ptr_[index], &data_ptr_[index + num], std::forward<ArgsType>(args)...);
    size_ = new_size;
    return Iterator(&data_ptr_[index]);
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
ZVector<ObjectType, kIfInitializeObject>::Iterator ZVector<ObjectType, kIfInitializeObject>::Inserts(
        const Iterator iterator, IndexType num, ArgsType&&... args) noexcept {
    IndexType new_size = size_ + num;
    IndexType index = iterator.object_ptr() - data_ptr_;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(&data_ptr_[index + num]), reinterpret_cast<Void*>(&data_ptr_[index]),
            (size_ - index) * sizeof(ObjectType));
    CreateObjects(&data_ptr_[index], &data_ptr_[index + num], std::forward<ArgsType>(args)...);
    size_ = new_size;
    return Iterator(&data_ptr_[index]);
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
ZVector<ObjectType, kIfInitializeObject>::ReverseIterator ZVector<ObjectType, kIfInitializeObject>::Inserts(
        const ReverseIterator iterator, IndexType num, ArgsType&&... args) noexcept {
    IndexType new_size = size_ + num;
    IndexType index = iterator.object_ptr() - data_ptr_ + 1;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    memmove(reinterpret_cast<Void*>(&data_ptr_[index + num]), reinterpret_cast<Void*>(&data_ptr_[index]),
        (size_ - index) * sizeof(ObjectType));
    CreateObjects(&data_ptr_[index], &data_ptr_[index + num], std::forward<ArgsType>(args)...);
    size_ = new_size;
    return Iterator(&data_ptr_[index]);
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::Erase(const IndexType index) noexcept {
    DestroyObject(index);
    memmove(reinterpret_cast<Void*>(&data_ptr_[index]), reinterpret_cast<Void*>(&data_ptr_[index + 1]),
            (size_ - index - 1) * sizeof(ObjectType));
    --size_;
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::Erase(const Iterator iterator) noexcept {
    IndexType index = iterator.object_ptr() - data_ptr_;
    DestroyObject(index);
    memmove(reinterpret_cast<Void*>(&data_ptr_[index]), reinterpret_cast<Void*>(&data_ptr_[index + 1]),
            (size_ - index - 1) * sizeof(ObjectType));
    --size_;
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::Erases(const IndexType index, const IndexType num) noexcept {
    DestroyObjects(&data_ptr_[index], num);
    memmove(reinterpret_cast<Void*>(&data_ptr_[index]), reinterpret_cast<Void*>(&data_ptr_[index + num]),
            (size_ - index - num) * sizeof(ObjectType));
    size_ -= num;
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::Erases(const Iterator iterator, const IndexType num) noexcept {
    IndexType index = iterator.object_ptr() - data_ptr_;
    DestroyObjects(iterator.object_ptr(), num);
    memmove(reinterpret_cast<Void*>(&data_ptr_[index]), reinterpret_cast<Void*>(&data_ptr_[index + num]), 
            (size_ - index - num) * sizeof(ObjectType));
    size_ -= num;
}
template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::Erases(const Iterator begin, const Iterator end) noexcept {
    DestroyObjects(begin.object_ptr(), end.object_ptr());
    memmove(reinterpret_cast<Void*>(begin.object_ptr()), reinterpret_cast<Void*>(end.object_ptr()),
            (size_ - (end.object_ptr() - data_ptr_)) * sizeof(ObjectType));
    size_ -= end.object_ptr() - begin.object_ptr();
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfInitializeObject>::Emplace(const IndexType index, ArgsType&&... args) noexcept {
    DestroyObject(index);
    CreateObject(index, std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfInitializeObject>::Emplace(Iterator iterator, ArgsType&&... args) noexcept {
    DestroyObject(iterator.object_ptr());
    CreateObject(iterator.object_ptr(), std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfInitializeObject>::Emplace(ReverseIterator iterator, ArgsType&&... args) noexcept {
    DestroyObject(iterator.object_ptr());
    CreateObject(iterator.object_ptr(), std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfInitializeObject>::Assign(const IndexType num, ArgsType&&... args) noexcept {
    IndexType new_size = num;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    DestroyObjects(data_ptr_, size_);
    CreateObjects(data_ptr_, num, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::Assign(const Iterator begin, const Iterator end) noexcept {
    IndexType new_size = end - begin;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    DestroyObjects(data_ptr_, size_);
    CreateObjects(data_ptr_, begin.object_ptr(), end.object_ptr());
    size_ = new_size;
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::Clear() noexcept {
    DestroyObjects(data_ptr_, data_ptr_ + size_);
    size_ = 0;
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::ExtendCapacity(const IndexType capacity) noexcept {
    MemoryType current_memory_size = capacity_ * sizeof(ObjectType);
    MemoryType need_memory_size = capacity * sizeof(ObjectType);
    MemoryType apply_mrmory_size;
    //The current memory piece doesn't have enough memory.
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

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::MoveDestroy() {
    data_ptr_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObject(const IndexType index, 
                                                                               ArgsType&&... args) {
    if (sizeof...(ArgsType) == 0) {
        if constexpr (kIfInitializeObject) {
            new(reinterpret_cast<Void*>(&(data_ptr_[index]))) ObjectType();
        }
    }
    else {
        new(reinterpret_cast<Void*>(&(data_ptr_[index]))) ObjectType(std::forward<ArgsType>(args)...);
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObject(ObjectType* const object_ptr,
                                                                        ArgsType&&... args) {
    if (sizeof...(ArgsType) == 0) {
        if constexpr (kIfInitializeObject) {
            new(reinterpret_cast<Void*>(object_ptr)) ObjectType();
        }
    }
    else {
        new(reinterpret_cast<Void*>(object_ptr)) ObjectType(std::forward<ArgsType>(args)...);
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::DestroyObject(const IndexType index) {
    if constexpr (kIfInitializeObject) {
        data_ptr_[index].~ObjectType();
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::DestroyObject(ObjectType* const object_ptr) {
    if constexpr (kIfInitializeObject) {
        (*object_ptr).~ObjectType();
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* begin_ptr,
                                                                         ObjectType* const end_ptr,
                                                                         ArgsType&&... args) {
    if (sizeof...(ArgsType) == 0) {
        if constexpr (kIfInitializeObject) {
            new(reinterpret_cast<Void*>(begin_ptr)) ObjectType[end_ptr - begin_ptr];
        }
    }
    else {
        while (begin_ptr < end_ptr) {
            new(reinterpret_cast<Void*>(begin_ptr)) ObjectType(std::forward<ArgsType>(args)...);
            ++begin_ptr;
        }
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* begin_ptr, 
                                                                         const IndexType num,
                                                                         ArgsType&&... args) {
    if (sizeof...(ArgsType) == 0) {
        if constexpr (kIfInitializeObject) {
            new(reinterpret_cast<Void*>(begin_ptr)) ObjectType[num];
        }
    }
    else {
        ObjectType* end_ptr = begin_ptr + num;
        while (begin_ptr < end_ptr) {
            new(reinterpret_cast<Void*>(begin_ptr)) ObjectType(std::forward<ArgsType>(args)...);
            ++begin_ptr;
        }
    }

    if constexpr (kIfInitializeObject) { 
        new(reinterpret_cast<Void*>(begin_ptr)) ObjectType[num];
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* dst_ptr, 
                                                                         const ObjectType* src_begin_ptr, 
                                                                         const ObjectType* const src_end_ptr) {
    if constexpr (kIfInitializeObject) {
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

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* dst_ptr, const ObjectType* src_ptr,
                                                                         const IndexType num) {

    if constexpr (kIfInitializeObject) {
        ObjectType* end_ptr = dst_ptr + num;
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

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr,
                                                                       const IndexType num) {
    if constexpr (kIfInitializeObject) {
        ObjectType* end_ptr;
        if (size_ < num) {
            end_ptr = dst_ptr + size_;
            while (dst_ptr < end_ptr) {
                *dst_ptr = *src_ptr;
                ++dst_ptr;
                ++src_ptr;
            }
            end_ptr = end_ptr + (num - size_);
            CreateObjects(dst_ptr, end_ptr, src_ptr);
        }
        else {
            end_ptr = dst_ptr + num;
            while (dst_ptr < end_ptr) {
                *dst_ptr = *src_ptr;
                ++dst_ptr;
                ++src_ptr;
            }
            end_ptr = end_ptr + (size_ - num);
            DestroyObjects(dst_ptr, end_ptr);
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(dst_ptr), reinterpret_cast<Void*>(const_cast<ObjectType*>(src_ptr)),
               static_cast<SizeType>(num * sizeof(ObjectType)));
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::DestroyObjects(ObjectType* begin_ptr, 
                                                                          ObjectType* const end_ptr) {
    if constexpr (kIfInitializeObject) {
        while (begin_ptr < end_ptr) {
            begin_ptr->~ObjectType();
            ++begin_ptr;
        }
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::DestroyObjects(ObjectType* begin_ptr, 
                                                                          const IndexType num) {
    if constexpr (kIfInitializeObject) {
        ObjectType* end_ptr = begin_ptr + num;
        while (begin_ptr < end_ptr) {
            begin_ptr->~ObjectType();
            ++begin_ptr;
        }
    }
}

}//zengine

#endif // !Z_CORE_Z_VECTOR_H_