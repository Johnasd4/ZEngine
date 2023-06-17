#ifndef Z_CORE_Z_ARRAY_H_
#define Z_CORE_Z_ARRAY_H_

#include"internal/drive.h"
#include"internal/z_container_base.h"
#include"internal/z_iterator_base.h"

namespace zengine {

/*
    An unfixed array, will auto extend size when used up.
*/
template<typename ObjectType, Bool kIfInitializeObject = std::is_class_v<ObjectType>>
class ZArray :public internal::ZContainerBase<ObjectType, kIfInitializeObject> {
public:
    class Iterator : public internal::IteratorBase<ObjectType> {
    public:
        FORCEINLINE Iterator(const ObjectType* data_ptr) :SuperType(data_ptr) {} 

        FORCEINLINE Iterator& operator+(const IndexType data_num) {
            SuperType::set_data_ptr(SuperType::data_ptr() + data_num);
            return *this;
        }
        FORCEINLINE Iterator& operator-(const IndexType data_num) { 
            SuperType::set_data_ptr(SuperType::data_ptr() - data_num);
            return *this;
        }
        FORCEINLINE Iterator& operator++() {
            SuperType::set_data_ptr(SuperType::data_ptr() + 1);
            return *this;
        }
        FORCEINLINE Iterator& operator--() {
            SuperType::set_data_ptr(SuperType::data_ptr() - 1);
            return *this;
        }

        FORCEINLINE Iterator& operator-(const Iterator iterator) {
            return SuperType::data_ptr() - iterator.SuperType::data_ptr();
        }

    protected:
        using SuperType = internal::IteratorBase<ObjectType>;
    };
    class ConstIterator : public Iterator {
    public:
        FORCEINLINE ConstIterator(const ObjectType* data_ptr) :SuperType(data_ptr) {}

        FORCEINLINE const ObjectType& operator*() { return SuperType::SuperType::operator*(); }
        FORCEINLINE const ObjectType* operator->() { return SuperType::SuperType::operator->(); }

    protected:
        using SuperType = Iterator;
    };

    class ReverseIterator : public internal::IteratorBase<ObjectType> {
    public:
        FORCEINLINE ReverseIterator(const ObjectType* data_ptr) :SuperType(data_ptr) {}

        FORCEINLINE ReverseIterator& operator+(const IndexType data_num) {
            SuperType::set_data_ptr(SuperType::data_ptr() - data_num);
            return *this;
        }
        FORCEINLINE ReverseIterator& operator-(const IndexType data_num) {
            SuperType::set_data_ptr(SuperType::data_ptr() + data_num);
            return *this;
        }
        FORCEINLINE ReverseIterator& operator++() {
            SuperType::set_data_ptr(SuperType::data_ptr() - 1);
            return *this;
        }
        FORCEINLINE ReverseIterator& operator--() {
            SuperType::set_data_ptr(SuperType::data_ptr() + 1);
            return *this;
        }

        FORCEINLINE Iterator& operator-(const Iterator iterator) {
            return iterator.SuperType::data_ptr() - SuperType::data_ptr();
        }

    protected:
        using SuperType = internal::IteratorBase<ObjectType>;
    };
    class ConstReverseIterator : public ReverseIterator {
    public:
        FORCEINLINE ConstReverseIterator(const ObjectType* data_ptr) :SuperType(data_ptr) {}

        FORCEINLINE const ObjectType& operator*() { return SuperType::SuperType::operator*(); }
        FORCEINLINE const ObjectType* operator->() { return SuperType::SuperType::operator->(); }

    protected:
        using SuperType = ReverseIterator;
    };

    FORCEINLINE ZArray() : SuperType() {}
    FORCEINLINE ZArray(const IndexType init_capacity) : SuperType(init_capacity) {}
    FORCEINLINE ZArray(const ZArray& array) : SuperType(array) {}
    FORCEINLINE ZArray(ZArray&& array) : SuperType(std::forward<ZArray>(array)) {}

    FORCEINLINE ZArray& operator=(const ZArray& array) { 
        return reinterpret_cast<ZArray&>(SuperType::operator=(array));
    }
    FORCEINLINE ZArray& operator=(ZArray&& array) { 
        return reinterpret_cast<ZArray&>(SuperType::operator=(std::forward<ZArray>(array)));
    }

    FORCEINLINE ObjectType& operator()(const IndexType index) { return SuperType::operator()(index); }
    FORCEINLINE const ObjectType& operator()(const IndexType index) const { return SuperType::operator()(index); }

    FORCEINLINE ~ZArray() {}

