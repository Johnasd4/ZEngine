#ifndef Z_CORE_Z_STACK_H_
#define Z_CORE_Z_STACK_H_

#include "internal/drive.h"
#include "m_error_message.h"
#include "z_object.h"

namespace zengine {

/*
    Stack caintainer.
    If kIfUnique is true, will call the constuctor when the object is created.
*/
template<typename ObjectType, Bool kIfUnique = kIsClass<ObjectType>>
class ZStack : public ZObject {
private:
    static constexpr Float32 kAutoExtendMulFactor = 1.5F;

public:
    ZStack() noexcept;
    ZStack(const IndexType capacity) noexcept;
    /*
        Fills the container by the object constructed by the arguements.
    */
    template<typename... ArgsType>
    ZStack(const IndexType capacity, ArgsType&&... args) noexcept;    
    ZStack(const ZStack& stack) noexcept;
    ZStack(ZStack&& stack) noexcept;

    ZStack& operator=(const ZStack& stack) noexcept;
    ZStack& operator=(ZStack&& stack) noexcept;

    NODISCARD FORCEINLINE const Bool operator==(const ZStack& stack) noexcept { 
        return data_ptr_ == stack.data_ptr_; 
    }    
    NODISCARD FORCEINLINE const Bool operator!=(const ZStack& stack) noexcept { 
        return data_ptr_ != stack.data_ptr_; 
    }

    ~ZStack() noexcept;

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

    NODISCARD FORCEINLINE const Bool IsEmpty() { return size_ == 0; }

    /*
        Resize the stack, If the given size is smaller then the current size,
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
        Remove the last object of the stack.
    */
    Void PopBack() noexcept;
    /*
        Remove the last object of the vector. Give the authority to the given address.
    */
    Void PopBack(ObjectType* object_ptr) noexcept;

    /*
        Create an object at the end of the stack by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the stack.
    */
    template<typename... ArgsType>
    Void PushBack(ArgsType&&... args) noexcept;
    /*
        Create objects at the end of the stack by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the stack.
    */
    template<typename... ArgsType>
    Void PushBacks(const IndexType num, ArgsType&&... args) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void EmplaceBack(ArgsType&&... args) noexcept;

    /*
        Construct the stack by filling it with the given amount of objects.
        The object is constructed by the arguements.
        If kIfUnique is false and no arguements, will only add the 
        size of the stack.
    */
    template<typename... ArgsType>
    Void Assign(const IndexType num, ArgsType&&... args) noexcept;

