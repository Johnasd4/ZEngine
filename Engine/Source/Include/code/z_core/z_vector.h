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
#ifndef Z_CORE_Z_VECTOR_H_
#define Z_CORE_Z_VECTOR_H_

#include "internal/drive.h"

#include "m_error_message.h"
#include "z_object.h"

namespace zengine {

template<typename ObjectType, Bool kIfUnique>
class ZVector;

namespace internal {

template<typename ObjectType>
class ZVectorIteratorBase {
public:
    FORCEINLINE ZVectorIteratorBase(ObjectType* object_ptr) : object_ptr_(object_ptr) {}
    FORCEINLINE ZVectorIteratorBase(const ZVectorIteratorBase& iterator) { CopyP(iterator); }
    FORCEINLINE ZVectorIteratorBase(ZVectorIteratorBase&& iterator) { 
        MoveP(std::forward<ZVectorIteratorBase>(iterator)); 
    }

    FORCEINLINE ZVectorIteratorBase& operator=(ObjectType* object_ptr) {
        object_ptr_ = object_ptr;
        return *this;
    }
    FORCEINLINE ZVectorIteratorBase& operator=(const ZVectorIteratorBase& iterator) {
        CopyP(iterator);
        return *this;
    }
    FORCEINLINE ZVectorIteratorBase& operator=(ZVectorIteratorBase&& iterator) {
        MoveP(std::forward<ZVectorIteratorBase>(iterator));
        return *this;
    }

    NODISCARD FORCEINLINE Bool operator==(const ZVectorIteratorBase& iterator) const {
        return this == &iterator;
    }
    NODISCARD FORCEINLINE Bool operator!=(const ZVectorIteratorBase& iterator) const {
        return this != &iterator;
    }

    NODISCARD FORCEINLINE ObjectType& operator*() const { return *object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* operator->() const { return object_ptr_; }


    NODISCARD FORCEINLINE operator ObjectType* () const { return object_ptr_; }

    FORCEINLINE ~ZVectorIteratorBase() {}

    NODISCARD FORCEINLINE ObjectType& object() const { return *object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* object_ptr() const { return object_ptr_; }

protected:
    ObjectType* object_ptr_;

private:
    FORCEINLINE Void CopyP(const ZVectorIteratorBase& iterator) {
        object_ptr_ = iterator.object_ptr_;
    }

    FORCEINLINE Void MoveP(ZVectorIteratorBase&& iterator) {
        object_ptr_ = iterator.object_ptr_;
        iterator.object_ptr_ = nullptr;
    }
};

template<typename ObjectType>
class ZVectorIterator : public ZVectorIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) const { return SuperType::object_ptr_[index]; }

