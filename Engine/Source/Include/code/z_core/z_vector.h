#ifndef Z_CORE_Z_VECTOR_H_
#define Z_CORE_Z_VECTOR_H_

#include"internal/drive.h"

namespace zengine {

/*
    Vector caintainer,
    If kIfInitializeObject is true, will call the constuctor when the object is created.
*/
template<typename ObjectType, Bool kIfInitializeObject = kIsClass<ObjectType>>
class ZVector :public ZObject {
public:
    class Iterator;
    class ReverseIterator;

    class Iterator {
        FORCEINLINE Iterator(const ObjectType* object_ptr) : object_ptr_(object_ptr) {}

        FORCEINLINE Iterator& operator+(const IndexType data_num) {
            object_ptr_ += data_num;
            return *this;
        }
        FORCEINLINE const Iterator& operator+(const IndexType data_num) const {
            object_ptr_ += data_num;
            return *this;
        }
        FORCEINLINE Iterator& operator-(const IndexType data_num) {
            object_ptr_ -= data_num;
            return *this;
        }
        FORCEINLINE const Iterator& operator-(const IndexType data_num) const {
            object_ptr_ -= data_num;
            return *this;
        }
        FORCEINLINE Iterator& operator++() {
            ++object_ptr_;
            return *this;
        }
        FORCEINLINE const Iterator& operator++() const {
            ++object_ptr_;
            return *this;
        }
        FORCEINLINE Iterator& operator--() {
            --object_ptr_;
            return *this;
        }
        FORCEINLINE const Iterator& operator--() const {
            --object_ptr_;
            return *this;
        }

        FORCEINLINE const IndexType operator-(const Iterator iterator) const { 
            return object_ptr_ - iterator.object_ptr_; 
        }

        FORCEINLINE ~Iterator() {}

    private:
        mutable ObjectType* object_ptr_;
    };

    class ReverseIterator {
        FORCEINLINE ReverseIterator(const ObjectType* object_ptr) : object_ptr_(object_ptr) {}

        FORCEINLINE ReverseIterator& operator+(const IndexType data_num) {
            object_ptr_ += data_num;
            return *this;
        }
        FORCEINLINE const ReverseIterator& operator+(const IndexType data_num) const {
            object_ptr_ += data_num;
            return *this;
        }
        FORCEINLINE ReverseIterator& operator-(const IndexType data_num) {
            object_ptr_ -= data_num;
            return *this;
        }
        FORCEINLINE const ReverseIterator& operator-(const IndexType data_num) const {
            object_ptr_ -= data_num;
            return *this;
        }
        FORCEINLINE ReverseIterator& operator++() {
            ++object_ptr_;
            return *this;
        }
        FORCEINLINE const ReverseIterator& operator++() const {
            ++object_ptr_;
            return *this;
        }
        FORCEINLINE ReverseIterator& operator--() {
            --object_ptr_;
            return *this;
        }
        FORCEINLINE const ReverseIterator& operator--() const {
            --object_ptr_;
            return *this;
        }

        FORCEINLINE const IndexType operator-(const ReverseIterator iterator) const {
            return object_ptr_ - iterator.object_ptr_;
        }

        FORCEINLINE ~ReverseIterator() {}

    private:
        mutable ObjectType* object_ptr_;
    };

    FORCEINLINE ZVector();
    ZVector(const IndexType capacity) noexcept;
    ZVector(const ZVector& array) noexcept;
    ZVector(ZVector&& array) noexcept;

    ZVector& operator=(const ZVector& array) noexcept;
    ZVector& operator=(ZVector&& array) noexcept;

    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index) { return data_ptr_[index]; }
    NODISCARD FORCEINLINE const ObjectType& operator[](const IndexType index) const { return data_ptr_[index]; }

    ~ZVector() noexcept;

    /*
        The iterator funcions.
    */
    NODISCARD FORCEINLINE Iterator Begin();
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const;
    NODISCARD FORCEINLINE ReverseIterator ReverseBegin();
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseBegin() const;
    NODISCARD FORCEINLINE Iterator End();
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const;
    NODISCARD FORCEINLINE ReverseIterator ReverseEnd();
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseEnd() const;