    /*
        Destroys all the objects in the stack, does not release the memory.
    */
    Void Clear() noexcept;
    /*
        Destroys all the objects in the stack, release the memory.
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
    IndexType capacity_;
    IndexType size_;
};

template<typename ObjectType, Bool kIfUnique>
ZStack<ObjectType, kIfUnique>::ZStack() noexcept
    : SuperType()
    , data_ptr_(nullptr)
    , capacity_(0)
    , size_(0)
{}

template<typename ObjectType, Bool kIfUnique>
ZStack<ObjectType, kIfUnique>::ZStack(const IndexType capacity) noexcept
    : SuperType()
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainer(capacity);
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
ZStack<ObjectType, kIfUnique>::ZStack(const IndexType capacity, ArgsType&&... args) noexcept
    : SuperType() 
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainer(capacity);
    CreateObjects(data_ptr_, capacity, std::forward<ArgsType>(args)...);
    size_ = capacity;
}

template<typename ObjectType, Bool kIfUnique>
ZStack<ObjectType, kIfUnique>::ZStack(const ZStack& stack) noexcept 
    : SuperType()
{
    CreateContainer(stack.size_);
    CreateAndCopyObjects(data_ptr_, stack.data_ptr_, stack.size_);
    size_ = stack.size_;
} 

template<typename ObjectType, Bool kIfUnique>
ZStack<ObjectType, kIfUnique>::ZStack(ZStack&& stack) noexcept 
    : SuperType()
    , data_ptr_(stack.data_ptr_)
    , capacity_(stack.capacity_)
    , size_(stack.size_)
{
    stack.MoveDestroy();
}

template<typename ObjectType, Bool kIfUnique>
ZStack<ObjectType, kIfUnique>& ZStack<ObjectType, kIfUnique>::operator=(
        const ZStack& stack) noexcept {
    DEBUG(&stack == this, "The source and the target of the copy is the same!");
    SuperType::operator=(stack);
    if (stack.size_ > capacity_) {
        ExtendContainer(stack.size_);
    }
    CopyObjects(data_ptr_, stack.data_ptr_, stack.size_);
    size_ = stack.size_;
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZStack<ObjectType, kIfUnique>& ZStack<ObjectType, kIfUnique>::operator=(
        ZStack&& stack) noexcept {
    DEBUG(&stack == this, "The source and the target of the copy is the same!");
    SuperType::operator=(std::forward<ZStack>(stack));
    DestroyObjects(data_ptr_, size_);
    data_ptr_ = stack.data_ptr_;
    size_ = stack.size_;
    capacity_ = stack.capacity_;
    stack.MoveDestroy();
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZStack<ObjectType, kIfUnique>::~ZStack() noexcept {
    DestroyContainer();
}

template<typename ObjectType, Bool kIfUnique>
Void ZStack<ObjectType, kIfUnique>::Resize(const IndexType size) noexcept {
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
Void ZStack<ObjectType, kIfUnique>::Resize(const IndexType size, ArgsType&&... args) noexcept {
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
Void ZStack<ObjectType, kIfUnique>::Reserve(const IndexType capacity) noexcept {
    if (capacity > capacity_) {
        ExtendContainer(capacity);
    }
}

template<typename ObjectType, Bool kIfUnique>
Void ZStack<ObjectType, kIfUnique>::PopBack() noexcept {
    DEBUG(size_ == 0, "No existing object to pop!");
    --size_;
    DestroyObject(size_);
}

template<typename ObjectType, Bool kIfUnique>
Void ZStack<ObjectType, kIfUnique>::PopBack(ObjectType* object_ptr) noexcept {
    DEBUG(size_ == 0, "No existing object to pop!");
    --size_;
    *object_ptr = std::move(data_ptr_[size_]);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZStack<ObjectType, kIfUnique>::PushBack(ArgsType&&... args) noexcept {
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObject(size_, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZStack<ObjectType, kIfUnique>::PushBacks(const IndexType num, ArgsType&&... args) noexcept {
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
Void ZStack<ObjectType, kIfUnique>::EmplaceBack(ArgsType&&... args) noexcept {
    DEBUG(size_ == 0, "No existing object to emplace!");
    DestroyObject(size_ - 1);
    CreateObject(size_ - 1, std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZStack<ObjectType, kIfUnique>::Assign(const IndexType num, ArgsType&&... args) noexcept {
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
Void ZStack<ObjectType, kIfUnique>::Clear() noexcept {
    DestroyObjects(data_ptr_, data_ptr_ + size_);
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
Void ZStack<ObjectType, kIfUnique>::Destroy() noexcept {
    DestroyContainer();
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::CreateContainer(const IndexType capacity) noexcept {
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    MemoryType need_memory_size = capacity * sizeof(ObjectType);
    MemoryType apply_mrmory_size;
    data_ptr_ = reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));
    capacity_ = apply_mrmory_size / sizeof(ObjectType);
}

template<typename ObjectType, Bool kIfUnique>
Void ZStack<ObjectType, kIfUnique>::ExtendContainer(const IndexType capacity) noexcept {
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
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::DestroyContainer() noexcept {
    DestroyObjects(data_ptr_, data_ptr_ + size_);
    memory_pool::ReleaseMemory(reinterpret_cast<Void*>(data_ptr_));
    data_ptr_ = nullptr;
    capacity_ = 0;
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::MoveDestroy() {
    data_ptr_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::CreateObject(const IndexType index, 
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
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::CreateObject(ObjectType* const object_ptr,
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
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::DestroyObject(const IndexType index) {
    if constexpr (kIfUnique) {
        data_ptr_[index].~ObjectType();
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::DestroyObject(ObjectType* const object_ptr) {
    if constexpr (kIfUnique) {
        (*object_ptr).~ObjectType();
    }
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::CreateObjects(ObjectType* begin_ptr,
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
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::CreateObjects(ObjectType* begin_ptr, 
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
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::CreateAndCopyObjects(ObjectType* dst_ptr, 
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
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::CreateAndCopyObjects(ObjectType* dst_ptr,
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
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::CreateAndCopyObjectsReverse(
        ObjectType* dst_ptr, const ObjectType* src_begin_ptr, const ObjectType* const src_end_ptr) {
    while (src_begin_ptr > src_end_ptr) {
        new(reinterpret_cast<Void*>(dst_ptr)) ObjectType(*src_begin_ptr);
        ++dst_ptr;
        --src_begin_ptr;
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::CopyObjects(ObjectType* dst_ptr, 
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
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::CopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr,
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
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::CopyObjectsReverse(ObjectType* dst_ptr,
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
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::DestroyObjects(ObjectType* begin_ptr, 
                                                                          ObjectType* const end_ptr) {
    if constexpr (kIfUnique) {
        while (begin_ptr < end_ptr) {
            begin_ptr->~ObjectType();
            ++begin_ptr;
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZStack<ObjectType, kIfUnique>::DestroyObjects(ObjectType* begin_ptr, 
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

#endif // !Z_CORE_Z_STACK_H_