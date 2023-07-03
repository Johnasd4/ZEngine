#ifndef Z_CORE_INTERNAL_Z_ITERATOR_BASE_H_
#define Z_CORE_INTERNAL_Z_ITERATOR_BASE_H_

#include"drive.h"
#include"..\f_memory_pool.h"
#include"..\z_object.h"

namespace zengine {
namespace internal {

/*
    The base class of the containers iterator.
*/
template<typename ObjectType>
class IteratorBase {
public:
    FORCEINLINE IteratorBase(const ObjectType* object_ptr) :object_ptr_(const_cast<ObjectType*>(object_ptr)) {}

    FORCEINLINE IteratorBase& operator=(const IteratorBase& iterator) {
        object_ptr_ = iterator.object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE const Bool operator==(const IteratorBase& iterator) { 
        return object_ptr_ == iterator.object_ptr_; 
    }
    NODISCARD FORCEINLINE const Bool operator!=(const IteratorBase& iterator) { 
        return object_ptr_ != iterator.object_ptr_; 
    }

    NODISCARD FORCEINLINE ObjectType& operator*() { return *object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* operator->() { return object_ptr_; }

    FORCEINLINE ~IteratorBase() {}

    NODISCARD FORCEINLINE ObjectType* object_ptr() const { return object_ptr_; }
    FORCEINLINE Void set_object_ptr(ObjectType* object_ptr) { object_ptr_ = object_ptr; }

private:
    ObjectType* object_ptr_;
};

}//internal
}//zengine

#endif // !Z_CORE_INTERNAL_Z_ITERATOR_BASE_H_