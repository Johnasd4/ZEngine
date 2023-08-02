/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#ifndef Z_CORE_Z_DEQUE_H_
#define Z_CORE_Z_DEQUE_H_

#include "internal/drive.h"

#include "m_error_message.h"
#include "z_object.h"

namespace zengine {

namespace internal {

template<typename ObjectType>
struct ZDequeDataNode {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) {
        return front_ptr[index];
    }
    NODISCARD FORCEINLINE const ObjectType& operator[](IndexType index) const {
        return front_ptr[index];
    }

    NODISCARD FORCEINLINE ObjectType& At(IndexType index) {
        return front_ptr[index];
    }
    NODISCARD FORCEINLINE const ObjectType& At(IndexType index) const {
        return front_ptr[index];
    } 

    NODISCARD FORCEINLINE ObjectType* AtPtr(IndexType index) {
        return front_ptr + index;
    }
    NODISCARD FORCEINLINE const ObjectType* AtPtr(IndexType index) const {
        return front_ptr + index;
    }

    ZDequeDataNode* next_node_ptr;
    ZDequeDataNode* previous_node_ptr;
    ObjectType* front_ptr;
    ObjectType* back_ptr;
    IndexType capacity;
};

template<typename ObjectType>
class ZDequeIteratorBase {
protected:
    using DataNode = ZDequeDataNode<ObjectType>;

public:
    FORCEINLINE ZDequeIteratorBase(ObjectType* object_ptr, DataNode* node_ptr) 
        : object_ptr_(object_ptr), node_ptr_(node_ptr) {}
    FORCEINLINE ZDequeIteratorBase(const ZDequeIteratorBase& iterator) { CopyP(iterator); }
    FORCEINLINE ZDequeIteratorBase(ZDequeIteratorBase&& iterator) { 
        MoveP(std::forward<ZDequeIteratorBase>(iterator)); 
    }

    FORCEINLINE ZDequeIteratorBase& operator=(const ZDequeIteratorBase& iterator) {
        CopyP(iterator);
        return *this;
    }
    FORCEINLINE ZDequeIteratorBase& operator=(ZDequeIteratorBase&& iterator) {
        MoveP(std::forward<ZDequeIteratorBase>(iterator));
        return *this;
    }

    NODISCARD FORCEINLINE Bool operator==(const ZDequeIteratorBase& iterator) const {
        return object_ptr_ == iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const ZDequeIteratorBase& iterator) const {
        return object_ptr_ != iterator.object_ptr_;
    }

    NODISCARD FORCEINLINE ObjectType& operator*() const { return *object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* operator->() const { return object_ptr_; }

    FORCEINLINE ~ZDequeIteratorBase() {}

    NODISCARD FORCEINLINE ObjectType& object() const { return *object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* object_ptr() const { return object_ptr_; }

    NODISCARD FORCEINLINE DataNode& data_node() const { return *node_ptr_; }
    NODISCARD FORCEINLINE DataNode* data_node_ptr() const { return node_ptr_; }

protected:
    ObjectType* FindObject(IndexType index) noexcept {
        DataNode* temp_node_ptr = node_ptr_;
        index = static_cast<IndexType>(object_ptr_ - temp_node_ptr->front_ptr) + index;
        while (index >= temp_node_ptr->capacity) {
            index -= temp_node_ptr->capacity;
            temp_node_ptr = temp_node_ptr->next_node_ptr;
        }
        while (index < 0) {
            temp_node_ptr = temp_node_ptr->previous_node_ptr;
            index += temp_node_ptr->capacity;
        }
        return temp_node_ptr->AtPtr(index);
    }

    Void MoveIterator(IndexType offset) noexcept {
        DataNode* temp_node_ptr = node_ptr_;
        IndexType index = static_cast<IndexType>(object_ptr_ - temp_node_ptr->front_ptr) + offset;
        while (index >= temp_node_ptr->capacity) {
            index -= temp_node_ptr->capacity;
            temp_node_ptr = temp_node_ptr->next_node_ptr;
        }
        while (index < 0) {
            temp_node_ptr = temp_node_ptr->previous_node_ptr;
            index += temp_node_ptr->capacity;
        }
        object_ptr_ = temp_node_ptr->AtPtr(index);
        node_ptr_ = temp_node_ptr;
    }

    ObjectType* object_ptr_;
    DataNode* node_ptr_;

private:
    FORCEINLINE Void CopyP(const ZDequeIteratorBase& iterator) {
        object_ptr_ = iterator.object_ptr_;
        node_ptr_ = iterator.node_ptr_;
    }

    FORCEINLINE Void MoveP(ZDequeIteratorBase&& iterator) {
        object_ptr_ = iterator.object_ptr_;
        node_ptr_ = iterator.node_ptr_;
        iterator.object_ptr_ = nullptr;
        iterator.node_ptr_ = nullptr;
    }
};

template<typename ObjectType>
class ZDequeIterator : public ZDequeIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) const {
        return *SuperType::FindObject(index);
    }

    FORCEINLINE ZDequeIterator& operator+=(IndexType num) {
        SuperType::MoveIterator(num);
        return *this;
    }
    FORCEINLINE ZDequeIterator& operator-=(IndexType num) {
        SuperType::MoveIterator(-num);
        return *this;
    }

    FORCEINLINE ZDequeIterator& operator++() {
        SuperType::MoveIterator(1);
        return *this;
    }
    FORCEINLINE ZDequeIterator& operator++(IndexType) {
        return operator++();
    }
    FORCEINLINE ZDequeIterator& operator--() {
        --SuperType::object_ptr_;
        if (SuperType::object_ptr_ < SuperType::node_ptr_->front_ptr) {
            SuperType::node_ptr_ = SuperType::node_ptr_->previous_node_ptr;
            SuperType::object_ptr_ = SuperType::node_ptr_->back_ptr - 1;
        }
        return *this;
    }
    FORCEINLINE ZDequeIterator& operator--(IndexType) {
        return operator--();
    }

    NODISCARD FORCEINLINE ZDequeIterator operator+(IndexType num) const {
        ZDequeIterator temp_iterator(*this);
        temp_iterator.SuperType::MoveIterator(num);
        return temp_iterator;
    }
    NODISCARD FORCEINLINE ZDequeIterator operator-(IndexType num) const {
        ZDequeIterator temp_iterator(*this);
        temp_iterator.SuperType::MoveIterator(-num);
        return temp_iterator;
    }

protected:
    using SuperType = ZDequeIteratorBase<ObjectType>;
};

template<typename ObjectType>
class ZDequeReverseIterator : public ZDequeIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) const {
        return *SuperType::FindObject(-index);
    }

    FORCEINLINE ZDequeReverseIterator& operator+=(IndexType num) {
        SuperType::MoveIterator(-num);
        return *this;
    }
    FORCEINLINE ZDequeReverseIterator& operator-=(IndexType num) {
        SuperType::MoveIterator(num);
        return *this;
    }

    FORCEINLINE ZDequeReverseIterator& operator++() {
        --SuperType::object_ptr_;
        if (SuperType::object_ptr_ < SuperType::node_ptr_->front_ptr) {
            SuperType::node_ptr_ = SuperType::node_ptr_->previous_node_ptr;
            SuperType::object_ptr_ = SuperType::node_ptr_->back_ptr - 1;
        }
        return *this;
    }
    FORCEINLINE ZDequeReverseIterator& operator++(IndexType) {
        return operator++();
    }
    FORCEINLINE ZDequeReverseIterator& operator--() {
        ++SuperType::object_ptr_;
        if (SuperType::object_ptr_ >= SuperType::node_ptr_->back_ptr) {
            SuperType::node_ptr_ = SuperType::node_ptr_->next_node_ptr;
            SuperType::object_ptr_ = SuperType::node_ptr_->front_ptr;
        }
        return *this;
    }
    FORCEINLINE ZDequeReverseIterator& operator--(IndexType) {
        return operator--();
    }

    NODISCARD FORCEINLINE ZDequeReverseIterator operator+(IndexType num) const {
        ZDequeIterator temp_iterator(*this);
        temp_iterator.SuperType::MoveIterator(-num);
        return temp_iterator;
    }
    NODISCARD FORCEINLINE ZDequeReverseIterator operator-(IndexType num) const {
        ZDequeIterator temp_iterator(*this);
        temp_iterator.SuperType::MoveIterator(num);
        return temp_iterator;
    }

