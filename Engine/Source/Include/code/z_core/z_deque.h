#ifndef Z_CORE_Z_DEQUE_H_
#define Z_CORE_Z_DEQUE_H_

#include "internal/drive.h"

#include "m_error_message.h"
#include "z_object.h"

namespace zengine {

template<typename ObjectType, Bool kIfUnique = kIsClass<ObjectType>>
class ZDeque;

namespace internal {

template<typename ObjectType>
class DequeIteratorBase {
public:
    FORCEINLINE DequeIteratorBase(ObjectType* object_ptr) : object_ptr_(object_ptr) {}
    FORCEINLINE DequeIteratorBase(const DequeIteratorBase& iterator) : object_ptr_(iterator.object_ptr_) {}
    FORCEINLINE DequeIteratorBase(const DequeIteratorBase&& iterator) : object_ptr_(iterator.object_ptr_) {
        iterator.MoveDestroy();
    }

    FORCEINLINE DequeIteratorBase& operator=(ObjectType* object_ptr) {
        object_ptr_ = object_ptr;
        return *this;
    }
    FORCEINLINE DequeIteratorBase& operator=(const DequeIteratorBase& iterator) {
        object_ptr_ = iterator.object_ptr_;
        return *this;
    }
    FORCEINLINE DequeIteratorBase& operator=(DequeIteratorBase&& iterator) {
        object_ptr_ = iterator.object_ptr_;
        iterator.MoveDestroy();
        return *this;
    }

    NODISCARD FORCEINLINE const Bool operator==(const DequeIteratorBase& iterator) const {
        return object_ptr_ == iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator!=(const DequeIteratorBase& iterator) const {
        return object_ptr_ != iterator.object_ptr_;
    }

    FORCEINLINE ~DequeIteratorBase() {}

    FORCEINLINE Void MoveDestroy() { object_ptr_ = nullptr; }

protected:
     ObjectType* object_ptr_;
     ZDeque<ObjectType>* deque_ptr_;
};

template<typename ObjectType>
class DequeIterator : public DequeIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index) const { return SuperType::object_ptr_[index]; }
    NODISCARD FORCEINLINE ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE DequeIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE DequeIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }

