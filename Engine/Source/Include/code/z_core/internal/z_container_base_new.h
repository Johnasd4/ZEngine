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
template<typename ObjectType, Bool kIfInitializeObject>
class ZContainerBase : public ZObject {
private:
    static constexpr IndexType kDefaultCapacity = 10;
    static constexpr Float32 kAutoExtendMulFactor = 1.2F;

protected:
    using SuperType = ZObject;

    FORCEINLINE ZContainerBase() {}
    FORCEINLINE ZContainerBase(const ZContainerBase& container) noexcept : SuperType(container) {}
    FORCEINLINE ZContainerBase(ZContainerBase&& container) noexcept : SuperType(container) { MoveDestroy(); }

    FORCEINLINE ZContainerBase& operator=(const ZContainerBase& container) { return SuperType::operator=(container); }
    FORCEINLINE ZContainerBase& operator=(ZContainerBase&& container) { 
        MoveDestroy();
        return SuperType::operator=(container); 
    }

    FORCEINLINE ~ZContainerBase() {}

    /*
        Creates an object at the certain index. Will Call Constrctor if needed.
    */
    template<typename... ArgsType>
    FORCEINLINE Void CreateObjectAtIndex(const IndexType index, ArgsType&&... args);

    /*
        Destroys an object at the certain index. Will Call Destrctor if needed.
    */
    FORCEINLINE Void DestroyObjectAtIndex(const IndexType index);

    /*
        Initialize the address memory by the num given. Will call the constructor
        if this object class's member kIfInitializeObject is true.
    */
    FORCEINLINE Void CreateObjects(ObjectType* start_ptr, const IndexType create_object_num);

    /*
        Initialize the address memory by the num given. Will call the constructor
        if this object class's member kIfInitializeObject is true.
    */
    template<typename... ArgsType>
    FORCEINLINE Void CreateObjects(ObjectType* start_ptr, const IndexType create_object_num, ArgsType&&... args);

    /*
        Destroy the objects by the num given, starts at the address given. Will
        call the destrctor if this object class's member kIfInitializeObject is true.
    */
    FORCEINLINE Void DestroyObjects(ObjectType* start_ptr, const IndexType destroy_object_num);

    /*
        Copy objects by the address given.Will call the copy assignment operator
        if this object class's member kIfInitializeObject is true, 
        if the copy num is bigger then the current capacity ,the extras
        will call the copy constrctor instrad, make sure the memory is big
        enough before calling this function. Otherwise, will use memcpy instead.
    */
    FORCEINLINE Void CopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr, const IndexType copy_object_num);

    /*
        Initialize the address by the num given. Will call the copy constructor
        if this object class's member kIfInitializeObject is true. 
        Otherwise, will use memcpy instead.
    */
    FORCEINLINE Void CreateAndCopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr, 
                                            const IndexType copy_object_num);

private:
    /*
        Called when the container is moved.
    */
    FORCEINLINE Void MoveDestroy() {}
};

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::CreateObjectAtIndex(const IndexType index, 
                                                                                      ArgsType&&... args) {
    if constexpr (kIfInitializeObject) {
        new(reinterpret_cast<Address>(&((*this)(index)))) ObjectType(std::forward<ArgsType>(args)...);
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::DestroyObjectAtIndex(const IndexType index) {
    if constexpr (kIfInitializeObject) {
        (*this)(index).~ObjectType();
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* start_ptr,
                                                                                const IndexType create_object_num) {
    if constexpr (kIfInitializeObject) {
        new(reinterpret_cast<Address>(start_ptr)) ObjectType[create_object_num];
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
template<typename... ArgsType>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::CreateObjects(ObjectType* start_ptr, 
                                                                                const IndexType create_object_num, 
                                                                                ArgsType&&... args) {
    if constexpr (kIfInitializeObject) {
        for (IndexType index = 0; index < create_object_num; ++index) {
            new(reinterpret_cast<Address>(start_ptr)) ObjectType(std::forward<ArgsType>(args)...);
            ++start_ptr;
        }
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::DestroyObjects(ObjectType* start_ptr, 
                                                                                 const IndexType destroy_object_num) {
    if constexpr (kIfInitializeObject) {
        for (IndexType index = 0; index < destroy_object_num; ++index) {
            start_ptr->~ObjectType();
            ++start_ptr;
        }
    }
}

template<typename ObjectType, Bool kIfInitializeObject>
FORCEINLINE Void ZContainerBase<ObjectType, kIfInitializeObject>::CopyObjects(ObjectType* dst_ptr, 
                                                                              const ObjectType* src_ptr, 
                                                                              const IndexType copy_object_num) {
    if constexpr (kIfInitializeObject) {
        for (IndexType index = 0; index < copy_object_num; ++index) {
            *dst_ptr = *src_ptr;
            ++dst_ptr;
            ++src_ptr;
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
    if constexpr (kIfInitializeObject) {
        for (IndexType index = 0; index < copy_object_num; ++index) {
            //Calls the copy constructor. 
            new(reinterpret_cast<Address>(dst_ptr))
                ObjectType(reinterpret_cast<Address>(const_cast<ObjectType*>(src_ptr)));
            ++dst_ptr;
            ++src_ptr;
        }
    }
    else {
        memcpy(reinterpret_cast<Address>(dst_ptr), reinterpret_cast<Address>(const_cast<ObjectType*>(src_ptr)),
               static_cast<SizeType>(copy_object_num) * sizeof(ObjectType));
    }
}

}//internal
}//zengine

#endif // !Z_CORE_INTERNAL_Z_CONTAINER_BASE_H_