protected:
    using SuperType = ZDequeIteratorBase<ObjectType>;
};

}//internal

/*
    Double end queue caintainer.
    If kIfUnique is true, will call the constuctor when the object is created.

    TODO(Johnasd4):Theory
*/
template<typename ObjectType, Bool kIfUnique = kIsClass<ObjectType>>
class ZDeque : public ZObject {
private:
    static constexpr Float32 kAutoExtendMulFactor = 0.5F;

public:
    using Iterator = internal::ZDequeIterator<ObjectType>;
    using ConstIterator = internal::ZDequeIterator<const ObjectType>;
    using ReverseIterator = internal::ZDequeReverseIterator<ObjectType>;
    using ConstReverseIterator = internal::ZDequeReverseIterator<const ObjectType>;

    ZDeque() noexcept;
    ZDeque(const ZDeque& deque) noexcept;
    ZDeque(ZDeque&& deque) noexcept;
     
    ZDeque(IndexType capacity) noexcept;
    /*
        Fills the container by the object constructed by the arguements.
    */
    template<typename... ArgsType>
    ZDeque(IndexType capacity, ArgsType&&... args) noexcept;    

    ZDeque(const Iterator& begin, const Iterator& end) noexcept : SuperType() {
        ZDequeOrderP(begin.object_ptr(), begin.data_node_ptr(), end.object_ptr(), end.data_node_ptr());
    }
    ZDeque(const ConstIterator& begin, const ConstIterator& end) noexcept : SuperType() {
        ZDequeOrderP(begin.object_ptr(), begin.data_node_ptr(), end.object_ptr(), end.data_node_ptr());
    }
    ZDeque(const ReverseIterator& begin, const ReverseIterator& end) noexcept : SuperType() {
        ZDequeReverseP(begin.object_ptr(), begin.data_node_ptr(), end.object_ptr(), end.data_node_ptr());
    }
    ZDeque(const ConstReverseIterator& begin, const ConstReverseIterator& end) noexcept : SuperType() {
        ZDequeReverseP(begin.object_ptr(), begin.data_node_ptr(), end.object_ptr(), end.data_node_ptr());
    }

    ZDeque& operator=(const ZDeque& deque) noexcept;
    ZDeque& operator=(ZDeque&& deque) noexcept;