    FORCEINLINE DequeIterator& operator++() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE DequeIterator& operator--() {
        --SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE DequeIterator operator+(const IndexType data_num) const {
        return DequeIterator(SuperType::object_ptr_ + data_num);
    }
    NODISCARD FORCEINLINE DequeIterator operator-(const IndexType data_num) const {
        return DequeIterator(SuperType::object_ptr_ - data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const DequeIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const DequeIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const DequeIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const DequeIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const DequeIterator& iterator) const {
        return static_cast<IndexType>(SuperType::object_ptr_ - iterator.SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = DequeIteratorBase<ObjectType>;
};

template<typename ObjectType>
class DequeReverseIterator : public DequeIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index) const { return SuperType::object_ptr_[-index]; }
    NODISCARD FORCEINLINE ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE DequeReverseIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE DequeReverseIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }

    FORCEINLINE DequeReverseIterator& operator++() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE DequeReverseIterator& operator--() {
        ++SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE DequeReverseIterator operator+(const IndexType data_num) const {
        return DequeReverseIterator(SuperType::object_ptr_ - data_num);
    }
    NODISCARD FORCEINLINE DequeReverseIterator operator-(const IndexType data_num) const {
        return DequeReverseIterator(SuperType::object_ptr_ + data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const DequeReverseIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const DequeReverseIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const DequeReverseIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const DequeReverseIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const DequeReverseIterator& iterator) const {
        return static_cast<IndexType>(iterator.SuperType::object_ptr_ - SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = DequeIteratorBase<ObjectType>;
};

}//internal

/*
    Double end queue caintainer.
    If kIfUnique is true, will call the constuctor when the object is created.
*/
template<typename ObjectType, Bool kIfUnique = kIsClass<ObjectType>>
class ZDeque : public ZObject {
private:
    static constexpr Float32 kAutoExtendMulFactor = 0.25F;

public:
    using IteratorType = internal::DequeIterator<ObjectType>;
    using ConstIteratorType = internal::DequeConstIterator<ObjectType>;
    using ReverseIteratorType = internal::DequeReverseIterator<ObjectType>;
    using ConstReverseIteratorType = internal::DequeConstReverseIterator<ObjectType>;

    ZDeque() noexcept;
    ZDeque(const IndexType capacity) noexcept;
    /*
        Fills the container by the object constructed by the arguements.
    */
    template<typename... ArgsType>
    ZDeque(const IndexType capacity, ArgsType&&... args) noexcept;    
    ZDeque(const IteratorType& begin, const IteratorType& end) noexcept;
    ZDeque(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept;
    ZDeque(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;
    ZDeque(const ConstReverseIteratorType& begin, const ConstReverseIteratorType& end) noexcept;
    ZDeque(const ZDeque& queue) noexcept;
    ZDeque(ZDeque&& queue) noexcept;

    ZDeque& operator=(const ZDeque& queue) noexcept;
    ZDeque& operator=(ZDeque&& queue) noexcept;

    NODISCARD FORCEINLINE const Bool operator==(const ZDeque& queue) noexcept;
    NODISCARD FORCEINLINE const Bool operator!=(const ZDeque& queue) noexcept;

    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index);
    NODISCARD FORCEINLINE const ObjectType& operator[](const IndexType index) const;

    ~ZDeque() noexcept;

    NODISCARD FORCEINLINE ObjectType& At(const IndexType index);
    NODISCARD FORCEINLINE const ObjectType& At(const IndexType index) const;
    NODISCARD FORCEINLINE ObjectType& Front();
    NODISCARD FORCEINLINE const ObjectType& Front() const;
    NODISCARD FORCEINLINE ObjectType& Back();
    NODISCARD FORCEINLINE const ObjectType& Back() const;

    NODISCARD FORCEINLINE const IndexType size() const { return size_; }
    NODISCARD FORCEINLINE const IndexType capacity() const { return capacity_; }

    NODISCARD FORCEINLINE const Bool IsEmpty();

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
        Shrinks the queue to the minimum capacity that can fit the current size.
    */
    Void ShrinkToFit() noexcept;

    /*
        Remove the object at the front of the queue.
    */
    Void PopFront() noexcept;
    /*
        Remove the object at the front of the queue.. Give the authority to the given address.
    */
    Void PopFront(ObjectType* object_ptr) noexcept;

    /*
        Create an object at the front of the queue by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the queue.
    */
    template<typename... ArgsType>
    Void PushFront(ArgsType&&... args) noexcept;
    /*
        Create objects at the front of the queue by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the queue.
    */
    template<typename... ArgsType>
    Void PushFronts(const IndexType num, ArgsType&&... args) noexcept;

    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the queue.
    */
    Void PushFronts(const IteratorType& begin, const IteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the queue.
    */
    Void PushFronts(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the queue.
    */
    Void PushFronts(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the queue.
    */
    Void PushFronts(const ConstReverseIteratorType& begin, const ConstReverseIteratorType& end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void EmplaceFront(ArgsType&&... args) noexcept;

    /*
        Remove the object at the back of the queue.
    */
    Void PopBack() noexcept;
    /*
        Remove the object at the back of the queue.. Give the authority to the given address.
    */
    Void PopBack(ObjectType* object_ptr) noexcept;

    /*
        Create an object at the back of the queue by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the queue.
    */
    template<typename... ArgsType>
    Void PushBack(ArgsType&&... args) noexcept;
    /*
        Create objects at the back of the queue by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the queue.
    */
    template<typename... ArgsType>
    Void PushBacks(const IndexType num, ArgsType&&... args) noexcept;

    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the queue.
    */
    Void PushBacks(const IteratorType& begin, const IteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the queue.
    */
    Void PushBacks(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the queue.
    */
    Void PushBacks(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the queue.
    */
    Void PushBacks(const ConstReverseIteratorType& begin, const ConstReverseIteratorType& end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void EmplaceBack(ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    IteratorType Insert(const IndexType index, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    IteratorType Insert(const IteratorType& iterator, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    ReverseIteratorType Insert(const ReverseIteratorType& iterator, ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    IteratorType Inserts(const IndexType index, IndexType num, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    IteratorType Inserts(const IteratorType& iterator, IndexType num, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    ReverseIteratorType Inserts(const ReverseIteratorType& iterator, IndexType num, ArgsType&&... args) noexcept;

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IndexType index,
        const IteratorType& src_begin, const IteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IndexType index,
        const ConstIteratorType& src_begin, const ConstIteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IndexType index,
        const ReverseIteratorType& src_begin, const ReverseIteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IndexType index,
        const ConstReverseIteratorType& src_begin, const ConstReverseIteratorType& src_end) noexcept;

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IteratorType& iterator,
        const IteratorType& src_begin, const IteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IteratorType& iterator,
        const ConstIteratorType& src_begin, const ConstIteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IteratorType& iterator,
        const ReverseIteratorType& src_begin, const ReverseIteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    IteratorType Inserts(const IteratorType& iterator,
        const ConstReverseIteratorType& src_begin, const ConstReverseIteratorType& src_end) noexcept;

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    ReverseIteratorType Inserts(const ReverseIteratorType& iterator,
        const IteratorType& src_begin, const IteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    ReverseIteratorType Inserts(const ReverseIteratorType& iterator,
        const ConstIteratorType& src_begin, const ConstIteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    ReverseIteratorType Inserts(const ReverseIteratorType& iterator,
        const ReverseIteratorType& src_begin, const ReverseIteratorType& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    ReverseIteratorType Inserts(const ReverseIteratorType& iterator,
        const ConstReverseIteratorType& src_begin,
        const ConstReverseIteratorType& src_end) noexcept;

    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    IteratorType Erase(const IndexType index) noexcept;
    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    IteratorType Erase(const IteratorType& iterator) noexcept;
    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    ReverseIteratorType Erase(const ReverseIteratorType& iterator) noexcept;

    /*
        Erases the num of objects that starts at the given index.
        Returns the iterator that points at the next object.
    */
    IteratorType Erases(const IndexType index, const IndexType num) noexcept;
    /*
        Erases the num of objects that starts at the given iterator.
        Returns the iterator that points at the next object.
    */
    IteratorType Erases(const IteratorType& iterator, const IndexType num) noexcept;
    /*
        Erases the num of objects that starts at the given iterator.
        Returns the iterator that points at the next object.
    */
    ReverseIteratorType Erases(const ReverseIteratorType& iterator, const IndexType num) noexcept;
    /*
        Erases the object between begin and end, involves begin, but don't involves end.
        Returns the iterator that points at the next object.
    */
    IteratorType Erases(const IteratorType& begin, const IteratorType& end) noexcept;
    /*
        Erases the object between begin and end, involves begin, but don't involves end.
        Returns the iterator that points at the next object.
    */
    ReverseIteratorType Erases(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const IndexType index, ArgsType&&... args) noexcept;
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const IteratorType& iterator, ArgsType&&... args) noexcept;
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const ReverseIteratorType& iterator, ArgsType&&... args) noexcept;

    /*
        Construct the queue by filling it with the given amount of objects.
        The object is constructed by the arguements.
        If kIfUnique is false and no arguements, will only add the 
        size of the queue.
    */
    template<typename... ArgsType>
    Void Assign(const IndexType num, ArgsType&&... args) noexcept;
    /*
        Construct the queue by filling it objects between the iterators.
    */
    Void Assign(const IteratorType& begin, const IteratorType& end) noexcept;
    /*
        Construct the queue by filling it objects between the iterators.
    */
    Void Assign(const ConstIteratorType& begin, const ConstIteratorType& end) noexcept;
    /*
        Construct the queue by filling it objects between the iterators.
    */
    Void Assign(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;
    /*
        Construct the queue by filling it objects between the iterators.
    */
    Void Assign(const ConstReverseIteratorType& begin, const ConstReverseIteratorType& end) noexcept;
    
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
    struct DataNode {
    public:
        DataNode* next_node_ptr;
        DataNode* previous_node_ptr;
        IndexType capacity;
        IndexType size;

        NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index) { 
            return reinterpret_cast<ObjectType*>(this + 1)[index];
        }
    };

    /*
        Creates an object at the certain place. Will call the Constrctor if needed.
    */
    template<typename... ArgsType>
    FORCEINLINE static Void CreateObjectP(ObjectType* const object_ptr, ArgsType&&... args);

    /*
        Destroys an object at the certain place. Will Call Destrctor if needed.
    */
    FORCEINLINE static Void DestroyObjectP(ObjectType* const object_ptr);

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the Constrctor if needed.
    */
    template<typename... ArgsType>
    inline static Void CreateObjectsP(ObjectType* begin_ptr, ObjectType* const end_ptr, ArgsType&&... args) noexcept;

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the copy constructor.
    */
    inline static Void CreateAndCopyObjectsP(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
                                             const ObjectType* const src_end_ptr) noexcept;

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the copy constructor.
    */
    static Void CreateAndCopyObjectsReverseP(ObjectType* dst_ptr, const ObjectType* src_begin_ptr,
                                            const ObjectType* const src_end_ptr) noexcept;

    /*
        Copy objects by the given pointer. Will call the copy assignment operator
        if this object class's member kIfUnique is true.
    */
    static Void CopyObjectsP(ObjectType* dst_ptr, DataNode* dst_node_ptr,
                             const ObjectType* src_begin_ptr, DataNode* src_begin_node_ptr,
                             const ObjectType* const src_end_ptr, DataNode* src_end_node_ptr) noexcept;

    /*
        Copy objects by the given pointer. Will call the copy assignment operator
        if this object class's member kIfUnique is true.
    */
    static Void CopyObjectsReverseP(ObjectType* dst_ptr, DataNode* dst_node_ptr,
                                    const ObjectType* src_begin_ptr, DataNode* src_begin_node_ptr,
                                    const ObjectType* const src_end_ptr, DataNode* src_end_node_ptr) noexcept;

    /*
        Destroy the objects by the given arguements([begin, end)). 
        Will call the destrctor if this object class's member kIfUnique is true.
    */
    inline static Void DestroyObjectsP(ObjectType* begin_ptr, ObjectType* const end_ptr) noexcept;

    /*
        Creates the capacity by the given capacity, the final capacity might
        not equal the given capacity.
    */
    Void CreateContainer(const IndexType capacity) noexcept;
    /*
        Extends the capacity by the given capacity, the final capacity might
        not equal the given capacity. Returns the data node pointer.
    */
    NODISCARD DataNode* const ExtendContainer(const IndexType capacity) noexcept;
    /*
        Shrinks the container to the minimum capacity that can fit the current size.
    */
    Void ShrinkContainer() noexcept;
    /*
        Destroys the container.
    */
    Void DestroyContainer() noexcept;

    /*
        Adds a new node to the front.
    */
    Void AddFrontNode() noexcept;
    /*
        Remove the front node.
    */
    Void RemoveFrontNode() noexcept;
    /*
        Adds a new node to the back.
    */
    Void AddBackNode() noexcept;
    /*
        Remove the back node.
    */
    Void RemoveBackNode() noexcept;

    /*
        Container move function.
    */
    FORCEINLINE Void MoveP(ZDeque&& vector);

    DataNode* front_node_ptr_;
    DataNode* back_node_ptr_;
    DataNode* empty_node_ptr_;
    IndexType capacity_;

    IndexType front_index_;
    IndexType back_index_;
    IndexType size_;
};

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::ZDeque() noexcept
    : SuperType()
    , front_node_ptr_(nullptr)
    , back_node_ptr_(nullptr)
    , empty_node_ptr_(nullptr)
    , capacity_(0)
    , front_index_(0)
    , back_index_(0)
    , size_(0)
{}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::ZDeque(const IndexType capacity) noexcept
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
ZDeque<ObjectType, kIfUnique>::ZDeque(const IndexType capacity, ArgsType&&... args) noexcept
    : SuperType() 
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainer(capacity);
    CreateObjectsP(data_ptr_, capacity, std::forward<ArgsType>(args)...);
    size_ = capacity;
    front_ptr_ = data_ptr_;
    back_ptr_ = data_ptr_ + size_ - 1;
}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::ZDeque(const ZDeque& queue) noexcept 
    : SuperType()
{
    CreateContainer(queue.size_);
    CreateAndCopyObjectsP(data_ptr_, queue.data_ptr_, queue.size_);
    size_ = queue.size_;
    front_ptr_ = data_ptr_;
    back_ptr_ = data_ptr_ + size_ - 1;
} 

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::ZDeque(ZDeque&& queue) noexcept 
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
ZDeque<ObjectType, kIfUnique>& ZDeque<ObjectType, kIfUnique>::operator=(
        const ZDeque& queue) noexcept {
    DEBUG(&queue == this, "The source and the target of the copy is the same!");
    SuperType::operator=(queue);
    if (queue.size_ > capacity_) {
        ExtendContainer(queue.size_);
    }
    CopyObjectsP(data_ptr_, queue.data_ptr_, queue.size_);
    size_ = queue.size_;
    front_ptr_ = data_ptr_;
    back_ptr_ = data_ptr_ + size_ - 1;
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>& ZDeque<ObjectType, kIfUnique>::operator=(
        ZDeque&& queue) noexcept {
    DEBUG(&queue == this, "The source and the target of the copy is the same!");
    SuperType::operator=(std::forward<ZDeque>(queue));
    DestroyObjectsP(data_ptr_, size_);
    data_ptr_ = queue.data_ptr_;
    front_ptr_ = queue.front_ptr_;
    back_ptr_ = queue.back_ptr_;
    size_ = queue.size_;
    capacity_ = queue.capacity_;
    queue.MoveDestroy();
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::~ZDeque() noexcept {
    DestroyContainer();
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::Resize(const IndexType size) noexcept {
    DEBUG(size < 0, "Negaive size is not valid!");
    if (size_ < size) {
        if (size > capacity_) {
            ExtendContainer(size);
        }
        CreateObjectsP(data_ptr_ + size_, size - size_);
    }
    else {
        DestroyObjectsP(data_ptr_ + size, data_ptr_ + size_);
    }
    size_ = size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZDeque<ObjectType, kIfUnique>::Resize(const IndexType size, ArgsType&&... args) noexcept {
    DEBUG(size < 0, "Negaive size is not valid!");
    if (size_ < size) {
        if (size > capacity_) {
            ExtendContainer(size);
        }
        CreateObjectsP(data_ptr_ + size_, size - size_, std::forward<ArgsType>(args)...);
    }
    else {
        DestroyObjectsP(data_ptr_ + size, data_ptr_ + size_);
    }
    size_ = size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::Reserve(const IndexType capacity) noexcept {
    if (capacity > capacity_) {
        ExtendContainer(capacity);
    }
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::PopFront() noexcept {
    DEBUG(size_ == 0, "No existing object to pop!");
    --size_;
    DestroyObjectP(size_);
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::PopFront(ObjectType* object_ptr) noexcept {
    DEBUG(size_ == 0, "No existing object to pop!");
    --size_;
    *object_ptr = std::move(data_ptr_[size_]);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZDeque<ObjectType, kIfUnique>::PushBack(ArgsType&&... args) noexcept {
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjectP(size_, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZDeque<ObjectType, kIfUnique>::PushBacks(const IndexType num, ArgsType&&... args) noexcept {
    DEBUG(num < 0, "Negative pushing num not valid!");
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjectsP(data_ptr_ + size_, num, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZDeque<ObjectType, kIfUnique>::Assign(const IndexType num, ArgsType&&... args) noexcept {
    IndexType new_size = num;
    DEBUG(num < 0, "Negative assign num not valid!");
    if (new_size > capacity_) {
        ExtendContainer(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    DestroyObjectsP(data_ptr_, size_);
    CreateObjectsP(data_ptr_, num, std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::Clear() noexcept {
    DestroyObjectsP(data_ptr_, data_ptr_ + size_);
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::Destroy() noexcept {
    DestroyContainer();
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::CreateContainer(const IndexType capacity) noexcept {
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    MemoryType need_memory_size = ((capacity * sizeof(ObjectType)) >> 1) + sizeof(DataNode);
    MemoryType apply_mrmory_size;
    //Applys 2 nodes instead.
    front_node_ptr_ = reinterpret_cast<DataNode*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));
    back_node_ptr_ = reinterpret_cast<DataNode*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));

    front_node_ptr_->next_node_ptr = back_node_ptr_;
    front_node_ptr_->previous_node_ptr = nullptr;
    front_node_ptr_->capacity = (apply_mrmory_size - sizeof(DataNode)) / sizeof(ObjectType);

    back_node_ptr_->next_node_ptr = nullptr;
    back_node_ptr_->previous_node_ptr = front_node_ptr_;
    back_node_ptr_->capacity = (apply_mrmory_size - sizeof(DataNode)) / sizeof(ObjectType);

    empty_node_ptr_ = nullptr;
    capacity_ = front_node_ptr_->capacity + back_node_ptr_->capacity;
}

template<typename ObjectType, Bool kIfUnique>
NODISCARD ZDeque<ObjectType, kIfUnique>::DataNode* const ZDeque<ObjectType, kIfUnique>::ExtendContainer(
        const IndexType capacity) noexcept {
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    MemoryType need_memory_size = capacity * sizeof(ObjectType) + sizeof(DataNode);
    MemoryType apply_mrmory_size;
    //Initialize the node.
    DataNode* temp_node_ptr =
        reinterpret_cast<DataNode*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));
    temp_node_ptr->capacity = (apply_mrmory_size - sizeof(DataNode)) / sizeof(ObjectType);
    //Update the container.
    capacity_ += temp_node_ptr->capacity;
    return temp_node_ptr;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::ShrinkContainer() noexcept {
    DataNode* temp_node_ptr;
    while (empty_node_ptr_ != nullptr) {
        temp_node_ptr = empty_node_ptr_;
        empty_node_ptr_ = empty_node_ptr_->next_node_ptr;
        capacity_ -= temp_node_ptr->capacity;
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(temp_node_ptr));
    }
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::DestroyContainer() noexcept {
    DataNode* temp_node_ptr;
    while (empty_node_ptr_ != nullptr) {
        temp_node_ptr = empty_node_ptr_;
        empty_node_ptr_ = empty_node_ptr_->next_node_ptr;
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(temp_node_ptr));
    }
    while (front_node_ptr_ != back_node_ptr_) {
        DestroyObjectsP(&back_node_ptr_[0], back_node_ptr_->size);
        temp_node_ptr = back_node_ptr_;
        back_node_ptr_ = back_node_ptr_->previous_node_ptr;
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(temp_node_ptr));
    }
    DestroyObjectsP(&front_node_ptr_[front_index_], front_node_ptr_->size);
    memory_pool::ReleaseMemory(reinterpret_cast<Void*>(data_ptr_));
    front_node_ptr_ = nullptr;
    back_node_ptr_ = nullptr;
    front_index_ = 0;
    back_index_ = 0;
    capacity_ = 0;
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::AddFrontNode() noexcept {
    DataNode* temp_node_ptr;
    if (empty_node_ptr_ == nullptr) {
        temp_node_ptr = ExtendContainer(capacity_ * kAutoExtendMulFactor);
    }
    else {
        temp_node_ptr = empty_node_ptr_;
        empty_node_ptr_ = empty_node_ptr_->next_node_ptr;
    }
    temp_node_ptr->next_node_ptr = front_node_ptr_;
    temp_node_ptr->previous_node_ptr = nullptr;
    if (front_node_ptr_ != nullptr) {
        front_node_ptr_->previous_node_ptr = temp_node_ptr;
        front_node_ptr_ = temp_node_ptr;
    }
    else {
        front_node_ptr_ = back_node_ptr_ = temp_node_ptr;
    }
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::RemoveFrontNode() noexcept {
    DataNode* temp_node_ptr;
    temp_node_ptr = front_node_ptr_;
    if (front_node_ptr_ == back_node_ptr_) {
        front_node_ptr_ = back_node_ptr_ = nullptr;
    }
    else {
        front_node_ptr_ = front_node_ptr_->next_node_ptr;
    }
    temp_node_ptr->next_node_ptr = empty_node_ptr_;
    empty_node_ptr_ = temp_node_ptr;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::AddBackNode() noexcept {
    DataNode* temp_node_ptr;
    if (empty_node_ptr_ == nullptr) {
        temp_node_ptr = ExtendContainer(capacity_ * kAutoExtendMulFactor);
    }
    else {
        temp_node_ptr = empty_node_ptr_;
        empty_node_ptr_ = empty_node_ptr_->next_node_ptr;
    }
    temp_node_ptr->next_node_ptr = nullptr;
    temp_node_ptr->previous_node_ptr = back_node_ptr_;
    if (back_node_ptr_ != nullptr) {
        back_node_ptr_->next_node_ptr = temp_node_ptr;
        back_node_ptr_ = temp_node_ptr;
    }
    else {
        front_node_ptr_ = back_node_ptr_ = temp_node_ptr;
    }
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::RemoveBackNode() noexcept {
    DataNode* temp_node_ptr;
    temp_node_ptr = back_node_ptr_;
    if (front_node_ptr_ == back_node_ptr_) {
        front_node_ptr_ = back_node_ptr_ = nullptr;
    }
    else {
        back_node_ptr_ = back_node_ptr_->previous_node_ptr;
    }
    temp_node_ptr->next_node_ptr = empty_node_ptr_;
    empty_node_ptr_ = temp_node_ptr;
}


template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::MoveP(ZDeque&& vector) {
    front_node_ptr_ = vector.front_node_ptr_;
    back_node_ptr_ = vector.back_node_ptr_;
    empty_node_ptr_ = vector.empty_node_ptr_;
    front_index_ = vector.front_index_;
    back_index_ = vector.back_index_;
    capacity_ = vector.capacity_;
    size_ = vector.size_;
    front_node_ptr_ = nullptr;
    back_node_ptr_ = nullptr;
    empty_node_ptr_ = nullptr;
    front_index_ = 0;
    back_index_ = 0;
    capacity_ = 0;
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CreateObjectP(ObjectType* const object_ptr, ArgsType&&... args) {
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
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::DestroyObjectP(ObjectType* const object_ptr) {
    if constexpr (kIfUnique) {
        (*object_ptr).~ObjectType();
    }
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
inline Void ZDeque<ObjectType, kIfUnique>::CreateObjectsP(ObjectType* begin_ptr, ObjectType* const end_ptr,
                                                         ArgsType&&... args) noexcept {
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
        if constexpr (sizeof...(args) != 0) {
            while (begin_ptr < end_ptr) {
                new(reinterpret_cast<Void*>(begin_ptr++)) ObjectType(std::forward<ArgsType>(args)...);
            }
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
inline Void ZDeque<ObjectType, kIfUnique>::CreateAndCopyObjectsP(ObjectType* dst_ptr, const ObjectType* src_begin_ptr, 
                                                                const ObjectType* const src_end_ptr) noexcept {
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
Void ZDeque<ObjectType, kIfUnique>::CreateAndCopyObjectsReverseP(ObjectType* dst_ptr, const ObjectType* src_begin_ptr, 
                                                                const ObjectType* const src_end_ptr) noexcept {
    while (src_begin_ptr > src_end_ptr) {
        new(reinterpret_cast<Void*>(dst_ptr)) ObjectType(*src_begin_ptr);
        ++dst_ptr;
        --src_begin_ptr;
    }
}



template<typename ObjectType, Bool kIfUnique>
inline Void ZDeque<ObjectType, kIfUnique>::DestroyObjectsP(ObjectType* begin_ptr, ObjectType* const end_ptr) noexcept {
    if constexpr (kIfUnique) {
        while (begin_ptr < end_ptr) {
            begin_ptr->~ObjectType();
            ++begin_ptr;
        }
    }
}

}//zengine


#endif // !Z_CORE_Z_DEQUE_H_