    /*
        The iterator funcions.
    */
    FORCEINLINE Iterator Begin() { return Iterator(SuperType::data_ptr()); }
    FORCEINLINE ConstIterator ConstBegin() const { return ConstIterator(SuperType::data_ptr()); }
    FORCEINLINE ReverseIterator ReverseBegin() { return ReverseIterator(SuperType::end_data_ptr() - 1); }
    FORCEINLINE ConstReverseIterator ConstReverseBegin() const {
        return ConstReverseIterator(SuperType::end_data_ptr() - 1);
    }
    FORCEINLINE Iterator End() { return Iterator(SuperType::end_data_ptr()); }
    FORCEINLINE ConstIterator ConstEnd() const { return ConstIterator(SuperType::end_data_ptr()); }
    FORCEINLINE ReverseIterator ReverseEnd() { return ReverseIterator(SuperType::data_ptr() - 1); }
    FORCEINLINE ConstReverseIterator ConstReverseEnd() const {
        return ConstReverseIterator(SuperType::data_ptr() - 1);
    }

    FORCEINLINE const IndexType size() const { return SuperType::size(); }
    FORCEINLINE const IndexType capacity() const { return SuperType::capacity(); }
    FORCEINLINE Void set_size(const IndexType need_size) { 
        SuperType::set_size(need_size); 
        SuperType::set_end_data_ptr(SuperType::data_ptr() + need_size);
    }
    FORCEINLINE Void set_capacity(const IndexType need_capacity) { SuperType::set_capacity(need_capacity); }

    FORCEINLINE const Bool IfEmpty() { return SuperType::IfEmpty(); }

    /*
        Returns false if the array is empty.
    */
    NODISCARD const Bool Pop(ObjectType* object_ptr) noexcept;
    Void Push(const ObjectType& object) noexcept;
    Void PushEmpty(const IndexType data_num) noexcept;

    /*
        Returns false if out of bounds. Takes a lot of performance.
    */
    NODISCARD const Bool Insert(const IndexType insert_index, const ObjectType& object) noexcept;
    /*
        Returns false if out of bounds. Takes a lot of performance.
    */
    NODISCARD const Bool Remove(const IndexType remove_index) noexcept;

    FORCEINLINE Void Clear() { SuperType::Clear(); }

protected:
    using SuperType = internal::ZContainerBase<ObjectType, kIfInitializeObject>;
};

template<typename ObjectType, Bool kIfInitializeObject>
NODISCARD const Bool ZArray<ObjectType, kIfInitializeObject>::Pop(ObjectType* object_ptr) noexcept {
    if (SuperType::size() == 0) {
        return false;
    }
    SuperType::change_size(-1);
    object_ptr = SuperType::end_data_ptr();
    SuperType::set_end_data_ptr(SuperType::end_data_ptr() - 1);
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZArray<ObjectType, kIfInitializeObject>::Push(const ObjectType& object) noexcept {
    SuperType::change_size(1);
    *SuperType::end_data_ptr() = object;
    SuperType::set_end_data_ptr(SuperType::end_data_ptr() + 1);
}

template<typename ObjectType, Bool kIfInitializeObject>
Void ZArray<ObjectType, kIfInitializeObject>::PushEmpty(const IndexType data_num) noexcept {
    SuperType::change_size(data_num);
    SuperType::set_end_data_ptr(SuperType::end_data_ptr() + data_num);
}

template<typename ObjectType, Bool kIfInitializeObject>
NODISCARD const Bool ZArray<ObjectType, kIfInitializeObject>::Insert(
        const IndexType insert_index, const ObjectType& object) noexcept {
    //Out of bounds.
    if (insert_index > SuperType::size()) {
        return false;
    }
    SuperType::change_size(1);
    //Move the objects forward to insert.
    for (IndexType index = SuperType::size() - 1; index > insert_index; --index) {
        (*this)(index) = std::move((*this)(index - 1));
    }
    SuperType::CreateAndCopyObjectAtIndex(insert_index, object);
    SuperType::set_end_data_ptr(SuperType::end_data_ptr() + 1);
    return true;
}

template<typename ObjectType, Bool kIfInitializeObject>
NODISCARD const Bool ZArray<ObjectType, kIfInitializeObject>::Remove(const IndexType remove_index) noexcept {
    //Out of bounds.
    if (remove_index >= SuperType::size()) {
        return false;
    }
    SuperType::change_size(-1);
    SuperType::DestroyObjectAtIndex(remove_index);
    //Moves the objects backward.
    memcpy(reinterpret_cast<Address>(&(*this)(remove_index)), reinterpret_cast<Address>(&(*this)(remove_index + 1)), 
           static_cast<SizeType>(SuperType::size() - remove_index) * sizeof(ObjectType));
    //Replace the last object with a new object.
    SuperType::CreateObjectAtIndex(SuperType::size());
    SuperType::set_end_data_ptr(SuperType::end_data_ptr() - 1);
    return true;
}

}//zengine

#endif // !Z_CORE_Z_ARRAY_H_