    FORCEINLINE ZVectorIterator& operator+=(IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE ZVectorIterator& operator-=(IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }

    FORCEINLINE ZVectorIterator& operator++() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZVectorIterator& operator++(IndexType) {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZVectorIterator& operator--() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZVectorIterator& operator--(IndexType) {
        --SuperType::object_ptr_;
        return *this;
    }
    NODISCARD FORCEINLINE ZVectorIterator operator+(IndexType data_num) const {
        return ZVectorIterator(SuperType::object_ptr_ + data_num);
    }
    NODISCARD FORCEINLINE ZVectorIterator operator-(IndexType data_num) const {
        return ZVectorIterator(SuperType::object_ptr_ - data_num);
    }

    NODISCARD FORCEINLINE Bool operator>(const ZVectorIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator>=(const ZVectorIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator<(const ZVectorIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator<=(const ZVectorIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE IndexType operator-(const ZVectorIterator& iterator) const {
        return static_cast<IndexType>(SuperType::object_ptr_ - iterator.SuperType::object_ptr_);
    }

protected:
    using SuperType = ZVectorIteratorBase<ObjectType>;
};

template<typename ObjectType>
class ZVectorReverseIterator : public ZVectorIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) const { return SuperType::object_ptr_[-index]; }

    FORCEINLINE ZVectorReverseIterator& operator+=(IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE ZVectorReverseIterator& operator-=(IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }

    FORCEINLINE ZVectorReverseIterator& operator++() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZVectorReverseIterator& operator++(IndexType) {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZVectorReverseIterator& operator--() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZVectorReverseIterator& operator--(IndexType) {
        ++SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE ZVectorReverseIterator operator+(IndexType data_num) const {
        return ZVectorReverseIterator(SuperType::object_ptr_ - data_num);
    }
    NODISCARD FORCEINLINE ZVectorReverseIterator operator-(IndexType data_num) const {
        return ZVectorReverseIterator(SuperType::object_ptr_ + data_num);
    }

    NODISCARD FORCEINLINE Bool operator>(const ZVectorReverseIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator>=(const ZVectorReverseIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator<(const ZVectorReverseIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator<=(const ZVectorReverseIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE IndexType operator-(const ZVectorReverseIterator& iterator) const {
        return static_cast<IndexType>(iterator.SuperType::object_ptr_ - SuperType::object_ptr_);
    }

protected:
    using SuperType = ZVectorIteratorBase<ObjectType>;
};

/*
    Order iterator types.
*/
template<typename IteratorType, typename ObjectType>
concept kIsOrderVectorIterator = kSameType<IteratorType, ZVectorIterator<ObjectType>> ||
                                 kSameType<IteratorType, ZVectorIterator<const ObjectType>>;

/*
    Reverse iterator types.
*/
template<typename IteratorType, typename ObjectType>
concept kIsReverseVectorIterator = kSameType<IteratorType, ZVectorReverseIterator<ObjectType>> ||
                                   kSameType<IteratorType, ZVectorReverseIterator<const ObjectType>>;

/*
    Const iterator types.
*/
template<typename IteratorType, typename ObjectType>
concept kIsConstVectorIterator = kSameType<IteratorType, ZVectorIterator<const ObjectType>> ||
                                 kSameType<IteratorType, ZVectorReverseIterator<const ObjectType>>;

/*
    All iterator types.
*/
template<typename IteratorType, typename ObjectType>
concept kIsVectorIterator = kIsOrderVectorIterator<IteratorType, ObjectType> ||
                            kIsReverseVectorIterator<IteratorType, ObjectType>;

}//internal

/*
    Vector caintainer.
    If kIfUnique is true, will call the constuctor when the object is created.
*/
template<typename ObjectType, Bool kIfUnique = kIsClass<ObjectType>>
class ZVector : public ZObject {
private:
    static constexpr Float32 kAutoExtendMulFactor = 1.5F;

public:
    using Iterator = internal::ZVectorIterator<ObjectType>;
    using ConstIterator = internal::ZVectorIterator<const ObjectType>;
    using ReverseIterator = internal::ZVectorReverseIterator<ObjectType>;
    using ConstReverseIterator = internal::ZVectorReverseIterator<const ObjectType>;

    ZVector() noexcept;
    ZVector(const ZVector& vector) noexcept;
    ZVector(ZVector&& vector) noexcept;

    ZVector(IndexType capacity) noexcept;
    /*
        Fills the container by the object constructed by the arguements.
    */
    template<typename... ArgsType>
    ZVector(IndexType capacity, ArgsType&&... args) noexcept;    

    ZVector(Iterator begin, Iterator end) noexcept : SuperType() { ZVectorP(begin, end); }
    ZVector(ConstIterator begin, ConstIterator end) noexcept : SuperType() { ZVectorP(begin, end); }
    ZVector(ReverseIterator begin, ReverseIterator end) noexcept : SuperType() { ZVectorP(begin, end); }
    ZVector(ConstReverseIterator begin, ConstReverseIterator end) noexcept : SuperType() { ZVectorP(begin, end); }

    FORCEINLINE ZVector& operator=(const ZVector& vector);
    inline ZVector& operator=(ZVector&& vector) noexcept;

    NODISCARD FORCEINLINE Bool operator==(const ZVector& vector) noexcept { 
        return this == &vector;
    }    
    NODISCARD FORCEINLINE Bool operator!=(const ZVector& vector) noexcept { 
        return this != &vector;
    }

    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) { 
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_[index]; 
    }
    NODISCARD FORCEINLINE const ObjectType& operator[](IndexType index) const { 
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_[index]; 
    }

    ~ZVector() noexcept;


    NODISCARD FORCEINLINE ObjectType& At(IndexType index) {
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_[index];
    }
    NODISCARD FORCEINLINE const ObjectType& At(IndexType index) const {
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_[index];
    }
    NODISCARD FORCEINLINE ObjectType* AtPtr(IndexType index) {
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_ + index;
    }
    NODISCARD FORCEINLINE const ObjectType* AtPtr(IndexType index) const {
        DEBUG(index < 0 || index >= size_, "Index out of bounds!");
        return data_ptr_ + index;
    }
    NODISCARD FORCEINLINE ObjectType& Front() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[0]; 
    }
    NODISCARD FORCEINLINE const ObjectType& Front() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[0];
    }
    NODISCARD FORCEINLINE ObjectType* FrontPtr() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_;
    }
    NODISCARD FORCEINLINE const ObjectType* FrontPtr() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_;
    }
    NODISCARD FORCEINLINE ObjectType& Back() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[size_ - 1];
    }
    NODISCARD FORCEINLINE const ObjectType& Back() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_[size_ - 1];
    }
    NODISCARD FORCEINLINE ObjectType* BackPtr() {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_ + (size_ - 1);
    }
    NODISCARD FORCEINLINE const ObjectType* BackPtr() const {
        DEBUG(size_ == 0, "No object exists!");
        return data_ptr_ + (size_ - 1);
    }

    NODISCARD FORCEINLINE IndexType size() const { return size_; }
    NODISCARD FORCEINLINE IndexType capacity() const { return capacity_; }
    NODISCARD FORCEINLINE const ObjectType* data_ptr() const { return data_ptr_; }

    /*
        The iterator funcions.
    */
    NODISCARD FORCEINLINE Iterator Begin() {
        DEBUG(data_ptr_ == nullptr, "Vector not exist!");
        return Iterator(data_ptr_); 
    }
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const { 
        DEBUG(data_ptr_ == nullptr, "Vector not exist!"); 
        return ConstIterator(data_ptr_); 
    }
    NODISCARD FORCEINLINE ReverseIterator ReverseBegin() { 
        DEBUG(data_ptr_ == nullptr, "Vector not exist!"); 
        return ReverseIterator(data_ptr_ + size_ - 1); 
    }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseBegin() const {
        DEBUG(data_ptr_ == nullptr, "Vector not exist!");
        return ConstReverseIterator(data_ptr_ + size_ - 1);
    }
    NODISCARD FORCEINLINE Iterator End() { 
        DEBUG(data_ptr_ == nullptr, "Vector not exist!"); 
        return Iterator(data_ptr_ + size_); 
    }
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const { 
        DEBUG(data_ptr_ == nullptr, "Vector not exist!"); 
        return ConstIterator(data_ptr_ + size_); 
    }
    NODISCARD FORCEINLINE ReverseIterator ReverseEnd() { 
        DEBUG(data_ptr_ == nullptr, "Vector not exist!"); 
        return ReverseIterator(data_ptr_ - 1); 
    }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseEnd() const {
        DEBUG(data_ptr_ == nullptr, "Vector not exist!");
        return ConstReverseIterator(data_ptr_ - 1);
    }

    NODISCARD FORCEINLINE Bool Empty() { return size_ == 0; }
    NODISCARD FORCEINLINE Bool Exist() { return data_ptr_ != nullptr; }

    /*
        Resize the vector, If the given size is smaller then the current size,
        the function will cut remove the extra objects. If the given size is
        bigger then the current size, the function will fill the extra size with
        the object constructed by the arguements.
    */
    template<typename... ArgsType>
    inline Void Resize(IndexType size, ArgsType&&... args) noexcept;

    /*
        Will extend the capacity by the given capacity, if the smaller then the
        given capacity, this function will do nothing, otherwise the capacity will
        entend to a size that equals or bigger then the given capacity.
    */
    FORCEINLINE Void Reserve(IndexType capacity);

    /*
        Shrinks the vector to the minimum capacity that can fit the current size.
    */
    FORCEINLINE Void ShrinkToFit();

    /*
        Remove the object at the back of the vector.
    */
    FORCEINLINE Void PopBack();
    /*
        Remove the object at the back of the vector. 
        Give the authority to the given address.
    */
    FORCEINLINE Void PopBack(ObjectType* object_ptr);

    /*
        Create an object at the back of the vector by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, will
        only add the size of the vector.
    */
    template<typename... ArgsType>
    FORCEINLINE Void PushBack(ArgsType&&... args);
    /*
        Create objects at the back of the vector by calling the constructor with
        the arguements. If kIfUnique is false and no arguements, 
        will only add the size of the vector.
    */
    template<typename... ArgsType>
    inline Void PushBacks(IndexType num, ArgsType&&... args) noexcept;
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the vector.
    */
    FORCEINLINE Void PushBacks(Iterator src_begin, Iterator src_end) {
        PushBacksP(src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the vector.
    */
    FORCEINLINE Void PushBacks(ConstIterator src_begin, ConstIterator src_end) {
        PushBacksP(src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the vector.
    */
    FORCEINLINE Void PushBacks(ReverseIterator src_begin, ReverseIterator src_end) {
        PushBacksP(src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the vector.
    */
    FORCEINLINE Void PushBacks(ConstReverseIterator src_begin, ConstReverseIterator src_end) {
        PushBacksP(src_begin, src_end);
    }

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    inline Void EmplaceBack(ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    FORCEINLINE Iterator Insert(IndexType index, ArgsType&&... args) { 
        return InsertP(Iterator(data_ptr_ + index), std::forward<ArgsType>(args)...);
    }
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE Iterator Insert(Iterator dst, ArgsType&&... args) {
        return InsertP(dst, std::forward<ArgsType>(args)...);
    }
    /*
        Inserts before the iterator. Returns the iterator that points at the newest object.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE ReverseIterator Insert(ReverseIterator dst, ArgsType&&... args) {
        return InsertP(dst, std::forward<ArgsType>(args)...);;
    }

    /*
        Inserts before the index. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    FORCEINLINE Iterator Inserts(IndexType index, IndexType num, ArgsType&&... args) {
        return InsertsP(Iterator(index), num, std::forward<ArgsType>(args)...);
    }
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE Iterator Inserts(Iterator dst, IndexType num, ArgsType&&... args) {
        return InsertsP(dst, num, std::forward<ArgsType>(args)...);
    }
    /*
        Inserts before the iterator. Returns the iterator that points at the first new object.
    */
    template<typename... ArgsType>
    NODISCARD FORCEINLINE ReverseIterator Inserts(ReverseIterator dst, IndexType num, ArgsType&&... args) {
        return InsertsP(dst, num, std::forward<ArgsType>(args)...);
    }

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE Iterator Inserts(IndexType index, Iterator src_begin, Iterator src_end) {
        return InsertsP(Iterator(index), src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE Iterator Inserts(IndexType index, ConstIterator src_begin, ConstIterator src_end) {
        return InsertsP(Iterator(index), src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE Iterator Inserts(IndexType index, ReverseIterator src_begin,ReverseIterator src_end) {
        return InsertsP(Iterator(index), src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    FORCEINLINE Iterator Inserts(IndexType index, ConstReverseIterator src_begin, ConstReverseIterator src_end) {
        return InsertsP(Iterator(index), src_begin, src_end);
    }

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE Iterator Inserts(Iterator dst, Iterator src_begin, Iterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE Iterator Inserts(Iterator dst, ConstIterator src_begin, ConstIterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE Iterator Inserts(Iterator dst, ReverseIterator src_begin, ReverseIterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE Iterator Inserts(Iterator dst, ConstReverseIterator src_begin, ConstReverseIterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE ReverseIterator Inserts(ReverseIterator dst, Iterator src_begin, Iterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE ReverseIterator Inserts(ReverseIterator dst, ConstIterator src_begin, ConstIterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE ReverseIterator Inserts(ReverseIterator dst,
                                                  ReverseIterator src_begin, ReverseIterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }
    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the iterator that points at the first new object.
    */
    NODISCARD FORCEINLINE ReverseIterator Inserts(ReverseIterator dst,
                                                  ConstReverseIterator src_begin, ConstReverseIterator src_end) {
        return InsertsP(dst, src_begin, src_end);
    }

    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    FORCEINLINE Iterator Erase(IndexType index) { return EraseP(Iterator(data_ptr_ + index)); }
    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE Iterator Erase(Iterator dst) { return EraseP(dst); }
    /*
        Erases the object by the index.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE ReverseIterator Erase(ReverseIterator dst) { return EraseP(dst); }

    /*
        Erases the num of objects that starts at the given index.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE Iterator Erases(IndexType index, IndexType num) {
        return ErasesP(Iterator(data_ptr_ + index), Iterator(data_ptr_ + (index + num)));
    }
    /*
        Erases the num of objects that starts at the given iterator.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE Iterator Erases(Iterator dst, IndexType num) { return ErasesP(dst, dst + num); }
    /*
        Erases the num of objects that starts at the given iterator.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE ReverseIterator Erases(ReverseIterator dst, IndexType num) { return ErasesP(dst, dst + num); }
    /*
        Erases the object between begin and end, involves begin, but don't involves end.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE Iterator Erases(Iterator dst_begin, Iterator dst_end) { return ErasesP(dst_begin, dst_end); }
    /*
        Erases the object between begin and end, involves begin, but don't involves end.
        Returns the iterator that points at the next object.
    */
    NODISCARD FORCEINLINE ReverseIterator Erases(ReverseIterator dst_begin, ReverseIterator dst_end) {
        return ErasesP(dst_begin, dst_end);
    }

    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    FORCEINLINE Void Emplace(IndexType index, ArgsType&&... args) {
        EmplaceP(Iterator(data_ptr_ + index), std::forward<ArgsType>(args)...);
    }
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    FORCEINLINE Void Emplace(Iterator dst, ArgsType&&... args) {
        EmplaceP(dst, std::forward<ArgsType>(args)...);
    }
    /*
        Calls the constructor with the arguements.
    */
    template<typename... ArgsType>
    FORCEINLINE Void Emplace(ReverseIterator dst, ArgsType&&... args) {
        EmplaceP(dst, std::forward<ArgsType>(args)...);
    }

    /*
        Construct the vector by filling it with the given amount of objects.
        The object is constructed by the arguements.
        If kIfUnique is false and no arguements, will only add the 
        size of the vector.
    */
    template<typename... ArgsType>
    Void Assign(IndexType num, ArgsType&&... args) noexcept;
    /*
        Construct the vector by filling it objects between the iterators.
    */
    FORCEINLINE Void Assign(Iterator src_begin, Iterator src_end) noexcept { AssignP(src_begin, src_end); }
    /*
        Construct the vector by filling it objects between the iterators.
    */
    FORCEINLINE Void Assign(ConstIterator src_begin, ConstIterator src_end) noexcept { AssignP(src_begin, src_end); }
    /*
        Construct the vector by filling it objects between the iterators.
    */
    FORCEINLINE Void Assign(ReverseIterator src_begin, ReverseIterator src_end) noexcept { 
        AssignP(src_begin, src_end); 
    }
    /*
        Construct the vector by filling it objects between the iterators.
    */
    FORCEINLINE Void Assign(ConstReverseIterator src_begin, ConstReverseIterator src_end) noexcept {
        AssignP(src_begin, src_end);
    }

    /*
        The small object on the front.
    */
    Void Sort() noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(IndexType begin, IndexType end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(IndexType begin, IndexType end, CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(Iterator begin, Iterator& end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(Iterator begin, Iterator end, CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(ReverseIterator begin, ReverseIterator end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
    requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(ReverseIterator begin, ReverseIterator end, CompareFunction&& compare_function) noexcept;

    /*
        Destroys all the objects in the vector, does not release the memory.
    */
    Void Clear() noexcept;
    /*
        Destroys all the objects in the vector, release the memory.
    */
    FORCEINLINE Void Destroy();

protected:
    using SuperType = ZObject;

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
    template<typename DstIteratorType, typename... ArgsType>
    requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
    FORCEINLINE static Void CreateObjectP(DstIteratorType dst, ArgsType&&... args);

    /*
        Destroys an object at the certain place. Will Call Destrctor if needed.
    */
    template<typename DstIteratorType>
    requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
    FORCEINLINE static Void DestroyObjectP(DstIteratorType dst);

    /*
        The base function of create and destroy objests.
    */
    template<Bool kIfCreate, typename DstIteratorType, typename... ArgsType>
    requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
    FORCEINLINE static Void CreateDestroyObjectsBaseP(DstIteratorType dst_begin, DstIteratorType dst_end, 
                                                      ArgsType&&... args);

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the Constrctor if needed.
    */
    template<typename IteratorType, typename... ArgsType>
    requires (!internal::kIsConstVectorIterator<IteratorType, ObjectType>)
    inline static Void CreateObjectsP(IteratorType begin, IteratorType end, ArgsType&&... args) noexcept {
        CreateDestroyObjectsBaseP<kCreateObjects>(begin, end, std::forward<ArgsType>(args)...);
    }

    /*
        Destroy the objects by the given arguements([begin, end)).
        Will call the destrctor if this object class's member kIfUnique is true.
    */
    template<typename IteratorType>
    requires (!internal::kIsConstVectorIterator<IteratorType, ObjectType>)
    inline static Void DestroyObjectsP(IteratorType begin, IteratorType end) noexcept {
        CreateDestroyObjectsBaseP<kDestroyObjects>(begin, end);
    }

    /*
        The base function of copy objests.
    */
    template<Bool kIfCreate, typename DstIteratorType, typename SrcIteratorType>
    requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType> &&
              internal::kIsVectorIterator<SrcIteratorType, ObjectType>)
    FORCEINLINE static Void CopyObjectsBaseP(DstIteratorType dst, SrcIteratorType src_begin, SrcIteratorType src_end);

    /*
        Initialize the memory by the given arguements([begin, end)).
        Will call the copy constructor.
    */
    template<typename DstIteratorType, typename SrcIteratorType>
    requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType> &&
              internal::kIsVectorIterator<SrcIteratorType, ObjectType>)
    inline static Void CreateAndCopyObjectsP(DstIteratorType dst, SrcIteratorType src_begin, 
                                             SrcIteratorType src_end) noexcept {
        CopyObjectsBaseP<kCreateAndCopyObjects>(dst, src_begin, src_end);
    }

    /*
        Copy objects by the given pointer. Will call the copy assignment operator
        if this object class's member kIfUnique is true.
    */
    template<typename DstIteratorType, typename SrcIteratorType>
    requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType> &&
              internal::kIsVectorIterator<SrcIteratorType, ObjectType>)
    inline static Void CopyObjectsP(DstIteratorType dst, SrcIteratorType src_begin,
                                    SrcIteratorType src_end) noexcept {
        CopyObjectsBaseP<kCopyObjects>(dst, src_begin, src_end);
    }

    /*
        Creates the capacity by the given capacity, the final capacity might
        not equal the given capacity.
    */
    Void CreateContainerP(IndexType capacity) noexcept;
    /*
        Extends the capacity by the given capacity, the final capacity might
        not equal the given capacity.
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
        Container copy function.
    */
    Void CopyP(const ZVector& vector) noexcept;

    /*
        Container move function.
    */
    FORCEINLINE Void MoveP(ZVector&& vector);

    /*
        Constructor with two order iterators.
    */
    template<typename SrcIteratorType>
    requires internal::kIsVectorIterator<SrcIteratorType, ObjectType>
    FORCEINLINE Void ZVectorP(SrcIteratorType src_begin, SrcIteratorType src_end);

    /*
        Makes a copy of the objects between the iterators and push them to the
        back of the vector.
    */
    template<typename SrcIteratorType>
    requires internal::kIsVectorIterator<SrcIteratorType, ObjectType>
    Void PushBacksP(SrcIteratorType src_begin, SrcIteratorType src_end) noexcept;

    /*
        Inserts before the index. Returns the pointer that points at the newest object.
    */
    template<typename DstIteratorType, typename... ArgsType>
    requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
    NODISCARD DstIteratorType InsertP(DstIteratorType dst, ArgsType&&... args) noexcept;

    /*
        Inserts before the index. Returns the pointer that points at the first new object.
    */
    template<typename DstIteratorType, typename... ArgsType>
    requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
    NODISCARD DstIteratorType InsertsP(DstIteratorType dst, IndexType num, ArgsType&&... args) noexcept;

    /*
        Makes a copy of the objects between the iterators and insert them to the
        given place. Returns the pointer that points at the first new object.
    */
    template<typename DstIteratorType, typename SrcIteratorType>
    requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>&&
    internal::kIsVectorIterator<SrcIteratorType, ObjectType>)
    NODISCARD DstIteratorType InsertsP(DstIteratorType dst, SrcIteratorType src_begin, SrcIteratorType src_end) noexcept;

    /*
        Erases the object by the index.
        Returns the pointer that points at the next object.
    */
    template<typename DstIteratorType, typename... ArgsType>
    requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
    NODISCARD inline DstIteratorType EraseP(DstIteratorType dst) noexcept;

    /*
        Erases the num of objects that starts at the given index.
        Returns the pointer that points at the next object.
    */
    template<typename DstIteratorType, typename... ArgsType>
    requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
    NODISCARD inline DstIteratorType ErasesP(DstIteratorType dst_begin, DstIteratorType dst_end) noexcept;

    /*
        Calls the constructor with the arguements.
    */
    template<typename DstIteratorType, typename... ArgsType>
    requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
    inline Void EmplaceP(DstIteratorType dst, ArgsType&&... args) noexcept;

    /*
        Construct the vector by filling it objects between the iterators.
    */
    template<typename SrcIteratorType, typename... ArgsType>
    requires (!internal::kIsConstVectorIterator<SrcIteratorType, ObjectType>)
    Void AssignP(SrcIteratorType src_begin, SrcIteratorType src_end) noexcept;

    ObjectType* data_ptr_;
    IndexType capacity_;

    IndexType size_;
};

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector() noexcept
    : SuperType()
    , data_ptr_(nullptr)
    , capacity_(0)
    , size_(0)
{}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector(const ZVector& vector) noexcept
    : SuperType(vector)
{
    DEBUG(&vector == this, "The source and the target of the copy is the same!");
    CreateContainerP(vector.size_);
    CreateAndCopyObjectsP(Iterator(data_ptr_), Iterator(vector.data_ptr_), Iterator(vector.data_ptr_ + vector.size_));
    size_ = vector.size_;
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector(ZVector&& vector) noexcept
    : SuperType(std::forward<ZVector>(vector))
{
    DEBUG(&vector == this, "The source and the target of the copy is the same!");
    MoveP(std::forward<ZVector>(vector));
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::ZVector(IndexType capacity) noexcept
    : SuperType()
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainerP(capacity);
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
ZVector<ObjectType, kIfUnique>::ZVector(IndexType capacity, ArgsType&&... args) noexcept
    : SuperType()
{
    DEBUG(capacity < 0, "Negaive capacity not valid!");
    CreateContainerP(capacity);
    CreateObjectsP(data_ptr_, data_ptr_ + capacity, std::forward<ArgsType>(args)...);
    size_ = capacity;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE ZVector<ObjectType, kIfUnique>& ZVector<ObjectType, kIfUnique>::operator=(const ZVector& vector) {
    DEBUG(&vector == this, "The source and the target of the copy is the same!");
    SuperType::operator=(vector);
    CopyP(vector);
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
inline ZVector<ObjectType, kIfUnique>& ZVector<ObjectType, kIfUnique>::operator=(ZVector&& vector) noexcept {
    DEBUG(&vector == this, "The source and the target of the copy is the same!");
    SuperType::operator=(std::forward<ZVector>(vector));
    DestroyObjectsP(Iterator(data_ptr_), Iterator(data_ptr_ + size_));
    MoveP(std::forward<ZVector>(vector));
    return *this;
}

template<typename ObjectType, Bool kIfUnique>
ZVector<ObjectType, kIfUnique>::~ZVector() noexcept {
    DestroyContainerP();
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
inline Void ZVector<ObjectType, kIfUnique>::Resize(IndexType size, ArgsType&&... args) noexcept {
    DEBUG(size < 0, "Negaive size is not valid!");
    if (size_ < size) {
        if (size > capacity_) {
            ExtendContainerP(size);
        }
        CreateObjectsP(Iterator(data_ptr_ + size_), Iterator(data_ptr_ + size), std::forward<ArgsType>(args)...);
    }
    else {
        DestroyObjectsP(Iterator(data_ptr_ + size), Iterator(data_ptr_ + size_));
    }
    size_ = size;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::Reserve(IndexType capacity) {
    if (capacity > capacity_) {
        ExtendContainerP(capacity);
    }
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::ShrinkToFit() {
    ShrinkContainerP();
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::PopBack() {               
    DEBUG(size_ == 0, "No existing object to pop!");
    DestroyObjectP(Iterator(data_ptr_ + size_--));
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::PopBack(ObjectType* object_ptr) {
    DEBUG(size_ == 0, "No existing object to pop!");
    *object_ptr = std::move(data_ptr_[size_--]);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::PushBack(ArgsType&&... args) {
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjectP(Iterator(data_ptr_ + size_), std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
inline Void ZVector<ObjectType, kIfUnique>::PushBacks(IndexType num, ArgsType&&... args) noexcept {
    DEBUG(num < 0, "Negative pushing num not valid!");
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    CreateObjectsP(Iterator(data_ptr_ + size_), Iterator(data_ptr_ + new_size), std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
inline Void ZVector<ObjectType, kIfUnique>::EmplaceBack(ArgsType&&... args) noexcept {
    DEBUG(size_ == 0, "No existing object to emplace!");
    Iterator dst(data_ptr_ + (size_ - 1));
    DestroyObjectP(dst);
    CreateObjectP(dst, std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfUnique>
template<typename... ArgsType>
Void ZVector<ObjectType, kIfUnique>::Assign(IndexType num, ArgsType&&... args) noexcept {
    DEBUG(num < 0, "Negative assign num not valid!");
    IndexType new_size = num;
    if (new_size > capacity_) {
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
    }
    DestroyObjectsP(Iterator(data_ptr_), Iterator(data_ptr_ + size_));
    CreateObjectsP(Iterator(data_ptr_), Iterator(data_ptr_ + num), std::forward<ArgsType>(args)...);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::Clear() noexcept {
    DestroyObjectsP(Iterator(data_ptr_), Iterator(data_ptr_ + size_));
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::Destroy() {
    DestroyContainerP();
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename... ArgsType>
requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CreateObjectP(DstIteratorType dst, ArgsType&&... args) {
    if constexpr (sizeof...(args) == 0) {
        if constexpr (kIfUnique) {
            new(reinterpret_cast<Void*>(dst.object_ptr())) ObjectType();
        }
    }
    else {
        new(reinterpret_cast<Void*>(dst.object_ptr())) ObjectType(std::forward<ArgsType>(args)...);
    }
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType>
requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::DestroyObjectP(DstIteratorType dst) {
    if constexpr (kIfUnique) {
        dst->~ObjectType();
    }
}

template<typename ObjectType, Bool kIfUnique>
template<Bool kIfCreate, typename DstIteratorType, typename... ArgsType>
requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
FORCEINLINE static Void ZVector<ObjectType, kIfUnique>::CreateDestroyObjectsBaseP(DstIteratorType dst_begin, 
                                                                                  DstIteratorType dst_end,
                                                                                  ArgsType&&... args) {
    if constexpr (sizeof...(args) != 0 || (!kIfCreate && kIfUnique)) {
        while (dst_begin < dst_end) {
            if constexpr (kIfCreate) {
                new(reinterpret_cast<Void*>(dst_begin.object_ptr())) ObjectType(std::forward<ArgsType>(args)...);
            }
            else {
                dst_begin->~ObjectType();
            }
            ++dst_begin;
        }
    }
    else if constexpr (kIfUnique){
        if constexpr (internal::kIsOrderVectorIterator<DstIteratorType, ObjectType>) {
            new(reinterpret_cast<Void*>(dst_begin.object_ptr())) ObjectType[dst_end - dst_begin];
        }
        else {
            new(reinterpret_cast<Void*>(dst_end.object_ptr() + 1)) ObjectType[dst_end - dst_begin];
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
template<Bool kIfCreate, typename DstIteratorType, typename SrcIteratorType>
requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>&&
          internal::kIsVectorIterator<SrcIteratorType, ObjectType>)
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::CopyObjectsBaseP(DstIteratorType dst, 
                                                                  SrcIteratorType src_begin, 
                                                                  SrcIteratorType src_end) {
    if constexpr (kIfUnique || (internal::kIsOrderVectorIterator<DstIteratorType, ObjectType> != 
                                internal::kIsOrderVectorIterator<SrcIteratorType, ObjectType>)) {
        while (src_begin < src_end) {
            if constexpr (kIfCreate) {
                new(reinterpret_cast<Void*>(dst.object_ptr())) ObjectType(*src_begin);
            }
            else {
                *dst = *src_begin;
            }
            ++dst;
            ++src_begin;
        }
    }
    else {
        if constexpr (internal::kIsOrderVectorIterator<DstIteratorType, ObjectType>) {
            memcpy(reinterpret_cast<Void*>(dst.object_ptr()), reinterpret_cast<Void*>(src_begin.object_ptr()),
                   static_cast<SizeType>(src_end - src_begin) * sizeof(ObjectType));
        }
        else {
            IndexType length = src_end - src_begin;
            memcpy(reinterpret_cast<Void*>(dst.object_ptr() - (length - 1)), 
                   reinterpret_cast<Void*>(src_end.object_ptr() + 1),
                   length * sizeof(ObjectType));
        }
    }
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::CreateContainerP(IndexType capacity) noexcept {
    MemoryType need_memory_size = capacity * sizeof(ObjectType);
    MemoryType apply_mrmory_size;
    data_ptr_ = reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory(need_memory_size, &apply_mrmory_size));
    capacity_ = apply_mrmory_size / sizeof(ObjectType);
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::ExtendContainerP(IndexType capacity) noexcept {
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
Void ZVector<ObjectType, kIfUnique>::ShrinkContainerP() noexcept {
    MemoryType need_memory_size = size_ * sizeof(ObjectType);
    MemoryType min_satisfied_memory_size = memory_pool::CalculateMemory(need_memory_size);
    MemoryType min_satisfied_capacity = min_satisfied_memory_size / sizeof(ObjectType);
    if (capacity_ != min_satisfied_capacity) {
        ObjectType* temp_data_ptr = reinterpret_cast<ObjectType*>(memory_pool::ApplyMemory(min_satisfied_memory_size));
        memcpy(reinterpret_cast<Void*>(temp_data_ptr), reinterpret_cast<Void*>(data_ptr_),
               size_ * sizeof(ObjectType));
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(data_ptr_));
        data_ptr_ = temp_data_ptr;
        capacity_ = min_satisfied_capacity;
    }
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::DestroyContainerP() noexcept {
    DestroyObjectsP(Iterator(data_ptr_), Iterator(data_ptr_ + size_));
    memory_pool::ReleaseMemory(reinterpret_cast<Void*>(data_ptr_));
    data_ptr_ = nullptr;
    capacity_ = 0;
    size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
Void ZVector<ObjectType, kIfUnique>::CopyP(const ZVector& vector) noexcept {
    if (vector.size_ > capacity_) {
        ExtendContainerP(vector.size_);
    }
    if constexpr (kIfUnique) {
        if (vector.size_ > size_) {
            CopyObjectsP(Iterator(data_ptr_), Iterator(vector.data_ptr_), Iterator(vector.data_ptr_ + size_));
            CreateAndCopyObjectsP(Iterator(data_ptr_ + size_), 
                                  Iterator(vector.data_ptr_ + size_), 
                                  Iterator(vector.data_ptr_ + vector.size_));
        }
        else {
            CopyObjectsP(Iterator(data_ptr_), Iterator(vector.data_ptr_), Iterator(vector.data_ptr_ + vector.size_));
            DestroyObjectsP(Iterator(data_ptr_ + vector.size_), Iterator(data_ptr_ + size_));
        }
    }
    else {
        CopyObjectsP(Iterator(data_ptr_), Iterator(vector.data_ptr_), Iterator(vector.data_ptr_ + vector.size_));
    }
    size_ = vector.size_;
}

template<typename ObjectType, Bool kIfUnique>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::MoveP(ZVector&& vector) {
    data_ptr_ = vector.data_ptr_;
    capacity_ = vector.capacity_;
    size_ = vector.size_;
    vector.data_ptr_ = nullptr;
    vector.capacity_ = 0;
    vector.size_ = 0;
}

template<typename ObjectType, Bool kIfUnique>
template<typename SrcIteratorType>
requires internal::kIsVectorIterator<SrcIteratorType, ObjectType>
FORCEINLINE Void ZVector<ObjectType, kIfUnique>::ZVectorP(SrcIteratorType src_begin, SrcIteratorType src_end) {
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    size_ = src_end - src_begin;
    CreateContainerP(size_);
    CreateAndCopyObjectsP(Iterator(data_ptr_), src_begin, src_end);
}

template<typename ObjectType, Bool kIfUnique>
template<typename SrcIteratorType>
requires internal::kIsVectorIterator<SrcIteratorType, ObjectType>
Void ZVector<ObjectType, kIfUnique>::PushBacksP(SrcIteratorType src_begin, SrcIteratorType src_end) noexcept {
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType new_size = size_ + (src_end - src_begin);
    if (new_size > capacity_) {
        if (static_cast<ObjectType*>(src_begin) >= data_ptr_ && 
                static_cast<ObjectType*>(src_begin) <= (data_ptr_ + size_)) {
            IndexType begin_index = static_cast<IndexType>(src_begin.object_ptr() - data_ptr_);
            IndexType end_index = static_cast<IndexType>(src_end.object_ptr() - data_ptr_);
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
            src_begin = data_ptr_ + begin_index;
            src_end = data_ptr_ + end_index;
        }
        else {
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
        }
    }
    CreateAndCopyObjectsP(Iterator(data_ptr_ + size_), src_begin, src_end);
    size_ = new_size;
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename... ArgsType>
requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
NODISCARD DstIteratorType ZVector<ObjectType, kIfUnique>::InsertP(DstIteratorType dst, ArgsType&&... args) noexcept {
    if constexpr (internal::kIsReverseVectorIterator<DstIteratorType, ObjectType>) {
        --dst;
    }
    DEBUG(dst.object_ptr() < data_ptr_ || dst.object_ptr() > data_ptr_ + size_, "Insert place out of bounds!");
    IndexType new_size = size_ + 1;
    if (new_size > capacity_) {
        IndexType index = static_cast<IndexType>(dst.object_ptr() - data_ptr_);
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
        dst = data_ptr_ + index;

    }
    memmove(reinterpret_cast<Void*>(dst.object_ptr() + 1), reinterpret_cast<Void*>(dst.object_ptr()),
            (size_ - static_cast<SizeType>(dst.object_ptr() - data_ptr_)) * sizeof(ObjectType));
    CreateObjectP(dst, std::forward<ArgsType>(args)...);
    size_ = new_size;
    return dst;
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename... ArgsType>
requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
NODISCARD DstIteratorType ZVector<ObjectType, kIfUnique>::InsertsP(DstIteratorType dst, IndexType num,
                                                                   ArgsType&&... args) noexcept {
    if constexpr (internal::kIsReverseVectorIterator<DstIteratorType, ObjectType>) {
        --dst;
    }
    DEBUG(dst.object_ptr() < data_ptr_ || dst.object_ptr() > data_ptr_ + size_, "Insert place out of bounds!");
    DEBUG(num < 0, "Negative insert num not valid!");
    IndexType new_size = size_ + num;
    if (new_size > capacity_) {
        IndexType index = static_cast<IndexType>(dst.object_ptr() - data_ptr_);
        ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
        dst = data_ptr_ + index;
    }
    memmove(reinterpret_cast<Void*>(dst.object_ptr() + num), reinterpret_cast<Void*>(dst.object_ptr()),
            (size_ - static_cast<SizeType>(dst.object_ptr() - data_ptr_)) * sizeof(ObjectType));
    CreateObjectsP(dst, dst + num, std::forward<ArgsType>(args)...);
    size_ = new_size;
    if constexpr (internal::kIsReverseVectorIterator<DstIteratorType, ObjectType>) {
        dst -= num - 1;
    }
    return dst;
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename SrcIteratorType>
requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType> &&
          internal::kIsVectorIterator<SrcIteratorType, ObjectType>)
NODISCARD DstIteratorType ZVector<ObjectType, kIfUnique>::InsertsP(DstIteratorType dst,
                                                                   SrcIteratorType src_begin, 
                                                                   SrcIteratorType src_end) noexcept {
    if constexpr (internal::kIsReverseVectorIterator<DstIteratorType, ObjectType>) {
        --dst;
    }
    DEBUG(dst.object_ptr() < data_ptr_ || dst.object_ptr() > data_ptr_ + size_, "Insert place out of bounds!");
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType num = src_end - src_begin;
    IndexType new_size = size_ + num;
    if (src_begin.object_ptr() >= data_ptr_ && src_begin.object_ptr() <= (data_ptr_ + size_)) {
        IndexType begin_index = static_cast<IndexType>(src_begin.object_ptr() - data_ptr_);
        IndexType end_index = static_cast<IndexType>(src_end.object_ptr() - data_ptr_);
        IndexType index = static_cast<IndexType>(dst.object_ptr() - data_ptr_);
        if (new_size > capacity_) {
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
            dst = data_ptr_ + index;
        }
        memmove(reinterpret_cast<Void*>(dst.object_ptr() + num), reinterpret_cast<Void*>(dst.object_ptr()),
                (size_ - static_cast<SizeType>(dst.object_ptr() - data_ptr_)) * sizeof(ObjectType));
        if (index >= end_index) {
            src_begin = data_ptr_ + begin_index;
            src_end = data_ptr_ + end_index;
            CreateAndCopyObjectsP(dst, src_begin, src_end);
        }
        else if (index < begin_index) {
            src_begin = data_ptr_ + begin_index + num;
            src_end = data_ptr_ + end_index + num;
            CreateAndCopyObjectsP(dst, src_begin, src_end);
        }
        else {
            IndexType part_1_num = index - begin_index;
            IndexType part_2_num = num - part_1_num;
            src_begin = data_ptr_ + begin_index;
            src_end = src_begin + part_1_num;
            CreateAndCopyObjectsP(dst, src_begin, src_end);
            src_begin = dst + num;
            src_end = src_begin + part_2_num;
            CreateAndCopyObjectsP(dst + part_1_num, src_begin, src_end);
        }
    }
    else {
        if (new_size > capacity_) {
            ExtendContainerP(static_cast<IndexType>(static_cast<Float32>(new_size) * kAutoExtendMulFactor));
        }
        memmove(reinterpret_cast<Void*>(dst.object_ptr() + num), reinterpret_cast<Void*>(dst.object_ptr()),
                (size_ - static_cast<SizeType>(dst.object_ptr() - data_ptr_)) * sizeof(ObjectType));
        CreateAndCopyObjectsP(dst, src_begin, src_end);
    }
    size_ = new_size;
    if constexpr (internal::kIsOrderVectorIterator<DstIteratorType, ObjectType>) {
        dst -= num - 1;
    }
    return dst;
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename... ArgsType>
requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
NODISCARD inline DstIteratorType ZVector<ObjectType, kIfUnique>::EraseP(DstIteratorType dst) noexcept {
    DEBUG(dst.object_ptr() < data_ptr_ || dst.object_ptr() >= data_ptr_ + size_, "Erase place out of bounds!");
    DestroyObjectP(dst);
    memmove(reinterpret_cast<Void*>(dst.object_ptr()), reinterpret_cast<Void*>(dst.object_ptr() + 1),
            (size_ - 1 - static_cast<SizeType>(dst.object_ptr() - data_ptr_)) * sizeof(ObjectType));
    --size_;
    if constexpr (internal::kIsOrderVectorIterator<DstIteratorType, ObjectType>) {
        --dst;
    }
    return dst;
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename... ArgsType>
requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
NODISCARD inline DstIteratorType ZVector<ObjectType, kIfUnique>::ErasesP(DstIteratorType dst_begin, 
                                                                         DstIteratorType dst_end) noexcept {
    DEBUG(dst_begin.object_ptr() < data_ptr_ || dst_begin.object_ptr() >= data_ptr_ + size_, 
          "Erase index out of bounds!");
    DEBUG(dst_end.object_ptr() < data_ptr_ || dst_end.object_ptr() > data_ptr_ + size_, "Erase index out of bounds!");
    DEBUG(dst_begin > dst_end, "Begin iterator after end iterator!");
    DestroyObjectsP(dst_begin, dst_end);
    memmove(reinterpret_cast<Void*>(dst_begin.object_ptr()), reinterpret_cast<Void*>(dst_end.object_ptr()),
        (size_ - static_cast<SizeType>(dst_end.object_ptr() - data_ptr_)) * sizeof(ObjectType));
    size_ -= dst_end - dst_begin;
    if constexpr (internal::kIsOrderVectorIterator<DstIteratorType, ObjectType>) {
        return dst_begin - 1;
    }
    else {
        return dst_end - 1;
    }
}

template<typename ObjectType, Bool kIfUnique>
template<typename DstIteratorType, typename... ArgsType>
requires (!internal::kIsConstVectorIterator<DstIteratorType, ObjectType>)
inline Void ZVector<ObjectType, kIfUnique>::EmplaceP(DstIteratorType dst, ArgsType&&... args) noexcept {
    DEBUG(dst.object_ptr() < data_ptr_ || dst.object_ptr() >= data_ptr_ + size_, "Emplace place out of bounds!");
    DestroyObjectP(dst);
    CreateObjectP(dst, std::forward<ArgsType>(args)...);
}

template<typename ObjectType, Bool kIfUnique>
template<typename SrcIteratorType, typename... ArgsType>
requires (!internal::kIsConstVectorIterator<SrcIteratorType, ObjectType>)
Void ZVector<ObjectType, kIfUnique>::AssignP(SrcIteratorType src_begin, SrcIteratorType src_end) noexcept {
    DEBUG(src_begin > src_end, "Begin iterator after end iterator!");
    IndexType new_size = src_end - src_begin;
    if (new_size > capacity_) {
        ExtendContainerP(new_size);
    }
    if constexpr (kIfUnique) {
        //If the iterator is from this.
        if (src_begin.object_ptr() >= data_ptr_ && src_begin.object_ptr() < (data_ptr_ + size_)) {
            if constexpr (internal::kIsOrderVectorIterator<SrcIteratorType, ObjectType>) {
                DestroyObjectsP(SrcIteratorType(data_ptr_), src_begin);
                DestroyObjectsP(src_end, SrcIteratorType(data_ptr_ + size_));
                //Move the objects to the front.
                memmove(reinterpret_cast<Void*>(data_ptr_), reinterpret_cast<Void*>(src_begin.object_ptr()),
                        new_size * sizeof(ObjectType));
            }
            else {
                DestroyObjectsP(SrcIteratorType(data_ptr_ + (size_ - 1)), src_begin);
                DestroyObjectsP(src_end, SrcIteratorType(data_ptr_ - 1));
                //Move the objects to the front.
                memmove(reinterpret_cast<Void*>(data_ptr_), reinterpret_cast<Void*>(src_end.object_ptr() + 1),
                        new_size * sizeof(ObjectType));
                //Reverses the objects.
                ObjectType* begin_ptr = data_ptr_;
                ObjectType* end_ptr = data_ptr_ + (new_size - 1);
                while (begin_ptr < end_ptr) {
                    Swap(begin_ptr, end_ptr);
                    ++begin_ptr;
                    --end_ptr;
                }
            }
        }
        else {
            if (new_size > size_) {
                CopyObjectsP(Iterator(data_ptr_), src_begin, src_begin + size_);
                CreateAndCopyObjectsP(Iterator(data_ptr_ + size_), src_begin + size_, src_end);
            }
            else {
                Iterator temp(data_ptr_);
                CopyObjectsP(Iterator(data_ptr_), src_begin, src_end);
                DestroyObjectsP(Iterator(data_ptr_ + new_size), Iterator(data_ptr_ + size_));
            }
        }
    }
    else {
        CopyObjectsP(Iterator(data_ptr_), src_begin, src_end);
    }
    size_ = new_size;
}

}//zengine

#endif // !Z_CORE_Z_VECTOR_H_