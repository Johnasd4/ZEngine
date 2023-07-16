#ifndef Z_CORE_Z_QUEUE_H_
#define Z_CORE_Z_QUEUE_H_

#include "internal/drive.h"

#include "m_error_message.h"
#include "z_object.h"

namespace zengine {

/*
    Queue caintainer.
    If kIfUnique is true, will call the constuctor when the object is created.
*/
template<typename ObjectType, Bool kIfUnique = kIsClass<ObjectType>>
class ZQueue : public ZObject {
private:
    static constexpr Float32 kAutoExtendMulFactor = 1.5F;

public:
    ZQueue() noexcept;
    ZQueue(const IndexType capacity) noexcept;
    /*
        Fills the container by the object constructed by the arguements.
    */
    template<typename... ArgsType>
    ZQueue(const IndexType capacity, ArgsType&&... args) noexcept;    
    ZQueue(const ZQueue& queue) noexcept;
    ZQueue(ZQueue&& queue) noexcept;

    ZQueue& operator=(const ZQueue& queue) noexcept;
    ZQueue& operator=(ZQueue&& queue) noexcept;

    NODISCARD FORCEINLINE const Bool operator==(const ZQueue& queue) noexcept { 
        return data_ptr_ == queue.data_ptr_; 
    }    
    NODISCARD FORCEINLINE const Bool operator!=(const ZQueue& queue) noexcept { 
        return data_ptr_ != queue.data_ptr_; 
    }

    ~ZQueue() noexcept;

    NODISCARD FORCEINLINE ObjectType& Front() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[front_ptr_];
    }
    NODISCARD FORCEINLINE const ObjectType& Front() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[front_ptr_];
    }
    NODISCARD FORCEINLINE ObjectType& Back() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[back_ptr_];
    }
    NODISCARD FORCEINLINE const ObjectType& Back() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[back_ptr_];
    }

    NODISCARD FORCEINLINE const IndexType size() const { return size_; }
    NODISCARD FORCEINLINE const IndexType capacity() const { return capacity_; }

    NODISCARD FORCEINLINE const Bool IsEmpty() { return size_ == 0; }

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
        Remove the first object of the queue.
    */
    Void PopFront() noexcept;
    /*
        Remove the first object of the queue. Give the authority to the given address.
    */
    Void PopFront(ObjectType* object_ptr) noexcept;

    /*
        Create an object at the end of the queue by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the queue.
    */
    template<typename... ArgsType>
    Void PushBack(ArgsType&&... args) noexcept;
    /*
        Create objects at the end of the queue by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the queue.
    */
    template<typename... ArgsType>
    Void PushBacks(const IndexType num, ArgsType&&... args) noexcept;

    /*
        Construct the queue by filling it with the given amount of objects.
        The object is constructed by the arguements.
        If kIfUnique is false and no arguements, will only add the 
        size of the queue.
    */
    template<typename... ArgsType>
    Void Assign(const IndexType num, ArgsType&&... args) noexcept;

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
    ObjectType* front_ptr_;
    ObjectType* back_ptr_;
    IndexType capacity_;
    IndexType size_;
};

template<typename ObjectType, Bool kIfUnique>
ZQueue<ObjectType, kIfUnique>::ZQueue() noexcept
    : SuperType()
    , data_ptr_(nullptr)
    , front_ptr_(nullptr)
    , back_ptr_(nullptr)
    , capacity_(0)
    , size_(0)
{}

template<typename ObjectType, Bool kIfUnique>
ZQueue<ObjectType, kIfUnique>::ZQueue(const IndexType capacity) noexcept
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
ZQueue<ObjectType, kIfUnique>::ZQueue(const IndexType capacity, ArgsType&&... args) noexcept
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
ZQueue<ObjectType, kIfUnique>::ZQueue(const ZQueue& queue) noexcept 
    : SuperType()
{
    CreateContainer(queue.size_);
    CreateAndCopyObjects(data_ptr_, queue.data_ptr_, queue.size_);
    size_ = queue.size_;
    front_ptr_ = data_ptr_;
    back_ptr_ = data_ptr_ + size_ - 1;
} 