    NODISCARD FORCEINLINE const IndexType size() const { return size_; }
    NODISCARD FORCEINLINE const IndexType capacity() const { return capacity_; }
    NODISCARD FORCEINLINE ObjectType* data_ptr() { return data_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* data_ptr() const { return data_ptr_; }

    /*
        Resize the vector, If the given size is smaller then the current size,
        the function will cut remove the extra objects. If the given size is
        bigger then the current size, the function will fill the extra size with
        the default value of ObjectType.
    */
    Void Resize(const IndexType size);
    /*
        Resize the vector, If the given size is smaller then the current size,
        the function will cut remove the extra objects. If the given size is
        bigger then the current size, the function will fill the extra size with
        the given object type.
    */
    Void Resize(const IndexType size, const ObjectType& object);

    /*
        Will extend the capacity by the given capacity, if the smaller then the
        given capacity, this function will do nothing, other the capacity will
        entend to a size that equals or bigger then the given capacity.
    */
    Void Reserve(const IndexType capacity);

    NODISCARD FORCEINLINE ObjectType& At(const IndexType index);
    NODISCARD FORCEINLINE const ObjectType& At(const IndexType index) const;
    NODISCARD FORCEINLINE ObjectType& Front();
    NODISCARD FORCEINLINE const ObjectType& Front() const;
    NODISCARD FORCEINLINE ObjectType& Back();
    NODISCARD FORCEINLINE const ObjectType& Back() const;

    NODISCARD FORCEINLINE const Bool IfEmpty();

    Void PopBack(ObjectType* object_ptr) noexcept;

    Void PushBack(const ObjectType& object) noexcept;
    Void PushBack(const IndexType num, const ObjectType& object) noexcept;
    Void PushBack(ObjectType&& object) noexcept;

    Void PushBackEmpty(const IndexType object) noexcept;
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void EmplaceBack(ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the interator that points at the newest object.
    */
    Iterator Insert(const IndexType index, const ObjectType& object) noexcept;
    /*
        Inserts before the index. Returns the interator that points at the newest object.
    */
    Iterator Insert(const IndexType index, const IndexType num, const ObjectType& object) noexcept;
    /*
        Inserts before the index. Returns the interator that points at the newest object.
    */
    Iterator Insert(const IndexType index, ObjectType&& object) noexcept;
    /*
        Inserts before the iterator. Returns the interator that points at the newest object.
    */
    Iterator Insert(const Iterator iterator, const ObjectType& object) noexcept;
    /*
        Inserts before the iterator. Returns the interator that points at the newest object.
    */
    Iterator Insert(const Iterator iterator, const IndexType num, const ObjectType& object) noexcept;
    /*
        Inserts before the iterator. Returns the interator that points at the newest object.
    */
    Iterator Insert(const Iterator iterator, ObjectType&& object) noexcept;

    Void Erase(const IndexType index) noexcept;
    Void Erase(const IndexType index, const IndexType num) noexcept;
    Void Erase(const Iterator iterator) noexcept;
    Void Erase(const Iterator iterator, const IndexType num) noexcept;
    Void Erase(const Iterator begin, const Iterator end) noexcept;

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

    Void Assign(const IndexType num, const ObjectType& object) noexcept;
    Void Assign(const Iterator begin, const Iterator end) noexcept;

    Void Sort();
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(CompareFunction&& compare_function);        
    Void Sort(const Iterator begin, const Iterator end);
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const Iterator begin, const Iterator end, CompareFunction&& compare_function);

    FORCEINLINE Void Clear();

protected:
    using SuperType = ZObject;

private:
    /*
        Changes the size, checks if the vector is full.
    */
    FORCEINLINE Void ChangeSize(const IndexType offset) noexcept;

    /*
        Auto extends the memory by the current size.
    */
    Void AutoExtend() noexcept;

    /*
        Extends the memory to the given size.
    */
    Void ExtendMemory(const MemoryType size) noexcept;

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
        Initialize the address memory by the given address([begin,end)). 
        Will call the constructor if this object class's member kIfInitializeObject is true.
    */
    FORCEINLINE Void CreateObjects(ObjectType* begin_ptr, ObjectType* end_ptr);

    /*
        Initialize the address memory by the given address([begin,end)) and object.
    */
    FORCEINLINE Void CreateObjects(ObjectType* begin_ptr, ObjectType* end_ptr, const ObjectType& object);

    /*
        Copy objects by the address given. Will call the copy assignment operator
        if this object class's member kIfInitializeObject is true. 
    */
    FORCEINLINE Void CopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr, const IndexType num);

    /*
        Destroy the objects by the given address([begin,end)). 
        Will call the destrctor if this object class's member kIfInitializeObject is true.
    */
    FORCEINLINE Void DestroyObjects(ObjectType* begin_ptr, ObjectType* end_ptr);

    ObjectType* data_ptr_;
    IndexType size_;
    IndexType capacity_;
};

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE ZVector<ObjectType, kIfInitializeObject>::ZVector()
    : data_ptr_(nullptr)
    , size_(0)
    , capacity_(0)
{}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjectAtIndex(const IndexType index) {
    if constexpr (kIfInitializeObject) {
        new(reinterpret_cast<Address>(&(data_ptr_[index]))) ObjectType();
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjectAtIndex(const IndexType index, ArgsType&&... args) {
    new(reinterpret_cast<Address>(&(data_ptr_[index]))) ObjectType(std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::DestroyObjectAtIndex(const IndexType index) {
    if constexpr (kIfInitializeObject) {
        data_ptr_[index].~ObjectType();
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* begin_ptr, ObjectType* end_ptr) {
    if constexpr (kIfInitializeObject) { 
        while (begin_ptr != end_ptr) {
            new(reinterpret_cast<Address>(begin_ptr)) ObjectType();
            ++begin_ptr;
        }
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* begin_ptr, ObjectType* end_ptr,
    const ObjectType& object) {
    while (begin_ptr != end_ptr) {
        new(reinterpret_cast<Address>(begin_ptr)) ObjectType(object);
        ++begin_ptr;
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::CopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr, 
                                                                       const IndexType num) {
    if constexpr (kIfInitializeObject) {
        for (IndexType index = 0; index < num; ++index) {
            *dst_ptr = *src_ptr;
            ++dst_ptr;
            ++src_ptr;
        }
    }
    else {
        memcpy(reinterpret_cast<Address>(dst_ptr), reinterpret_cast<Address>(const_cast<ObjectType*>(src_ptr)),
               static_cast<SizeType>(num * sizeof(ObjectType)));
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZVector<ObjectType, kIfInitializeObject>::DestroyObjects(ObjectType* begin_ptr, ObjectType* end_ptr) {
    if constexpr (kIfInitializeObject) {
        while (begin_ptr != end_ptr) {
            begin_ptr->~ObjectType();
            ++begin_ptr;
        }
    }
}

}//zengine

#endif // !Z_CORE_Z_VECTOR_H_