    NODISCARD FORCEINLINE Bool operator==(const ZDeque& deque) noexcept {
        return this == &deque;
    }
    NODISCARD FORCEINLINE Bool operator!=(const ZDeque& deque) noexcept {
        return this != &deque;
    }

    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) { return AtP(index); }
    NODISCARD FORCEINLINE const ObjectType& operator[](IndexType index) const { return AtP(index); }

    ~ZDeque() noexcept;

    NODISCARD FORCEINLINE ObjectType& At(IndexType index);
    NODISCARD FORCEINLINE const ObjectType& At(IndexType index) const;
    NODISCARD FORCEINLINE ObjectType* AtPtr(IndexType index);
    NODISCARD FORCEINLINE const ObjectType* AtPtr(IndexType index) const;
    NODISCARD FORCEINLINE ObjectType& Front();
    NODISCARD FORCEINLINE const ObjectType& Front() const;
    NODISCARD FORCEINLINE ObjectType* FrontPtr();
    NODISCARD FORCEINLINE const ObjectType* FrontPtr() const;
    NODISCARD FORCEINLINE ObjectType& Back();
    NODISCARD FORCEINLINE const ObjectType& Back() const;
    NODISCARD FORCEINLINE ObjectType* BackPtr();
    NODISCARD FORCEINLINE const ObjectType* BackPtr() const;

    NODISCARD FORCEINLINE IndexType size() const { return size_; }
    NODISCARD FORCEINLINE IndexType capacity() const { return capacity_; }

    NODISCARD FORCEINLINE Bool Empty();
    NODISCARD FORCEINLINE Bool Exist();

    /*
        Resize the deque, If the given size is smaller then the current size,
        the function will cut remove the extra objects. If the given size is
        bigger then the current size, the function will fill the extra size with
        the default value of ObjectType.
    */
    Void Resize(IndexType size) noexcept;
    /*
        Resize the deque, If the given size is smaller then the current size,
        the function will cut remove the extra objects. If the given size is
        bigger then the current size, the function will fill the extra size with
        the object constructed by the arguements.
    */
    template<typename... ArgsType>
    Void Resize(IndexType size, ArgsType&&... args) noexcept;

    /*
        Will extend the capacity by the given capacity, if the smaller then the
        given capacity, this function will do nothing, otherwise the capacity will
        entend to a size that equals or bigger then the given capacity.
    */
    Void Reserve(IndexType capacity) noexcept;

    /*
        Shrinks the deque to the minimum capacity that can fit the current size.
    */
    Void ShrinkToFit() noexcept;

    /*
        Remove the object at the front of the deque.
    */
    Void PopFront() noexcept;
    /*
        Remove the object at the front of the deque.. Give the authority to the given address.
    */
    Void PopFront(ObjectType* object_ptr) noexcept;

    /*
        Create an object at the front of the deque by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the deque.
    */
    template<typename... ArgsType>
    Void PushFront(ArgsType&&... args) noexcept;
    /*
        Create objects at the front of the deque by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the deque.
    */
    template<typename... ArgsType>
    Void PushFronts(IndexType num, ArgsType&&... args) noexcept;

    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the deque.
    */
    Void PushFronts(const Iterator& begin, const Iterator& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the deque.
    */
    Void PushFronts(const ConstIterator& begin, const ConstIterator& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the deque.
    */
    Void PushFronts(const ReverseIterator& begin, const ReverseIterator& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        front of the deque.
    */
    Void PushFronts(const ConstReverseIterator& begin, const ConstReverseIterator& end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void EmplaceFront(ArgsType&&... args) noexcept;

    /*
        Remove the object at the back of the deque.
    */
    Void PopBack() noexcept;
    /*
        Remove the object at the back of the deque.. Give the authority to the given address.
    */
    Void PopBack(ObjectType* object_ptr) noexcept;

    /*
        Create an object at the back of the deque by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the deque.
    */
    template<typename... ArgsType>
    Void PushBack(ArgsType&&... args) noexcept;
    /*
        Create objects at the back of the deque by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the deque.
    */
    template<typename... ArgsType>
    Void PushBacks(IndexType num, ArgsType&&... args) noexcept;

    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the deque.
    */
    Void PushBacks(const Iterator& begin, const Iterator& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the deque.
    */
    Void PushBacks(const ConstIterator& begin, const ConstIterator& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the deque.
    */
    Void PushBacks(const ReverseIterator& begin, const ReverseIterator& end) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the deque.
    */
    Void PushBacks(const ConstReverseIterator& begin, const ConstReverseIterator& end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void EmplaceBack(ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    Iterator Insert(IndexType index, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    Iterator Insert(const Iterator& iterator, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    ReverseIterator Insert(const ReverseIterator& iterator, ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    Iterator Inserts(IndexType index, IndexType num, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    Iterator Inserts(const Iterator& iterator, IndexType num, ArgsType&&... args) noexcept;
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    ReverseIterator Inserts(const ReverseIterator& iterator, IndexType num, ArgsType&&... args) noexcept;

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    Iterator Inserts(IndexType index,
        const Iterator& src_begin, const Iterator& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    Iterator Inserts(IndexType index,
        const ConstIterator& src_begin, const ConstIterator& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    Iterator Inserts(IndexType index,
        const ReverseIterator& src_begin, const ReverseIterator& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    Iterator Inserts(IndexType index,
        const ConstReverseIterator& src_begin, const ConstReverseIterator& src_end) noexcept;

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    Iterator Inserts(const Iterator& iterator,
        const Iterator& src_begin, const Iterator& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    Iterator Inserts(const Iterator& iterator,
        const ConstIterator& src_begin, const ConstIterator& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    Iterator Inserts(const Iterator& iterator,
        const ReverseIterator& src_begin, const ReverseIterator& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    Iterator Inserts(const Iterator& iterator,
        const ConstReverseIterator& src_begin, const ConstReverseIterator& src_end) noexcept;

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    ReverseIterator Inserts(const ReverseIterator& iterator,
        const Iterator& src_begin, const Iterator& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    ReverseIterator Inserts(const ReverseIterator& iterator,
        const ConstIterator& src_begin, const ConstIterator& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    ReverseIterator Inserts(const ReverseIterator& iterator,
        const ReverseIterator& src_begin, const ReverseIterator& src_end) noexcept;
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    ReverseIterator Inserts(const ReverseIterator& iterator,
        const ConstReverseIterator& src_begin,
        const ConstReverseIterator& src_end) noexcept;

    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    Iterator Erase(IndexType index) noexcept;
    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    Iterator Erase(const Iterator& iterator) noexcept;
    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    ReverseIterator Erase(const ReverseIterator& iterator) noexcept;

    /*
        Erases the num of objects that starts at the given index.
        Returns the iterator that points at the next object.
    */
    Iterator Erases(IndexType index, IndexType num) noexcept;
    /*
        Erases the num of objects that starts at the given iterator.
        Returns the iterator that points at the next object.
    */
    Iterator Erases(const Iterator& iterator, IndexType num) noexcept;
    /*
        Erases the num of objects that starts at the given iterator.
        Returns the iterator that points at the next object.
    */
    ReverseIterator Erases(const ReverseIterator& iterator, IndexType num) noexcept;
    /*
        Erases the object between begin and end, involves begin, but don't involves end.
        Returns the iterator that points at the next object.
    */
    Iterator Erases(const Iterator& begin, const Iterator& end) noexcept;
    /*
        Erases the object between begin and end, involves begin, but don't involves end.
        Returns the iterator that points at the next object.
    */
    ReverseIterator Erases(const ReverseIterator& begin, const ReverseIterator& end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(IndexType index, ArgsType&&... args) noexcept;
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const Iterator& iterator, ArgsType&&... args) noexcept;
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    Void Emplace(const ReverseIterator& iterator, ArgsType&&... args) noexcept;

    /*
        Construct the deque by filling it with the given amount of objects.
        The object is constructed by the arguements.
        If kIfUnique is false and no arguements, will only add the 
        size of the deque.
    */
    template<typename... ArgsType>
    Void Assign(IndexType num, ArgsType&&... args) noexcept;
    /*
        Construct the deque by filling it objects between the iterators.
    */
    Void Assign(const Iterator& begin, const Iterator& end) noexcept;
    /*
        Construct the deque by filling it objects between the iterators.
    */
    Void Assign(const ConstIterator& begin, const ConstIterator& end) noexcept;
    /*
        Construct the deque by filling it objects between the iterators.
    */
    Void Assign(const ReverseIterator& begin, const ReverseIterator& end) noexcept;
    /*
        Construct the deque by filling it objects between the iterators.
    */
    Void Assign(const ConstReverseIterator& begin, const ConstReverseIterator& end) noexcept;
    
    /*
        Destroys all the objects in the deque, does not release the memory.
    */
    Void Clear() noexcept;
    /*
        Destroys all the objects in the deque, release the memory.
    */
    Void Destroy() noexcept;

protected:
    using SuperType = ZObject;
    using DataNode = internal::ZDequeDataNode<ObjectType>;

private:
    static constexpr Bool kCreateObjects = true;
    static constexpr Bool kDestroyObjects = false;
    static constexpr Bool kCreateAndCopyObjects = true;
    static constexpr Bool kCopyObjects = false;
    static constexpr Bool kCreateAndCopyObjectsReverse = true;
    static constexpr Bool kCopyObjectsReverse = false;

    /*
        Creates an object at the certain place. Will call the Constrctor if needed.
    */
    template<typename... ArgsType>
    FORCEINLINE static Void CreateObjectP(ObjectType* object_ptr, ArgsType&&... args);

    /*
        Destroys an object at the certain place. Will Call Destrctor if needed.
    */
    FORCEINLINE static Void DestroyObjectP(ObjectType* object_ptr);

    /*
        The base function of create and destroy objests.
    */
    template<Bool kIfCreate, typename... ArgsType>
    FORCEINLINE static Void CreateDestroyObjectsBaseP(ObjectType* front_ptr, DataNode* begin_node_ptr,
                                                      ObjectType* back_ptr, DataNode* end_node_ptr,
                                                      ArgsType&&... args) noexcept;

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the Constrctor if needed.
    */
    template<typename... ArgsType>
    inline static Void CreateObjectsP(ObjectType* front_ptr, DataNode* begin_node_ptr,
                                      ObjectType* back_ptr, DataNode* end_node_ptr,
                                      ArgsType&&... args) noexcept;

    /*
        Destroy the objects by the given arguements([begin, end)). 
        Will call the destrctor if this object class's member kIfUnique is true.
    */
    inline static Void DestroyObjectsP(ObjectType* front_ptr, DataNode* begin_node_ptr,
                                       ObjectType* back_ptr, DataNode* end_node_ptr) noexcept;

    /*
        The base function of copy objests.
    */
    template<Bool kIfCreate>
    FORCEINLINE static Void CopyObjectsBaseP(ObjectType* dst_ptr, DataNode* dst_node_ptr,
                                             const ObjectType* src_begin_ptr, const DataNode* src_begin_node_ptr,
                                             const ObjectType* src_end_ptr, const DataNode* src_end_node_ptr,
                                             ObjectType** dst_end_ptr_ptr,DataNode** dst_end_node_ptr_ptr) noexcept;

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the copy constructor.
    */
    static Void CreateAndCopyObjectsP(ObjectType* dst_ptr, DataNode* dst_node_ptr,
                                      const ObjectType* src_begin_ptr, const DataNode* src_begin_node_ptr,
                                      const ObjectType* src_end_ptr, const DataNode* src_end_node_ptr,
                                      ObjectType** dst_end_ptr_ptr,DataNode** dst_end_node_ptr_ptr) noexcept;

    /*
        Copy objects by the given pointer. Will call the copy assignment operator
        if this object class's member kIfUnique is true.
    */
    static Void CopyObjectsP(ObjectType* dst_ptr, DataNode* dst_node_ptr,
                             const ObjectType* src_begin_ptr, const DataNode* src_begin_node_ptr,
                             const ObjectType* src_end_ptr, const DataNode* src_end_node_ptr,
                             ObjectType** dst_end_ptr_ptr, DataNode** dst_end_node_ptr_ptr) noexcept;

    /*
        The base function of reverse copy objests.
    */
    template<Bool kIfCreate>
    FORCEINLINE static Void CopyObjectsReverseBaseP(ObjectType* dst_ptr, 
                                                    DataNode* dst_node_ptr,
                                                    const ObjectType* src_begin_ptr, 
                                                    const DataNode* src_begin_node_ptr,
                                                    const ObjectType* src_end_ptr, 
                                                    const DataNode* src_end_node_ptr,
                                                    ObjectType** dst_end_ptr_ptr,
                                                    DataNode** dst_end_node_ptr_ptr) noexcept;

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the copy constructor.
    */
    static Void CreateAndCopyObjectsReverseP(ObjectType* dst_ptr, DataNode* dst_node_ptr,
                                             const ObjectType* src_begin_ptr, const DataNode* src_begin_node_ptr,
                                             const ObjectType* src_end_ptr, const DataNode* src_end_node_ptr,
                                             ObjectType** dst_end_ptr_ptr, DataNode** dst_end_node_ptr_ptr) noexcept;


    /*
        Copy objects by the given pointer. Will call the copy assignment operator
        if this object class's member kIfUnique is true.
    */
    static Void CopyObjectsReverseP(ObjectType* dst_ptr, DataNode* dst_node_ptr,
                                    const ObjectType* src_begin_ptr, const DataNode* src_begin_node_ptr,
                                    const ObjectType* src_end_ptr, const DataNode* src_end_node_ptr,
                                    ObjectType** dst_end_ptr_ptr, DataNode** dst_end_node_ptr_ptr) noexcept;

    /*
        Starts at the given place and find the place that is num objects away.
    */
    static Void FindPlaceP(const ObjectType* front_ptr, const DataNode* begin_node_ptr, IndexType num,
                           ObjectType** find_ptr_ptr, DataNode** find_node_ptr_ptr) noexcept;

    /*
        Calculate the length between the two places.
    */
    static IndexType CalculateLengthP(const ObjectType* front_ptr, const DataNode* begin_node_ptr,
                                      const ObjectType* back_ptr, const DataNode* end_node_ptr) noexcept;



    /*
        Creates the capacity by the given capacity, the final capacity might
        not equal the given capacity.
    */
    Void CreateContainerP(IndexType capacity) noexcept;
    /*
        Extends the capacity by the given capacity, the final capacity might
        not equal the given capacity. Returns the data node pointer.
    */
    Void ExtendContainerP(IndexType capacity) noexcept;
    /*
        Shrinks the container to the minimum capacity that can fit the current size.
    */
    Void ShrinkContainerP() noexcept;
    /*
        Destroys the container.
    */
    Void DestroyContainerP() noexcept;

    /*
        Constructor with two order iterators.
    */
    FORCEINLINE Void ZDequeOrderP(const ObjectType* front_ptr, const DataNode* begin_node_ptr,
                                  const ObjectType* back_ptr, const DataNode* end_node_ptr) noexcept;

    /*
        Constructor with two reverse iterators.
    */
    FORCEINLINE Void ZDequeReverseP(const ObjectType* front_ptr, const DataNode* begin_node_ptr,
                                    const ObjectType* back_ptr, const DataNode* end_node_ptr) noexcept;

    /*
        Container copy function.
    */
    Void CopyP(const ZDeque& deque) noexcept;

    /*
        Container move function.
    */
    FORCEINLINE Void MoveP(ZDeque&& deque);

    /*
        Finds the object by the given index.
    */
    ObjectType* AtP(const IndexType index) noexcept;

    DataNode* front_node_ptr_;
    DataNode* back_node_ptr_;
    DataNode* front_ptr_;
    DataNode* back_ptr_;
    IndexType capacity_;
    IndexType size_;
};

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::ZDeque() noexcept 
    : SuperType()
    , front_node_ptr_(nullptr)
    , back_node_ptr_(nullptr)
    , front_ptr_(nullptr)
    , back_ptr_(nullptr)
    , capacity_(0)
    , size_(0)
{}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::ZDeque(const ZDeque& deque) noexcept 
    : SuperType(deque)
{
    DEBUG(&deque == this, "The source and the target of the copy is the same!");
    CreateContainerP(deque.size_);
    ObjectType* temp_back_ptr = deque.back_ptr_;
    if (temp_back_ptr == deque.back_node_ptr_->back_ptr) {
        temp_back_ptr = deque.back_node_ptr_->next_node_ptr->
    }
    front_ptr_ = front_node_ptr_->front_ptr;
    CreateAndCopyObjectsP(front_ptr_, front_node_ptr_, 
                          deque.front_ptr_, deque.front_node_ptr_,
                          deque.back_ptr_, deque.back_node_ptr_,
                          &back_ptr_, &back_node_ptr_);
    back_ptr_ -= 1;
    size_ = deque.size_;
}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::ZDeque(ZDeque&& deque) noexcept
    : SuperType(std::forward<ZDeque>(deque))
{
    DEBUG(&deque == this, "The source and the target of the copy is the same!");
    MoveP(std::forward<ZDeque>(deque));
}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::ZDeque(IndexType capacity) noexcept
    : SuperType()
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainerP(capacity);
    back_node_ptr_ = front_node_ptr_->previous_node_ptr;
    front_ptr_ = front_node_ptr_->front_ptr;
    back_ptr_ = back_node_ptr_->back_ptr - 1;
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
ZDeque<ObjectType, kIfUnique>::ZDeque(IndexType capacity, ArgsType&&... args) noexcept
    : SuperType()
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainerP(capacity);
    ObjectType* back_ptr;
    DataNode* end_node_ptr;
    front_ptr_ = front_node_ptr_->front_ptr;
    FindPlaceP(front_ptr_, front_node_ptr_, capacity, &back_ptr, &end_node_ptr);
    CreateObjectsP(front_ptr_, front_node_ptr_, back_ptr, end_node_ptr, std::forward<ArgsType>(args)...);
    back_ptr_ = back_ptr - 1;
    back_node_ptr_ = end_node_ptr;
    size_ = capacity;
}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>& ZDeque<ObjectType, kIfUnique>::operator=(const ZDeque& deque) noexcept {
    DEBUG(&deque == this, "The source and the target of the copy is the same!");
    SuperType::operator=(deque);
    CopyP(deque);
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>& ZDeque<ObjectType, kIfUnique>::operator=(ZDeque&& deque) noexcept {
    DEBUG(&deque == this, "The source and the target of the copy is the same!");
    SuperType::operator=(std::forward<ZDeque>(deque));
    DestroyObjectsP(front_ptr_, front_node_ptr_, back_ptr_ + 1, back_node_ptr_);
    MoveP(std::forward<ZDeque>(deque));
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZDeque<ObjectType, kIfUnique>::~ZDeque() noexcept {
    DestroyContainerP();
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::CreateObjectP(ObjectType* object_ptr, ArgsType&&... args) {
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
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::DestroyObjectP(ObjectType* object_ptr) {
    if constexpr (kIfUnique) {
        (*object_ptr).~ObjectType();
    }
}

template<typename ObjectType, Bool kIfUnique>
template<Bool kIfCreate, typename... ArgsType>
FORCEINLINE static Void ZDeque<ObjectType, kIfUnique>::CreateDestroyObjectsBaseP(ObjectType* front_ptr,
                                                                                 DataNode* begin_node_ptr,
                                                                                 ObjectType* back_ptr,
                                                                                 DataNode* end_node_ptr,
                                                                                 ArgsType&&... args) noexcept {
    if constexpr (sizeof...(args) != 0 || (!kIfCreate && kIfUnique)) {
        if (back_ptr == end_node_ptr->front_ptr) {
            end_node_ptr = end_node_ptr->previous_node_ptr;
            back_ptr = end_node_ptr->back_ptr;
        }
        if (front_ptr < begin_node_ptr->front_ptr) {
            begin_node_ptr = begin_node_ptr->previous_node_ptr;
            front_ptr = begin_node_ptr->back_ptr - 1;
        }
        else if (front_ptr >= begin_node_ptr->back_ptr) {
            begin_node_ptr = begin_node_ptr->next_node_ptr;
            front_ptr = begin_node_ptr->front_ptr;
        }
        while (begin_node_ptr != end_node_ptr) {
            ObjectType* temp_end_ptr = begin_node_ptr->back_ptr;
            while (front_ptr != temp_end_ptr) {
                if constexpr (kIfCreate) {
                    new(reinterpret_cast<Void*>(front_ptr)) ObjectType(std::forward<ArgsType>(args)...);
                }
                else {
                    front_ptr->~ObjectType();
                }
                ++front_ptr;
            }
            begin_node_ptr = begin_node_ptr->next_node_ptr;
            front_ptr = begin_node_ptr->front_ptr;
        }
        while (front_ptr != back_ptr) {
            if constexpr (kIfCreate) {
                new(reinterpret_cast<Void*>(front_ptr)) ObjectType(std::forward<ArgsType>(args)...);
            }
            else {
                front_ptr->~ObjectType();
            }
            ++front_ptr;
        }
    }
    else {
        if constexpr (kIfUnique) {
            if (back_ptr == end_node_ptr->front_ptr) {
                end_node_ptr = end_node_ptr->previous_node_ptr;
                back_ptr = end_node_ptr->back_ptr;
            }
            if (front_ptr < begin_node_ptr->front_ptr) {
                begin_node_ptr = begin_node_ptr->previous_node_ptr;
                front_ptr = begin_node_ptr->back_ptr - 1;
            }
            else if (front_ptr >= begin_node_ptr->back_ptr) {
                begin_node_ptr = begin_node_ptr->next_node_ptr;
                front_ptr = begin_node_ptr->front_ptr;
            }
            if (begin_node_ptr != end_node_ptr) {
                new(reinterpret_cast<Void*>(front_ptr)) ObjectType[begin_node_ptr->back_ptr - front_ptr];
                while (begin_node_ptr != end_node_ptr) {
                    new(reinterpret_cast<Void*>(begin_node_ptr->front_ptr)) ObjectType[begin_node_ptr->capacity];
                    begin_node_ptr = begin_node_ptr->next_node_ptr;
                }
                new(reinterpret_cast<Void*>(begin_node_ptr->front_ptr)) ObjectType[back_ptr - begin_node_ptr->front_ptr];
            }
            else {
                new(reinterpret_cast<Void*>(front_ptr)) ObjectType[back_ptr - front_ptr];
            }
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
inline Void ZDeque<ObjectType, kIfUnique>::CreateObjectsP(ObjectType* front_ptr, DataNode* begin_node_ptr,
                                                          ObjectType* back_ptr, DataNode* end_node_ptr,
                                                          ArgsType&&... args) noexcept {
    CreateDestroyObjectsBaseP<kCreateObjects>(front_ptr, begin_node_ptr, back_ptr, end_node_ptr, 
                                              std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfUnique>
inline Void ZDeque<ObjectType, kIfUnique>::DestroyObjectsP(ObjectType* front_ptr, DataNode* begin_node_ptr,
                                                           ObjectType* back_ptr, DataNode* end_node_ptr) noexcept {
    CreateDestroyObjectsBaseP<kDestroyObjects>(front_ptr, begin_node_ptr, back_ptr, end_node_ptr);
}

template<typename ObjectType, Bool kIfUnique>
template<Bool kIfCreate>
FORCEINLINE static Void ZDeque<ObjectType, kIfUnique>::CopyObjectsBaseP(ObjectType* dst_ptr, 
                                                                        DataNode* dst_node_ptr,
                                                                        const ObjectType* src_begin_ptr, 
                                                                        const DataNode* src_begin_node_ptr,
                                                                        const ObjectType* src_end_ptr, 
                                                                        const DataNode* src_end_node_ptr,
                                                                        ObjectType** dst_end_ptr_ptr, 
                                                                        DataNode** dst_end_node_ptr_ptr) noexcept {
    if constexpr (kIfUnique) {
        ObjectType* temp_dst_ptr;
        const ObjectType* temp_src_begin_ptr;
        const ObjectType* temp_src_end_ptr;
        IndexType dst_left_size = static_cast<IndexType>(dst_node_ptr->back_ptr - dst_ptr);
        IndexType src_left_size = static_cast<IndexType>(src_begin_node_ptr->back_ptr - src_begin_ptr);
        if (src_end_ptr == src_end_node_ptr->front_ptr) {
            src_end_node_ptr = src_end_node_ptr->previous_node_ptr;
            src_end_ptr = src_end_node_ptr->back_ptr;
        }
        if (dst_ptr < dst_node_ptr->front_ptr) {
            dst_node_ptr = dst_node_ptr->previous_node_ptr;
            dst_ptr = dst_node_ptr->back_ptr - 1;
        }
        else if (dst_ptr >= dst_node_ptr->back_ptr) {
            dst_node_ptr = dst_node_ptr->next_node_ptr;
            dst_ptr = dst_node_ptr->front_ptr;
        }
        if (src_begin_ptr < src_begin_node_ptr->front_ptr) {
            src_begin_node_ptr = src_begin_node_ptr->previous_node_ptr;
            src_begin_ptr = src_begin_node_ptr->back_ptr - 1;
        }
        else if (src_begin_ptr >= src_begin_node_ptr->back_ptr) {
            src_begin_node_ptr = src_begin_node_ptr->next_node_ptr;
            src_begin_ptr = src_begin_node_ptr->front_ptr;
        }
        while (src_begin_node_ptr != src_end_node_ptr) {
            temp_dst_ptr = dst_ptr;
            temp_src_begin_ptr = src_begin_ptr;
            if (dst_left_size < src_left_size) {
                temp_src_end_ptr = temp_src_begin_ptr + dst_left_size;
                dst_node_ptr = dst_node_ptr->next_node_ptr;
                dst_ptr = dst_node_ptr->front_ptr;
                src_begin_ptr = temp_src_end_ptr;
                src_left_size -= dst_left_size;
                dst_left_size = dst_node_ptr->capacity;
            }
            else {
                temp_src_end_ptr = temp_src_begin_ptr + src_left_size;
                src_begin_node_ptr = src_begin_node_ptr->next_node_ptr;
                src_begin_ptr = src_begin_node_ptr->front_ptr;
                dst_ptr += src_left_size;
                dst_left_size -= src_left_size;
                src_left_size = src_begin_node_ptr->capacity;
            }
            while (temp_src_begin_ptr < temp_src_end_ptr) {
                if constexpr (kIfCreate) {
                    new(reinterpret_cast<Void*>(temp_dst_ptr)) ObjectType(*temp_src_begin_ptr);
                }
                else {
                    *temp_dst_ptr = *temp_src_begin_ptr;
                }
                ++temp_dst_ptr;
                ++temp_src_begin_ptr;
            }
        }
        src_left_size = src_end_ptr - src_begin_ptr;
        while (dst_left_size < src_left_size) {
            temp_dst_ptr = dst_ptr;
            temp_src_end_ptr = src_begin_ptr + dst_left_size;
            dst_ptr = dst_node_ptr->front_ptr;
            src_left_size -= dst_left_size;
            dst_left_size = dst_node_ptr->capacity;
            while (src_begin_ptr < temp_src_end_ptr) {
                if constexpr (kIfCreate) {
                    new(reinterpret_cast<Void*>(temp_dst_ptr)) ObjectType(*src_begin_ptr);
                }
                else {
                    *temp_dst_ptr = *src_begin_ptr;
                }
                ++temp_dst_ptr;
                ++src_begin_ptr;
            }
        }
        while (src_begin_ptr < src_end_ptr) {
            if constexpr (kIfCreate) {
                new(reinterpret_cast<Void*>(dst_ptr)) ObjectType(*src_begin_ptr);
            }
            else {
                *dst_ptr = *src_begin_ptr;
            }
            ++dst_ptr;
            ++src_begin_ptr;
        }
        *dst_end_ptr_ptr = dst_ptr;
        *dst_end_node_ptr_ptr = dst_node_ptr;
    }
    else {
        ObjectType* temp_dst_ptr;
        const ObjectType* temp_src_begin_ptr;
        IndexType copy_num;
        IndexType dst_left_size = static_cast<IndexType>(dst_node_ptr->back_ptr - dst_ptr);
        IndexType src_left_size = static_cast<IndexType>(src_begin_node_ptr->back_ptr - src_begin_ptr);
        if (src_end_ptr == src_end_node_ptr->front_ptr) {
            src_end_node_ptr = src_end_node_ptr->previous_node_ptr;
            src_end_ptr = src_end_node_ptr->back_ptr;
        }
        if (dst_ptr < dst_node_ptr->front_ptr) {
            dst_node_ptr = dst_node_ptr->previous_node_ptr;
            dst_ptr = dst_node_ptr->back_ptr - 1;
        }
        else if (dst_ptr >= dst_node_ptr->back_ptr) {
            dst_node_ptr = dst_node_ptr->next_node_ptr;
            dst_ptr = dst_node_ptr->front_ptr;
        }
        if (src_begin_ptr < src_begin_node_ptr->front_ptr) {
            src_begin_node_ptr = src_begin_node_ptr->previous_node_ptr;
            src_begin_ptr = src_begin_node_ptr->back_ptr - 1;
        }
        else if (src_begin_ptr >= src_begin_node_ptr->back_ptr) {
            src_begin_node_ptr = src_begin_node_ptr->next_node_ptr;
            src_begin_ptr = src_begin_node_ptr->front_ptr;
        }
        while (src_begin_node_ptr != src_end_node_ptr) {
            temp_dst_ptr = dst_ptr;
            temp_src_begin_ptr = src_begin_ptr;
            if (dst_left_size < src_left_size) {
                copy_num = dst_left_size;
                dst_node_ptr = dst_node_ptr->next_node_ptr;
                dst_ptr = dst_node_ptr->front_ptr;
                src_begin_ptr += dst_left_size;
                src_left_size -= dst_left_size;
                dst_left_size = dst_node_ptr->capacity;
            }
            else {
                copy_num = src_left_size;
                src_begin_node_ptr = src_begin_node_ptr->next_node_ptr;
                src_begin_ptr = src_begin_node_ptr->front_ptr;
                dst_ptr += src_left_size;
                dst_left_size -= src_left_size;
                src_left_size = src_begin_node_ptr->capacity;
            }
            memcpy(reinterpret_cast<Void*>(temp_dst_ptr),
                   reinterpret_cast<Void*>(const_cast<ObjectType*>(temp_src_begin_ptr)),
                   copy_num * sizeof(ObjectType));
        }
        src_left_size = src_end_ptr - src_begin_ptr;
        while (dst_left_size < src_left_size) {
            copy_num = dst_left_size;
            temp_dst_ptr = dst_ptr;
            dst_node_ptr = dst_node_ptr->next_node_ptr;
            dst_ptr = dst_node_ptr->front_ptr;
            src_left_size -= dst_left_size;
            dst_left_size = dst_node_ptr->capacity;
            memcpy(reinterpret_cast<Void*>(temp_dst_ptr),
                   reinterpret_cast<Void*>(const_cast<ObjectType*>(src_begin_ptr)),
                   copy_num * sizeof(ObjectType));
        }
        copy_num = static_cast<IndexType>(src_end_ptr - src_begin_ptr);
        memcpy(reinterpret_cast<Void*>(dst_ptr),
               reinterpret_cast<Void*>(const_cast<ObjectType*>(src_begin_ptr)),
               copy_num * sizeof(ObjectType));
        *dst_end_ptr_ptr = dst_ptr + copy_num;
        *dst_end_node_ptr_ptr = dst_node_ptr;
    }
}


template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::CreateAndCopyObjectsP(ObjectType* dst_ptr, 
                                                          DataNode* dst_node_ptr,
                                                          const ObjectType* src_begin_ptr, 
                                                          const DataNode* src_begin_node_ptr,
                                                          const ObjectType* src_end_ptr, 
                                                          const DataNode* src_end_node_ptr,
                                                          ObjectType** dst_end_ptr_ptr,
                                                          DataNode** dst_end_node_ptr_ptr) noexcept {
    CopyObjectsBaseP<kCreateAndCopyObjects>(dst_ptr, dst_node_ptr,
                                            src_begin_ptr, src_begin_node_ptr, 
                                            src_end_ptr, src_end_node_ptr, 
                                            dst_end_ptr_ptr, dst_end_node_ptr_ptr);
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::CopyObjectsP(ObjectType* dst_ptr, 
                                                 DataNode* dst_node_ptr,
                                                 const ObjectType* src_begin_ptr, 
                                                 const DataNode* src_begin_node_ptr,
                                                 const ObjectType* src_end_ptr, 
                                                 const DataNode* src_end_node_ptr,
                                                 ObjectType** dst_end_ptr_ptr,
                                                 DataNode** dst_end_node_ptr_ptr) noexcept {
    CopyObjectsBaseP<kCopyObjects>(dst_ptr, dst_node_ptr,
                                   src_begin_ptr, src_begin_node_ptr,
                                   src_end_ptr, src_end_node_ptr,
                                   dst_end_ptr_ptr, dst_end_node_ptr_ptr);
}

template<typename ObjectType, Bool kIfUnique>
template<Bool kIfCreate>
FORCEINLINE static Void ZDeque<ObjectType, kIfUnique>::CopyObjectsReverseBaseP(
        ObjectType* dst_ptr, DataNode* dst_node_ptr,
        const ObjectType* src_begin_ptr, const DataNode* src_begin_node_ptr,
        const ObjectType* src_end_ptr, const DataNode* src_end_node_ptr,
        ObjectType** dst_end_ptr_ptr, DataNode** dst_end_node_ptr_ptr) noexcept {
    ObjectType* temp_dst_ptr;
    const ObjectType* temp_src_begin_ptr;
    const ObjectType* temp_src_end_ptr;
    IndexType dst_left_size = static_cast<IndexType>(dst_node_ptr->back_ptr - dst_ptr);
    IndexType src_left_size = static_cast<IndexType>(src_begin_node_ptr->back_ptr - src_begin_ptr);
    if (src_end_ptr == src_end_node_ptr->back_ptr - 1) {
        src_end_node_ptr = src_end_node_ptr->next_node_ptr;
        src_end_ptr = src_end_node_ptr->front_ptr - 1;
    }
    if (dst_ptr < dst_node_ptr->front_ptr) {
        dst_node_ptr = dst_node_ptr->previous_node_ptr;
        dst_ptr = dst_node_ptr->back_ptr - 1;
    }
    else if (dst_ptr >= dst_node_ptr->back_ptr) {
        dst_node_ptr = dst_node_ptr->next_node_ptr;
        dst_ptr = dst_node_ptr->front_ptr;
    }
    if (src_begin_ptr < src_begin_node_ptr->front_ptr) {
        src_begin_node_ptr = src_begin_node_ptr->previous_node_ptr;
        src_begin_ptr = src_begin_node_ptr->back_ptr - 1;
    }
    else if (src_begin_ptr >= src_begin_node_ptr->back_ptr) {
        src_begin_node_ptr = src_begin_node_ptr->next_node_ptr;
        src_begin_ptr = src_begin_node_ptr->front_ptr;
    }
    while (src_begin_node_ptr != src_end_node_ptr) {
        temp_dst_ptr = dst_ptr;
        temp_src_begin_ptr = src_begin_ptr;
        if (dst_left_size < src_left_size) {
            temp_src_end_ptr = temp_src_begin_ptr - dst_left_size;
            dst_node_ptr = dst_node_ptr->next_node_ptr;
            dst_ptr = dst_node_ptr->front_ptr;
            src_begin_ptr = temp_src_end_ptr;
            src_left_size -= dst_left_size;
            dst_left_size = dst_node_ptr->capacity;
        }
        else {
            temp_src_end_ptr = temp_src_begin_ptr - src_left_size;
            src_begin_node_ptr = src_begin_node_ptr->previous_node_ptr;
            src_begin_ptr = src_begin_node_ptr->back_ptr - 1;
            dst_ptr += src_left_size;
            dst_left_size -= src_left_size;
            src_left_size = src_begin_node_ptr->capacity;
        }
        while (temp_src_begin_ptr > temp_src_end_ptr) {
            if constexpr (kIfCreate) {
                new(reinterpret_cast<Void*>(temp_dst_ptr)) ObjectType(*temp_src_begin_ptr);
            }
            else {
                *temp_dst_ptr = *temp_src_begin_ptr;
            }
            ++temp_dst_ptr;
            --temp_src_begin_ptr;
        }
    }
    src_left_size = src_begin_ptr - src_end_ptr;
    while (dst_left_size < src_left_size) {
        temp_dst_ptr = dst_ptr;
        temp_src_end_ptr = src_begin_ptr - dst_left_size;
        dst_node_ptr = dst_node_ptr->next_node_ptr;
        dst_ptr = dst_node_ptr->front_ptr;
        src_left_size -= dst_left_size;
        dst_left_size = dst_node_ptr->capacity;
        while (src_begin_ptr > temp_src_end_ptr) {
            if constexpr (kIfCreate) {
                new(reinterpret_cast<Void*>(temp_dst_ptr)) ObjectType(*src_begin_ptr);
            }
            else {
                *temp_dst_ptr = *src_begin_ptr;
            }
            ++temp_dst_ptr;
            --src_begin_ptr;
        }
    }
    while (src_begin_ptr > src_end_ptr) {
        if constexpr (kIfCreate) {
            new(reinterpret_cast<Void*>(dst_ptr)) ObjectType(*src_begin_ptr);
        }
        else {
            *dst_ptr = *src_begin_ptr;
        }
        ++dst_ptr;
        --src_begin_ptr;
    }
    *dst_end_ptr_ptr = dst_ptr;
    *dst_end_node_ptr_ptr = dst_node_ptr;
}


template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::CreateAndCopyObjectsReverseP(ObjectType* dst_ptr,
                                                                 DataNode* dst_node_ptr,
                                                                 const ObjectType* src_begin_ptr,
                                                                 const DataNode* src_begin_node_ptr,
                                                                 const ObjectType* src_end_ptr,
                                                                 const DataNode* src_end_node_ptr,
                                                                 ObjectType** dst_end_ptr_ptr,
                                                                 DataNode** dst_end_node_ptr_ptr) noexcept {
    CopyObjectsReverseBaseP<kCreateAndCopyObjectsReverse>(dst_ptr, dst_node_ptr, 
                                                          src_begin_ptr, src_begin_node_ptr, 
                                                          src_end_ptr, src_end_node_ptr, 
                                                          dst_end_ptr_ptr, dst_end_node_ptr_ptr);
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::CopyObjectsReverseP(ObjectType* dst_ptr, 
                                                        DataNode* dst_node_ptr,
                                                        const ObjectType* src_begin_ptr,
                                                        const DataNode* src_begin_node_ptr,
                                                        const ObjectType* src_end_ptr,
                                                        const DataNode* src_end_node_ptr,
                                                        ObjectType** dst_end_ptr_ptr,
                                                        DataNode** dst_end_node_ptr_ptr) noexcept {
    CopyObjectsReverseBaseP<kCopyObjectsReverse>(dst_ptr, dst_node_ptr,
                                                 src_begin_ptr, src_begin_node_ptr,
                                                 src_end_ptr, src_end_node_ptr,
                                                 dst_end_ptr_ptr, dst_end_node_ptr_ptr);
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::FindPlaceP(const ObjectType* front_ptr, 
                                               const DataNode* begin_node_ptr, 
                                               IndexType num,
                                               ObjectType** find_ptr_ptr, 
                                               DataNode** find_node_ptr_ptr) noexcept {
    IndexType node_left_num = begin_node_ptr->back_ptr - front_ptr;
    if (num > node_left_num) {
        do {
            num -= node_left_num;
            begin_node_ptr = begin_node_ptr->next_node_ptr;
            node_left_num = begin_node_ptr->capacity;
        } while (num > node_left_num);
        front_ptr = begin_node_ptr->front_ptr;
    }
    *find_node_ptr_ptr = begin_node_ptr;
    *find_ptr_ptr = front_ptr + num;
}

template<typename ObjectType, Bool kIfUnique>
static IndexType ZDeque<ObjectType, kIfUnique>::CalculateLengthP(const ObjectType* front_ptr, 
                                                                 const DataNode* begin_node_ptr,
                                                                 const ObjectType* back_ptr,
                                                                 const DataNode* end_node_ptr) noexcept {
    IndexType num = (back_ptr - end_node_ptr->front_ptr) - (front_ptr - begin_node_ptr->front_ptr);
    while (begin_node_ptr != end_node_ptr) {
        num += begin_node_ptr->capacity;
        begin_node_ptr = begin_node_ptr->next_node_ptr;
    }
    return num;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::CreateContainerP(IndexType capacity) noexcept {
    MemoryType need_memory_size = ((capacity >> 1) * sizeof(ObjectType)) + sizeof(DataNode);
    MemoryType node_apply_mrmory_size;
    MemoryType temp_node_apply_mrmory_size;
    //Applys 2 nodes instead.
    DataNode* temp_node_1_ptr =
        reinterpret_cast<DataNode*>(memory_pool::ApplyMemory(need_memory_size, &node_apply_mrmory_size));
    DataNode* temp_node_2_ptr =
        reinterpret_cast<DataNode*>(memory_pool::ApplyMemory(need_memory_size, &temp_node_apply_mrmory_size));
    temp_node_1_ptr->next_node_ptr = temp_node_1_ptr->previous_node_ptr = temp_node_2_ptr;
    temp_node_1_ptr->capacity = (node_apply_mrmory_size - sizeof(DataNode)) / sizeof(ObjectType);
    temp_node_1_ptr->front_ptr = reinterpret_cast<ObjectType*>(temp_node_1_ptr + 1);
    temp_node_1_ptr->back_ptr = temp_node_1_ptr->front_ptr + temp_node_1_ptr->capacity;

    temp_node_2_ptr->next_node_ptr = temp_node_2_ptr->previous_node_ptr = temp_node_1_ptr;
    temp_node_2_ptr->capacity = (temp_node_apply_mrmory_size - sizeof(DataNode)) / sizeof(ObjectType);
    temp_node_2_ptr->front_ptr = reinterpret_cast<ObjectType*>(temp_node_2_ptr + 1);
    temp_node_2_ptr->back_ptr = temp_node_2_ptr->front_ptr + temp_node_2_ptr->capacity;

    front_node_ptr_ = temp_node_1_ptr;

    capacity_ = temp_node_1_ptr->capacity + temp_node_2_ptr->capacity;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::ExtendContainerP(IndexType capacity) noexcept {
    if (front_node_ptr_ == nullptr) {
        CreateContainerP(capacity);
        back_node_ptr_ = front_node_ptr_->previous_node_ptr;
        front_ptr_ = front_node_ptr_->front_ptr;
        back_ptr_ = back_node_ptr_->back_ptr - 1;
        return;
    }
    MemoryType need_memory_size = capacity * sizeof(ObjectType) + sizeof(DataNode);
    MemoryType apply_mrmory_size;
    //Initialize the node.
    DataNode* temp_node_ptr =
        reinterpret_cast<DataNode*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));
    temp_node_ptr->capacity = (apply_mrmory_size - sizeof(DataNode)) / sizeof(ObjectType);
    temp_node_ptr->front_ptr = reinterpret_cast<ObjectType*>(temp_node_ptr + 1);
    temp_node_ptr->back_ptr = temp_node_ptr->front_ptr + temp_node_ptr->capacity;
    //Update the container.
    front_node_ptr_->previous_node_ptr = temp_node_ptr;
    back_node_ptr_->next_node_ptr = temp_node_ptr;
    temp_node_ptr->next_node_ptr = front_node_ptr_;
    temp_node_ptr->previous_node_ptr = back_node_ptr_;
    capacity_ += temp_node_ptr->capacity;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::ShrinkContainerP() noexcept {
    if (capacity_ == 0) {
        return;
    }
    DataNode* temp_node_ptr = front_node_ptr_->previous_node_ptr;
    while (temp_node_ptr != back_node_ptr_) {
        DataNode* delete_node_ptr = temp_node_ptr;
        capacity_ -= delete_node_ptr->capacity;
        temp_node_ptr = temp_node_ptr->previous_node_ptr;
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(delete_node_ptr));
    }
    front_node_ptr_->previous_node_ptr = back_node_ptr_;
    back_node_ptr_->next_node_ptr = front_node_ptr_;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::DestroyContainerP() noexcept {
    if (capacity_ == 0) {
        return;
    }
    if (size_ != 0) {
        DestroyObjectsP(front_ptr_, front_node_ptr_, back_ptr_ + 1, back_node_ptr_);
    }
    //Release the memeorys.
    DataNode* temp_front_node_ptr = front_node_ptr_;
    do {
        DataNode* delete_node_ptr = temp_front_node_ptr;
        temp_front_node_ptr = temp_front_node_ptr->next_node_ptr;
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(delete_node_ptr));
    } while (temp_front_node_ptr != front_node_ptr_);
    //Set zero.
    front_node_ptr_ = nullptr;
    back_node_ptr_ = nullptr;
    front_ptr_ = nullptr;
    back_ptr_ = nullptr;
    capacity_ = 0;
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::ZDequeOrderP(const ObjectType* front_ptr, 
                                                             const DataNode* begin_node_ptr,
                                                             const ObjectType* back_ptr, 
                                                             const DataNode* end_node_ptr) noexcept {
    size_ = CalculateLengthP(front_ptr, begin_node_ptr, back_ptr, end_node_ptr);
    CreateContainerP(size_);
    front_ptr_ = front_node_ptr_->front_ptr;
    CreateAndCopyObjectsP(front_ptr_, front_node_ptr_,
                          front_ptr, begin_node_ptr, 
                          back_ptr, end_node_ptr, 
                          &back_ptr_, &back_node_ptr_);
    back_ptr_ -= 1;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::ZDequeReverseP(const ObjectType* front_ptr,
                                                               const DataNode* begin_node_ptr,
                                                               const ObjectType* back_ptr,
                                                               const DataNode* end_node_ptr) noexcept {
    size_ = CalculateLengthP(back_ptr, end_node_ptr, front_ptr, begin_node_ptr);
    CreateContainerP(size_);
    front_ptr_ = front_node_ptr_->front_ptr;
    CreateAndCopyObjectsReverseP(front_ptr_, front_node_ptr_,
                                 front_ptr, begin_node_ptr, 
                                 back_ptr, end_node_ptr,
                                 &back_ptr_, &back_node_ptr_);
    back_ptr_ -= 1;
}

template<typename ObjectType, Bool kIfUnique>
Void ZDeque<ObjectType, kIfUnique>::CopyP(const ZDeque& deque) noexcept {
    IndexType need_capacity = deque.size_ +  - capacity_ + 
                              static_cast<IndexType>(front_ptr_ - front_node_ptr_->front_ptr);
    if (need_capacity > 0) {
        ExtendContainerP(need_capacity);
    }
    if constexpr (kIfUnique) {
        ObjectType* temp_ptr;
        DataNode* temp_node_ptr;
        if (deque.size_ > size_) {
            ObjectType* temp_end_ptr;
            DataNode* temp_end_node_ptr;
            FindPlaceP(deque.front_ptr_, deque.front_node_ptr_, size_, &temp_ptr, &temp_node_ptr);
            CopyObjectsP(front_ptr_, front_node_ptr_, 
                         deque.front_ptr_, deque.front_node_ptr_, 
                         temp_ptr, temp_node_ptr,
                         &temp_end_ptr, &temp_end_node_ptr);
            CreateAndCopyObjectsP(temp_end_ptr, temp_end_node_ptr,
                                  temp_ptr, temp_node_ptr,
                                  deque.back_ptr_ + 1, deque.back_node_ptr_,
                                  &back_ptr_, &back_node_ptr_);
            back_ptr_ -= 1;
        }
        else {
            CopyObjectsP(front_ptr_, front_node_ptr_,
                         deque.front_ptr_, deque.front_node_ptr_,
                         deque.back_ptr_ + 1, deque.back_node_ptr_,
                         &temp_ptr, &temp_node_ptr);
            DestroyObjectsP(temp_ptr, temp_node_ptr, back_ptr_ + 1, back_node_ptr_);
            back_ptr_ = temp_ptr - 1;
            back_node_ptr_ = temp_node_ptr;
        }
    }
    else {
        CopyObjectsP(front_ptr_, front_node_ptr_,
                     deque.front_ptr_, deque.front_node_ptr_,
                     deque.back_ptr_ + 1, deque.back_node_ptr_,
                     &back_ptr_, &back_node_ptr_);
        back_ptr_ -= 1;
    }
    size_ = deque.size_;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZDeque<ObjectType, kIfUnique>::MoveP(ZDeque&& deque) {
    front_node_ptr_ = deque.front_node_ptr_;
    back_node_ptr_ = deque.back_node_ptr_;
    front_ptr_ = deque.front_ptr_;
    back_ptr_ = deque.back_ptr_;
    capacity_ = deque.capacity_;
    size_ = deque.size_;
    deque.front_node_ptr_ = nullptr;
    deque.back_node_ptr_ = nullptr;
    deque.front_ptr_ = nullptr;
    deque.back_ptr_ = nullptr;
    deque.capacity_ = 0;
    deque.size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
ObjectType* ZDeque<ObjectType, kIfUnique>::AtP(const IndexType index) noexcept {
    DataNode* temp_node_ptr = front_node_ptr_;
    index = static_cast<IndexType>(front_ptr_ - temp_node_ptr->front_ptr) + index;
    while (index >= temp_node_ptr->capacity) {
        index -= temp_node_ptr->capacity;
        temp_node_ptr = temp_node_ptr->next_node_ptr;
    }
    while (index < 0) {
        temp_node_ptr = temp_node_ptr->previous_node_ptr;
        index += temp_node_ptr->capacity;
    }
    return temp_node_ptr->AtPtr(index);
}

}//zengine

#endif // !Z_CORE_Z_DEQUE_H_