template<typename ObjectType, Bool kIfUnique>
ZQueue<ObjectType, kIfUnique>::ZQueue(ZQueue&& queue) noexcept 
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
ZQueue<ObjectType, kIfUnique>& ZQueue<ObjectType, kIfUnique>::operator=(
        const ZQueue& queue) noexcept {
    DEBUG(&queue == this, "The source and the target of the copy is the same!");
    SuperType::operator=(queue);
    if (queue.size_ > capacity_) {
        ExtendContainer(queue.size_);
    }
    CopyObjects(data_ptr_, vector.data_ptr_, vector.size_);
    size_ = queue.size_;
    front_ptr_ = data_ptr_;
    back_ptr_ = data_ptr_ + size_ - 1;
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZQueue<ObjectType, kIfUnique>& ZQueue<ObjectType, kIfUnique>::operator=(
        ZQueue&& queue) noexcept {
    DEBUG(&queue == this, "The source and the target of the copy is the same!");
    SuperType::operator=(std::forward<ZQueue>(queue));
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
ZQueue<ObjectType, kIfUnique>::~ZQueue() noexcept {
    DestroyContainer();
}

template<typename ObjectType, Bool kIfUnique>
Void ZQueue<ObjectType, kIfUnique>::Resize(const IndexType size) noexcept {
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
Void ZQueue<ObjectType, kIfUnique>::Resize(const IndexType size, ArgsType&&... args) noexcept {
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
Void ZQueue<ObjectType, kIfUnique>::Reserve(const IndexType capacity) noexcept {
    if (capacity > capacity_) {
        ExtendContainer(capacity);
    }
}

template<typename ObjectType, Bool kIfUnique>
Void ZQueue<ObjectType, kIfUnique>::PopFront() noexcept {
    DEBUG(size_ == 0, "No existing object to pop!");
    --size_;
    DestroyObject(size_);
}

template<typename ObjectType, Bool kIfUnique>
Void ZQueue<ObjectType, kIfUnique>::PopFront(ObjectType* object_ptr) noexcept {
    DEBUG(size_ == 0, "No existing object to pop!");
    --size_;
    *object_ptr = std::move(data_ptr_[size_]);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZQueue<ObjectType, kIfUnique>::PushBack(ArgsType&&... args) noexcept {
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObject(size_, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZQueue<ObjectType, kIfUnique>::PushBacks(const IndexType num, ArgsType&&... args) noexcept {
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
Void ZQueue<ObjectType, kIfUnique>::Assign(const IndexType num, ArgsType&&... args) noexcept {
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
Void ZQueue<ObjectType, kIfUnique>::Clear() noexcept {
    DestroyObjects(data_ptr_, data_ptr_ + size_);
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
Void ZQueue<ObjectType, kIfUnique>::Destroy() noexcept {
    DestroyContainer();
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::CreateContainer(const IndexType capacity) noexcept {
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    MemoryType need_memory_size = capacity * sizeof(ObjectType);
    MemoryType apply_mrmory_size;
    data_ptr_ = reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));
    capacity_ = apply_mrmory_size / sizeof(ObjectType);
}

template<typename ObjectType, Bool kIfUnique>
Void ZQueue<ObjectType, kIfUnique>::ExtendContainer(const IndexType capacity) noexcept {
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
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::DestroyContainer() noexcept {
    DestroyObjects(front_ptr_, back_ptr_ + 1);
    memory_pool::ReleaseMemory(reinterpret_cast<Void*>(data_ptr_));
    data_ptr_ = nullptr;
    front_ptr_ = nullptr;
    back_ptr_ = nullptr;
    capacity_ = 0;
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::MoveDestroy() {
    data_ptr_ = nullptr;
    front_ptr_ = nullptr;
    back_ptr_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::CreateObject(const IndexType index, 
                                                                               ArgsType&&... args) {
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
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::CreateObject(ObjectType* const object_ptr,
                                                                        ArgsType&&... args) {
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
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::DestroyObject(const IndexType index) {
    if constexpr (kIfUnique) {
        data_ptr_[index].~ObjectType();
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::DestroyObject(ObjectType* const object_ptr) {
    if constexpr (kIfUnique) {
        (*object_ptr).~ObjectType();
    }
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::CreateObjects(ObjectType* begin_ptr,
                                                                         ObjectType* const end_ptr,
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
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::CreateObjects(ObjectType* begin_ptr, 
                                                                         const IndexType num,
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
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::CreateAndCopyObjects(ObjectType* dst_ptr, 
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
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::CreateAndCopyObjects(ObjectType* dst_ptr,
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
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::CreateAndCopyObjectsReverse(
        ObjectType* dst_ptr, const ObjectType* src_begin_ptr, const ObjectType* const src_end_ptr) {
    while (src_begin_ptr > src_end_ptr) {
        new(reinterpret_cast<Void*>(dst_ptr)) ObjectType(*src_begin_ptr);
        ++dst_ptr;
        --src_begin_ptr;
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::CopyObjects(ObjectType* dst_ptr, 
                                                                       const ObjectType* src_begin_ptr,
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
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::CopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr,
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
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::CopyObjectsReverse(ObjectType* dst_ptr,
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
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::DestroyObjects(ObjectType* begin_ptr, 
                                                                          ObjectType* const end_ptr) {
    if constexpr (kIfUnique) {
        while (begin_ptr < end_ptr) {
            begin_ptr->~ObjectType();
            ++begin_ptr;
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZQueue<ObjectType, kIfUnique>::DestroyObjects(ObjectType* begin_ptr, 
                                                                          const IndexType num) {
    if constexpr (kIfUnique) {
        ObjectType* end_ptr = begin_ptr + num;
        while (begin_ptr < end_ptr) {
            begin_ptr->~ObjectType();
            ++begin_ptr;
        }
    }
}

}//zengine


#endif // !Z_CORE_Z_QUEUE_H_