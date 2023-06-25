#ifndef Z_CORE_INTERNAL_Z_CONTAINER_BASE_H_
#define Z_CORE_INTERNAL_Z_CONTAINER_BASE_H_

#include<type_traits>

#include"drive.h"
#include"..\f_memory_pool.h"
#include"..\z_object.h"

namespace zengine {
namespace internal {

/*
    The base class of the containers.
    Property:
        data_ptr_ : The pointer that points at the first object.
        end_data_ptr_; The pointer that points at the object after the last.
        size_; The num of the objects in the container.
        capacity_; The max num of object the container can contain.  
*/
template<typename ObjectType, Bool kIfInitializeObject, typename Allocator>
class ZContainerBase : public ZObject {
private:
    static constexpr IndexType kDefaultCapacity = 10;
    static constexpr Float32 kAutoExtendMulFactor = 1.2F;

protected:
    using SuperType = ZObject;

    ZContainerBase() noexcept;
    ZContainerBase(const IndexType init_capacity) noexcept;
    /*
        Does not release the memory of src_data_ptr.
    */
    ZContainerBase(const ObjectType* src_data_ptr, const IndexType data_size) noexcept;
    ZContainerBase(const ZContainerBase& container) noexcept;
    ZContainerBase(ZContainerBase&& container) noexcept;

    ZContainerBase& operator=(const ZContainerBase& container);
    ZContainerBase& operator=(ZContainerBase&& container);

    NODISCARD FORCEINLINE ObjectType& operator()(const IndexType index) { return data_ptr_[index]; }
    NODISCARD FORCEINLINE const ObjectType& operator()(const IndexType index) const { return data_ptr_[index]; }

    ~ZContainerBase() noexcept;

    NODISCARD FORCEINLINE ObjectType* data_ptr() const { return data_ptr_; }
    NODISCARD FORCEINLINE ObjectType* end_data_ptr() const { return end_data_ptr_; }

    NODISCARD FORCEINLINE const IndexType size() const { return size_; }
    NODISCARD FORCEINLINE const IndexType capacity() const { return capacity_; }

    FORCEINLINE Void set_end_data_ptr(ObjectType* end_data_ptr) { end_data_ptr_ = end_data_ptr; }

    /*
        Change the size to a cetain size. This will auto extend the container
        if needed. Specialize for string type.
    */
    Void set_size(const IndexType need_size) { ((size_ = need_size) > capacity_) ? AutoExtend() : Void(); }
    /*
        Changes the size by offset based on the current size. This will auto
        extend the container if needed.
    */
    Void change_size(const IndexType offset) { ((size_ += offset) > capacity_) ? AutoExtend() : Void(); }
    /*
        Changes the capacity to a certain size. Will call the constrctor or
        destrctor if needed.
    */
    Void set_capacity(const IndexType capacity) noexcept;

    NODISCARD FORCEINLINE const Bool IfEmpty() { return size_ == 0; }

    FORCEINLINE Void Clear() { 
        size_ = 0; 
        end_data_ptr_ = data_ptr_;
    }

    /*
        Creates an object at the certain index. Will Call Constrctor if needed.
    */
    FORCEINLINE Void CreateObjectAtIndex(const IndexType index);
    /*
        Copies an object at the certain index. Will Call Constrctor if needed.
    */
    FORCEINLINE Void CreateAndCopyObjectAtIndex(const IndexType index, const ObjectType& object);
    /*
        Destroys an object at the certain index. Will Call Destrctor if needed.
    */
    FORCEINLINE Void DestroyObjectAtIndex(const IndexType index);
    /*
        Initialize the address memory by the num given. Will call the constructor
        if this object class's member kIfInitializeObject is
        true.
    */
    FORCEINLINE Void CreateObjects(ObjectType* start_ptr, const IndexType create_object_num);
    /*
        Copy objects by the address given.Will call the copy assignment operator
        if this object class's member kIfInitializeObject is
        true, if the copy num is bigger then the current capacity ,the extras
        will call the copy constrctor instrad, make sure the memory is big
        enough before calling this function. Otherwise, will use memcpy instead.
    */
    FORCEINLINE Void CopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr, const IndexType copy_object_num);
    /*
        Initialize the address by the num given. Will call the copy constructor
        if this object class's member kIfInitializeObject is
        true. Otherwise, will use memcpy instead.
    */
    FORCEINLINE Void CreateAndCopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr, 
                                            const IndexType copy_object_num);
    /*
        Destroy the objects by the num given, starts at the address given. Will
        call the destrctor if this object class's member
        kIfInitializeObject is true.
    */
    FORCEINLINE Void DestroyObjects(ObjectType* start_ptr, const IndexType destroy_object_num);

    /*
        Extends the capacity of the container. Will extend more the the capacity
        needed.
    */
    FORCEINLINE Void AutoExtend() noexcept;

private:
    /*
        Only extends the memory piece's memory that the container uses. Do
        nothing to the caontainer.
    */
    Void ExtendMemory(const MemoryType need_memory_size) noexcept;

