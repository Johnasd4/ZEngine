#ifndef Z_CORE_Z_SMART_PTR_H_
#define Z_CORE_Z_SMART_PTR_H_

#include<atomic>

#include"internal/drive.h"

#include"f_memory_pool.h"

namespace zengine {

template<typename ObjectType>
class ZUniquePtr;
template<typename ObjectType>
class ZSharedPtr;
template<typename ObjectType>
class ZWeakPtr;

namespace internal {

    static constexpr Bool kCounterThreadSafeDefault = false;

/*
    The smart pointer counter type.
    Template Parameters:
    - kCounterThreadSafe: Wheather the counter is thread safe of not.
*/
template<Bool kCounterThreadSafe>
struct ZSmartPtrCounter {};

template<>
class ZSmartPtrCounter<true> {
public:
    FORCEINLINE ZSmartPtrCounter() : used_count(1), weak_count(1) {}

    std::atomic<IndexType> used_count;
    std::atomic<IndexType> weak_count;
};

template<>
class ZSmartPtrCounter<false> {
public:
    FORCEINLINE ZSmartPtrCounter() : used_count(1), weak_count(1) {}

    IndexType used_count;
    IndexType weak_count;
};

/*
    The smart pointer's base type.
    Template Parameters:
    - kCounterThreadSafe: Wheather the counter is thread safe of not.
*/
template<typename ObjectType, Bool kCounterThreadSafe>
class ZSmartPtrBase {
protected:
    /*
        Calls the object's constructor
    */
    template<typename... Args>
    FORCEINLINE ZSmartPtrBase(Args&&... args);

    FORCEINLINE ~ZSmartPtrBase();

private:
    ZSmartPtrCounter<kCounterThreadSafe>* counter_ptr_;
    mutable ObjectType* object_ptr_;
};

template<typename ObjectType>
template<typename... Args>
FORCEINLINE ZSmartPtrBase<ObjectType>::ZSmartPtrBase(Args&&... args)
        : object_ptr_(reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory(sizeof(ObjectType)))) {
    //Only calls the constructor if it's a class object.
    if constexpr (std::is_class_v<ObjectType>) {
        new(reinterpret_cast<Address>(object_ptr_)) ObjectType(std::forward<Args>(args)...);
    }
}

template<typename ObjectType>
FORCEINLINE ZSmartPtrBase<ObjectType>::~ZSmartPtrBase() {
    //Only calls the desturctor if it's a class object.
    if constexpr (std::is_class_v<ObjectType>) {
        object_ptr_->~ObjectType();
    }
    memory_pool::ReleaseMemory(reinterpret_cast<Address>(object_ptr_));
}

}//internal

}//zengine



#endif // !Z_CORE_Z_SMART_PTR_H_
