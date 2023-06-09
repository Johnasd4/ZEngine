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
    __forceinline IteratorBase(const ObjectType* object_ptr) :object_ptr_(const_cast<ObjectType*>(object_ptr)) {}

    __forceinline IteratorBase& operator=(const IteratorBase& iterator) {
        object_ptr_ = iterator.object_ptr_;
        return *this;
    }

    __forceinline const Bool operator==(const IteratorBase& iterator) { return object_ptr_ == iterator.object_ptr_; }
    __forceinline const Bool operator!=(const IteratorBase& iterator) { return object_ptr_ != iterator.object_ptr_; }

    __forceinline ObjectType& operator*() { return *object_ptr_; }
    __forceinline ObjectType* operator->() { return object_ptr_; }

    __forceinline ~IteratorBase() {}

    __forceinline ObjectType* object_ptr() const { return object_ptr_; }
    __forceinline Void set_object_ptr(ObjectType* object_ptr) { object_ptr_ = object_ptr; }

private:
    ObjectType* object_ptr_;
};




}//internal
}//zengine

#endif // !Z_CORE_INTERNAL_Z_ITERATOR_BASE_H_