    /*
        Called when the container is moved.
    */
    FORCEINLINE Void MoveDestroy();

    ObjectType* data_ptr_;
    ObjectType* end_data_ptr_;
    IndexType size_;
    IndexType capacity_;
};

template<typename ObjectType, Bool kIfInitializeObject>
ZContainerBase<ObjectType, kIfInitializeObject>::ZContainerBase() noexcept
        : SuperType()
        , data_ptr_((ObjectType*)memory_pool::ApplyMemory(kDefaultCapacity * sizeof(ObjectType)))
        , end_data_ptr_(data_ptr_)
        , capacity_(kDefaultCapacity)
        , size_(0) {    
    CreateObjects(data_ptr_, kDefaultCapacity);
}

template<typename ObjectType, Bool kIfInitializeObject>
ZContainerBase<ObjectType, kIfInitializeObject>::ZContainerBase(const IndexType init_capacity) noexcept
        : SuperType()
        , data_ptr_((ObjectType*)memory_pool::ApplyMemory(init_capacity * sizeof(ObjectType)))
        , end_data_ptr_(data_ptr_)
        , capacity_(init_capacity)
        , size_(0) {
    CreateObjects(data_ptr_, init_capacity);
}

template<typename ObjectType, Bool kIfInitializeObject>
ZContainerBase<ObjectType, kIfInitializeObject>::ZContainerBase(const ObjectType* src_data_ptr,
                                                                const IndexType data_size) noexcept
        : SuperType()
        , data_ptr_((ObjectType*)memory_pool::ApplyMemory(data_size * sizeof(ObjectType)))
        , end_data_ptr_(data_ptr_ + data_size)
        , capacity_(data_size)
        , size_(data_size) {
    CreateAndCopyObjects(data_ptr_, src_data_ptr, data_size);
}

template<typename ObjectType, Bool kIfInitializeObject>
ZContainerBase<ObjectType, kIfInitializeObject>::ZContainerBase(const ZContainerBase& container) noexcept
        : SuperType(container)
        , data_ptr_((ObjectType*)memory_pool::ApplyMemory(container.capacity_ * sizeof(ObjectType)))
        , end_data_ptr_(data_ptr_ + container.size_)
        , capacity_(container.capacity_)
        , size_(container.size_) {
    CreateAndCopyObjects(data_ptr_, container.data_ptr_, container.capacity_);
}

template<typename ObjectType, Bool kIfInitializeObject>
ZContainerBase<ObjectType, kIfInitializeObject>::ZContainerBase(ZContainerBase&& container) noexcept
        : SuperType(std::forward<ZContainerBase>(container))
        , data_ptr_(container.data_ptr_)
        , end_data_ptr_(container.end_data_ptr_)
        , capacity_(container.capacity_)
        , size_(container.size_) {
    container.MoveDestroy();
}

/*
    Specialized to make the function faster.
*/
template<typename ObjectType, Bool kIfInitializeObject>
ZContainerBase<ObjectType, kIfInitializeObject>& ZContainerBase<ObjectType, kIfInitializeObject>::operator=(
        const ZContainerBase& container) {
    SuperType::operator=(container);
    //Makes sure the memory is big enough.
    if (container.capacity_ > capacity_) {
        //Only extends the memory.
        ExtendMemory(container.capacity_ * sizeof(ObjectType));
    }
    CopyObjects(data_ptr_, container.data_ptr_, container.capacity_);
    end_data_ptr_ = data_ptr_ + container.size_;
    capacity_ = container.capacity_;
    size_ = container.size_;
    return *this;
}

template<typename ObjectType, Bool kIfInitializeObject>
ZContainerBase<ObjectType, kIfInitializeObject>& ZContainerBase<ObjectType, kIfInitializeObject>::operator=(
        ZContainerBase&& container) {
    SuperType::operator=(std::forward<ZContainerBase>(container));
    memory_pool::ReleaseMemory(data_ptr_);
    data_ptr_ = container.data_ptr_;
    end_data_ptr_ = container.end_data_ptr_;
    capacity_ = container.capacity_;
    size_ = container.size_;
    container.MoveDestroy();
    return *this;
}

