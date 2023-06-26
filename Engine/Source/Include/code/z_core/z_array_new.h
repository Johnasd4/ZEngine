#ifndef Z_CORE_Z_ARRAY_H_
#define Z_CORE_Z_ARRAY_H_

#include"internal/drive.h"
#include"internal/z_container_base.h"
#include"internal/z_iterator_base.h"

namespace zengine {

/*
    An unfixed array, will auto extend size when used up.
*/
template<typename ObjectType, Bool kIfInitializeObject = kIsClass<ObjectType>>
class ZArray :public ZObject {
public:
    class Iterator : public internal::IteratorBase<ObjectType> {};
    class ConstIterator : public Iterator {};

    class ReverseIterator : public internal::IteratorBase<ObjectType> {};
    class ConstReverseIterator : public ReverseIterator {};

    FORCEINLINE ZArray();
    ZArray(const IndexType capacity) noexcept;
    ZArray(const ZArray& array) noexcept;
    ZArray(ZArray&& array) noexcept;

    ZArray& operator=(const ZArray& array) noexcept;
    ZArray& operator=(ZArray&& array) noexcept;

    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index);
    NODISCARD FORCEINLINE const ObjectType& operator[](const IndexType index) const;

    ~ZArray() noexcept;

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

    NODISCARD FORCEINLINE const IndexType size() const;
    NODISCARD FORCEINLINE const IndexType capacity() const;

    Void resize(const IndexType size);
    Void resize(const IndexType size, const ObjectType& object);
    Void set_capacity(const IndexType capacity);

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

    Void Erase(const IndexType index) noexcept;
    Void Erase(const IndexType index, const IndexType num) noexcept;
    Void Erase(const Iterator begin, const Iterator end) noexcept;
    Void Erase(const ReverseIterator begin, const ReverseIterator end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const IndexType index, ArgsType&&... args) noexcept;

    Void Assign(const IndexType num, const ObjectType& object) noexcept;
    Void Assign(const Iterator begin, const Iterator end) noexcept;
    Void Assign(const ReverseIterator begin, const ReverseIterator end) noexcept;

    FORCEINLINE Void Clear();

protected:
    using SuperType = ZObject;

private:
    /*
        Auto extends the capacity of the container.
    */
    FORCEINLINE Void AutoExtend() noexcept;

    /*
        Called when the container is moved.
    */
    FORCEINLINE Void MoveDestroy();

    /*
        Creates an object at the certain index. Will Call Constrctor if needed.
    */
    FORCEINLINE Void CreateObjectAtIndex(const IndexType index);
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
        if this object class's member kIfInitializeObject is
        true.
    */
    FORCEINLINE Void CreateObjects(ObjectType* start_ptr, const IndexType num);

    /*
        Initialize the address memory by the num given. Will call the constructor
        if this object class's member kIfInitializeObject is
        true.
    */
    template<typename... ArgsType>
    FORCEINLINE Void CreateObjects(ObjectType* start_ptr, const IndexType num, ArgsType&&... args);

    /*
        Copy objects by the address given.Will call the copy assignment operator
        if this object class's member kIfInitializeObject is true, 
        if the copy num is bigger then the current capacity ,the extras
        will call the copy constrctor instrad, make sure the memory is big
        enough before calling this function. Otherwise, will use memcpy instead.
    */
    FORCEINLINE Void CopyObjects(ObjectType* dst_ptr, const ObjectType* src_ptr, const IndexType num);

    /*
        Destroy the objects by the num given, starts at the address given. Will
        call the destrctor if this object class's member kIfInitializeObject is true.
    */
    FORCEINLINE Void DestroyObjects(ObjectType* start_ptr, const IndexType num); 

    ObjectType* data_ptr_;
    IndexType size_;
    IndexType capacity_;
};

}//zengine

#endif // !Z_CORE_Z_ARRAY_H_