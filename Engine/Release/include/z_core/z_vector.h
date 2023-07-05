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
    FORCEINLINE VectorIteratorBase& operator=(const VectorIteratorBase& iterator) {
        object_ptr_ = iterator.object_ptr_;
        return *this;
    }
    FORCEINLINE VectorIteratorBase& operator=(VectorIteratorBase&& iterator) {
        object_ptr_ = iterator.object_ptr_;
        iterator.MoveDestroy();
        return *this;
    }

    NODISCARD FORCEINLINE const Bool operator==(const VectorIteratorBase& iterator) {
        return object_ptr_ == iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator!=(const VectorIteratorBase& iterator) {
        return object_ptr_ != iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>(const VectorIteratorBase& iterator) {
        return object_ptr_ > iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const VectorIteratorBase& iterator) {
        return object_ptr_ >= iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const VectorIteratorBase& iterator) {
        return object_ptr_ < iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const VectorIteratorBase& iterator) {
        return object_ptr_ <= iterator.object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const VectorIteratorBase iterator) const {
        return object_ptr_ - iterator.object_ptr_;
    }

    NODISCARD FORCEINLINE ObjectType& operator*() { return *object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* operator->() { return object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType& operator*() const { return *object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* operator->() const { return object_ptr_; }

    FORCEINLINE ~VectorIteratorBase() {}

    FORCEINLINE Void MoveDestroy() { object_ptr_ = nullptr; }

protected:
    mutable ObjectType* object_ptr_;
};


template<typename ObjectType>
class VectorIterator : public VectorIteratorBase<ObjectType> {
public:
    FORCEINLINE VectorIterator& operator+(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE const VectorIterator& operator+(const IndexType data_num) const {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE VectorIterator& operator-(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE const VectorIterator& operator-(const IndexType data_num) const {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE VectorIterator& operator++() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE const VectorIterator& operator++() const {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE VectorIterator& operator--() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE const VectorIterator& operator--() const {
        --SuperType::object_ptr_;
        return *this;
    }

protected:
    using SuperType = VectorIteratorBase<ObjectType>;
};

template<typename ObjectType>
class VectorReverseIterator : public VectorIteratorBase<ObjectType> {
public:
    FORCEINLINE VectorReverseIterator& operator+(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE const VectorReverseIterator& operator+(const IndexType data_num) const {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE VectorReverseIterator& operator-(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE const VectorReverseIterator& operator-(const IndexType data_num) const {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE VectorReverseIterator& operator++() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE const VectorReverseIterator& operator++() const {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE VectorReverseIterator& operator--() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE const VectorReverseIterator& operator--() const {
        ++SuperType::object_ptr_;
        return *this;
    }

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
    using IteratorType = internal::VectorIterator<ObjectType>;
    using ReverseIteratorType = internal::VectorReverseIterator<ObjectType>;

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
    NODISCARD FORCEINLINE IteratorType Begin() { return IteratorType(data_ptr_); }
    NODISCARD FORCEINLINE const IteratorType ConstBegin() const { return IteratorType(data_ptr_); }
    NODISCARD FORCEINLINE ReverseIteratorType ReverseBegin() { return ReverseIteratorType(data_ptr_ + size_ - 1); }
    NODISCARD FORCEINLINE const ReverseIteratorType ConstReverseBegin() const { 
        return ReverseIteratorType(data_ptr_ + size_ - 1); 
    }
    NODISCARD FORCEINLINE IteratorType End() { return IteratorType(data_ptr_ + size_); }
    NODISCARD FORCEINLINE const IteratorType ConstEnd() { return IteratorType(data_ptr_ + size_); }
    NODISCARD FORCEINLINE ReverseIteratorType ReverseEnd() { return ReverseIteratorType(data_ptr_ - 1); }
    NODISCARD FORCEINLINE const ReverseIteratorType ConstReverseEnd() const { 
        return ReverseIteratorType(data_ptr_ - 1); 
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

    Void PopBack() noexcept;

    Void PushBack(const ObjectType& object) noexcept;
    Void PushBack(const IndexType num, const ObjectType& object) noexcept;
    Void PushBack(ObjectType&& object) noexcept;

    Void PushBackEmpty(const IndexType num) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void EmplaceBack(ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the interator that points at the newest object.
    */
    IteratorType Insert(const IndexType index, const ObjectType& object) noexcept;
    /*
        Inserts before the index. Returns the interator that points at the newest object.
    */
    IteratorType Insert(const IndexType index, const IndexType num, const ObjectType& object) noexcept;
    /*
        Inserts before the index. Returns the interator that points at the newest object.
    */
    IteratorType Insert(const IndexType index, ObjectType&& object) noexcept;
    /*
        Inserts before the iterator. Returns the interator that points at the newest object.
    */
    IteratorType Insert(const IteratorType iterator, const ObjectType& object) noexcept;
    /*
        Inserts before the iterator. Returns the interator that points at the newest object.
    */
    IteratorType Insert(const IteratorType iterator, const IndexType num, const ObjectType& object) noexcept;
    /*
        Inserts before the iterator. Returns the interator that points at the newest object.
    */
    IteratorType Insert(const IteratorType iterator, ObjectType&& object) noexcept;

    Void Erase(const IndexType index) noexcept;
    Void Erase(const IndexType index, const IndexType num) noexcept;
    Void Erase(const IteratorType iterator) noexcept;
    Void Erase(const IteratorType iterator, const IndexType num) noexcept;
    Void Erase(const IteratorType begin, const IteratorType end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const IndexType index, ArgsType&&... args) noexcept;
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const IteratorType iterator, ArgsType&&... args) noexcept;

    Void Assign(const IndexType num, const ObjectType& object) noexcept;
    Void Assign(const IteratorType begin, const IteratorType end) noexcept;

    Void Sort();
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(CompareFunction&& compare_function);        
    Void Sort(const IteratorType begin, const IteratorType end);
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const IteratorType begin, const IteratorType end, CompareFunction&& compare_function);

    FORCEINLINE Void Clear();

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
    FORCEINLINE Void CreateObjectAtIndex(const IndexType index);
    /*
        Creates an object at the certain index. Will always call the Constrctor.
    */
    template<typename... ArgsType>
    FORCEINLINE Void CreateObjectAtIndex(const IndexType index, ArgsType&&... args);

    /*
        Destroys an object at the certain index. Will Call Destrctor if needed.
    */
    FORCEINLINE Void DestroyObjectAtIndex(const IndexType index);

    /*
        Initialize the address memory by the given address([begin, end)). 
        Will call the constructor if this object class's member kIfInitializeObject is true.
    */
    FORCEINLINE Void CreateObjects(ObjectType* begin_ptr, const ObjectType* end_ptr);
    /*
        Initialize the address memory by the given address([begin, begin + num)),
        Will call the constructor if this object class's member kIfInitializeObject is true.
    */
    FORCEINLINE Void CreateObjects(ObjectType* begin_ptr, const IndexType num);

    /*
        Initialize the address memory by the given address([begin, end)).
        Will call the copy constructor.
    */
    FORCEINLINE Void CreateObjects(ObjectType* begin_ptr, const ObjectType* end_ptr, ObjectType* src_ptr);

    /*
        Initialize the address memory by the given address([begin + num)).
        Will call the copy constructor.
    */
    FORCEINLINE Void CreateObjects(ObjectType* begin_ptr, ObjectType* src_ptr, const IndexType num);

    /*
        Initialize the address memory by the given address([begin, end)) and 
        call the constructor with the given arguements.
    */
    template<typename... ArgsType>
    FORCEINLINE Void CreateObjects(ObjectType* begin_ptr, const ObjectType* end_ptr, ArgsType&&... args);

    /*
        Copy objects by the address given. Will call the copy assignment operator
        if this object class's member kIfInitializeObject is true. 
    */
    FORCEINLINE Void CopyObjects(ObjectType* dst_ptr, ObjectType* src_ptr, const IndexType num);

    /*
        Destroy the objects by the given address([begin, end)). 
        Will call the destrctor if this object class's member kIfInitializeObject is true.
    */
    FORCEINLINE Void DestroyObjects(ObjectType* begin_ptr, const ObjectType* end_ptr);

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
ZVector<ObjectType, kIfInitializeObject>::ZVector(const ZVector& vector) noexcept
    : size_(vector.size_)
{
    ExtendCapacity(vector.size_);
    CreateObjects(vector.data_ptr_, vector.data_ptr_ + vector.size_, vector.data_ptr_);
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
        memory_pool::ReleaseMemory(reinterpret_cast<Address>(data_ptr_));
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
        DestroyObjectAtIndex(size_);
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::PushBack(const ObjectType& object) noexcept {
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjectAtIndex(size_, object);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::PushBack(const IndexType num, const ObjectType& object) noexcept {
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjects(data_ptr_+ size_, data_ptr_ + new_size, object);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::PushBack(ObjectType&& object) noexcept {
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    data_ptr_[size_] = std::forward<ObjectType>(object);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::PushBackEmpty(const IndexType num) noexcept {
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendCapacity(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjects(data_ptr_ + size_, num);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfInitializeObject>::EmplaceBack(ArgsType&&... args) noexcept {
    DestroyObjectAtIndex(size_ - 1);
    CreateObjectAtIndex(size_ - 1, std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZVector<ObjectType, kIfInitializeObject>::ExtendCapacity(const IndexType capacity) noexcept {
    MemoryType current_memory_size = capacity_ * sizeof(ObjectType);
    MemoryType need_memory_size = capacity * sizeof(ObjectType);
    MemoryType apply_mrmory_size;
    //The current memory piece doesn't have enough memory.
    if (!memory_pool::CheckMemory(reinterpret_cast<Address>(data_ptr_), need_memory_size, &apply_mrmory_size)) {
        ObjectType* temp_data_ptr = 
            reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));
        memcpy(reinterpret_cast<Address>(temp_data_ptr), reinterpret_cast<Address>(data_ptr_),
               size_ * sizeof(ObjectType));
        memory_pool::ReleaseMemory(reinterpret_cast<Address>(data_ptr_));
        data_ptr_ = temp_data_ptr;
    }
    capacity_ = apply_mrmory_size / sizeof(ObjectType);  
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::MoveDestroy() {
    data_ptr_ == nullptr;
    size_ = 0;
    capacity_ = 0;
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjectAtIndex(const IndexType index) {
    if constexpr (kIfInitializeObject) {
        new(reinterpret_cast<Address>(&(data_ptr_[index]))) ObjectType();
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjectAtIndex(const IndexType index, 
                                                                               ArgsType&&... args) {
    new(reinterpret_cast<Address>(&(data_ptr_[index]))) ObjectType(std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::DestroyObjectAtIndex(const IndexType index) {
    if constexpr (kIfInitializeObject) {
        data_ptr_[index].~ObjectType();
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* begin_ptr, 
                                                                         const ObjectType* end_ptr) {
    if constexpr (kIfInitializeObject) { 
        new(reinterpret_cast<Address>(begin_ptr)) ObjectType[end_ptr - begin_ptr];
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* begin_ptr, 
                                                                         const IndexType num) {
    if constexpr (kIfInitializeObject) { 
        new(reinterpret_cast<Address>(begin_ptr)) ObjectType[num];
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* begin_ptr, 
                                                                         const ObjectType* end_ptr, 
                                                                         ObjectType* src_ptr) {
    if constexpr (kIfInitializeObject) {
        while (begin_ptr < end_ptr) {
            new(reinterpret_cast<Address>(begin_ptr)) ObjectType(*src_ptr);
            ++begin_ptr;
            ++src_ptr;
        }
    }
    else {
        memcpy(reinterpret_cast<Address>(begin_ptr), reinterpret_cast<Address>(const_cast<ObjectType*>(src_ptr)),
            static_cast<SizeType>((end_ptr - begin_ptr) * sizeof(ObjectType)));
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* begin_ptr, 
                                                                         ObjectType* src_ptr,
                                                                         const IndexType num) {

    if constexpr (kIfInitializeObject) {
        ObjectType* end_ptr = begin_ptr + num;
        while (begin_ptr < end_ptr) {
            new(reinterpret_cast<Address>(begin_ptr)) ObjectType(*src_ptr);
            ++begin_ptr;
            ++src_ptr;
        }
    }
    else {
        memcpy(reinterpret_cast<Address>(begin_ptr), reinterpret_cast<Address>(const_cast<ObjectType*>(src_ptr)),
            static_cast<SizeType>(num * sizeof(ObjectType)));
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* begin_ptr, 
                                                                         const ObjectType* end_ptr,
                                                                         ArgsType&&... args) {
    while (begin_ptr < end_ptr) {
        new(reinterpret_cast<Address>(begin_ptr)) ObjectType(std::forward<ArgsType>(args)...);
        ++begin_ptr;
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CopyObjects(ObjectType* dst_ptr, ObjectType* src_ptr, 
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
        memcpy(reinterpret_cast<Address>(dst_ptr), reinterpret_cast<Address>(const_cast<ObjectType*>(src_ptr)),
               static_cast<SizeType>(num * sizeof(ObjectType)));
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::DestroyObjects(ObjectType* begin_ptr, 
                                                                          const ObjectType* end_ptr) {
    if constexpr (kIfInitializeObject) {
        while (begin_ptr < end_ptr) {
            begin_ptr->~ObjectType();
            ++begin_ptr;
        }
    }
}

}//zengine

#endif // !Z_CORE_Z_VECTOR_H_