template<typename ObjectType, Bool kIfInitializeObject>
ZContainerBase<ObjectType, kIfInitializeObject>::~ZContainerBase() noexcept {
    if (data_ptr_ == nullptr) {
        return;
    }
    DestroyObjects(data_ptr_, capacity_);
    memory_pool::ReleaseMemory(reinterpret_cast<Address>(data_ptr_));
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZContainerBase<ObjectType, kIfInitializeObject>::set_capacity(const IndexType capacity) noexcept {
    MemoryType current_memory_size = capacity_ * sizeof(ObjectType);
    MemoryType need_memory_size = capacity * sizeof(ObjectType);
    ExtendMemory(need_memory_size);
    //Inicialize the objects if needed.
    CreateObjects(reinterpret_cast<ObjectType*>(reinterpret_cast<AddressType>(data_ptr_) + current_memory_size),
                  capacity - capacity_);
    //Destroy the extra objects if needed.
    DestroyObjects(data_ptr_ + capacity_, capacity - capacity_);
    capacity_ = capacity;
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::CreateObjectAtIndex(const IndexType index) {
    //The object class has member kIfInitializeObject.
    if constexpr (kIfInitializeObject) {
        new(reinterpret_cast<Address>(&((*this)(index)))) ObjectType();
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::CreateAndCopyObjectAtIndex(
        const IndexType index, const ObjectType& object) {
    //The object class has member kIfInitializeObject.
    if constexpr (kIfInitializeObject) {
        new(reinterpret_cast<Address>(&((*this)(index)))) ObjectType(object);
    }
    else {
        (*this)(index) = object;
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::DestroyObjectAtIndex(const IndexType index) {
    //The object class has member kIfInitializeObject.
    if constexpr (kIfInitializeObject) {
        (*this)(index).~ObjectType();
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* start_ptr, 
                                                                                  const IndexType create_object_num) {
    //The object class has member kIfInitializeObject.
    if constexpr (kIfInitializeObject) {
        new(reinterpret_cast<Address>(start_ptr)) ObjectType[create_object_num];
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::CopyObjects(
    ObjectType* dst_ptr, const ObjectType* src_ptr, const IndexType copy_object_num) {
    //The object class has member kIfInitializeObject.
    if constexpr (kIfInitializeObject) {
        if (capacity_ >= copy_object_num) {
            for (IndexType index = 0; index < copy_object_num; ++index) {
                *dst_ptr = *src_ptr;
                dst_ptr += 1;
                src_ptr += 1;
            }
        }
        else {
            //Copy
            for (IndexType index = 0; index < capacity_; ++index) {
                *dst_ptr = *src_ptr;
                dst_ptr += 1;
                src_ptr += 1;
            }
            //Call constructor
            for (IndexType index = capacity_; index < copy_object_num; ++index) {
                new(reinterpret_cast<Address>(dst_ptr)) ObjectType(reinterpret_cast<Address>(src_ptr));
                dst_ptr += 1;
                src_ptr += 1;
            }
        }
    }
    else {
        memcpy(reinterpret_cast<Address>(dst_ptr), reinterpret_cast<Address>(const_cast<ObjectType*>(src_ptr)),
            static_cast<SizeType>(copy_object_num * sizeof(ObjectType)));
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::CreateAndCopyObjects(
        ObjectType* dst_ptr, const ObjectType* src_ptr, const IndexType copy_object_num) {
    //The object class has member kIfInitializeObject.
    if constexpr (kIfInitializeObject) {
        for (IndexType index = 0; index < copy_object_num; ++index) {
            //Calls the copy constructor. 
            new(reinterpret_cast<Address>(dst_ptr))
                ObjectType(reinterpret_cast<Address>(const_cast<ObjectType*>(src_ptr)));
            dst_ptr += 1;
            src_ptr += 1;
        }
    }
    else {
        memcpy(reinterpret_cast<Address>(dst_ptr), reinterpret_cast<Address>(const_cast<ObjectType*>(src_ptr)),
               static_cast<SizeType>(copy_object_num) * sizeof(ObjectType));
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::DestroyObjects(
        ObjectType* start_ptr, const IndexType destroy_object_num) {
    //The object class has member kIfInitializeObject.
    if constexpr (kIfInitializeObject) {
        for (IndexType index = 0; index < destroy_object_num; ++index) {
            start_ptr->~ObjectType();
            start_ptr++;
        }
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::AutoExtend() noexcept {
    Float32 temp_capacity = static_cast<Float32>(capacity_);
    do {
        temp_capacity *= kAutoExtendMulFactor;
    } while (temp_capacity < size_);
    set_capacity(static_cast<IndexType>(temp_capacity));
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZContainerBase<ObjectType, kIfInitializeObject>::ExtendMemory(const MemoryType memory_size) noexcept {
    if (memory_pool::CheckMemory(memory_size, data_ptr_)) {
        return;
    }
    ObjectType* temp_data_ptr = reinterpret_cast<ObjectType*>(
        memory_pool::ApplyMemory(memory_size, reinterpret_cast<Address>(data_ptr_)));
    memcpy(reinterpret_cast<Address>(temp_data_ptr), reinterpret_cast<Address>(data_ptr_), 
           memory_size);
    memory_pool::ReleaseMemory(data_ptr_);
    end_data_ptr_ = temp_data_ptr + (end_data_ptr_ - data_ptr_);
    data_ptr_ = temp_data_ptr;
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::MoveDestroy() {
    data_ptr_ = nullptr;
    end_data_ptr_ = nullptr;
    capacity_ = 0;
    size_ = 0;
}

}//internal
}//zengine

#endif // !Z_CORE_INTERNAL_Z_CONTAINER_BASE_H_