#ifndef Z_CORE_Z_ARRAY_H_
#define Z_CORE_Z_ARRAY_H_

#include "internal/drive.h"

#include "z_object.h"

namespace zengine {

namespace internal {

template<typename ObjectType>
class ArrayIteratorBase {
public:
    FORCEINLINE ArrayIteratorBase(ObjectType* object_ptr) : object_ptr_(object_ptr) {}
    FORCEINLINE ArrayIteratorBase(const ArrayIteratorBase& iterator) : object_ptr_(iterator.object_ptr_) {}
    FORCEINLINE ArrayIteratorBase(const ArrayIteratorBase&& iterator) : object_ptr_(iterator.object_ptr_) {
        iterator.MoveDestroy();
    }

    FORCEINLINE ArrayIteratorBase& operator=(ObjectType* object_ptr) {
        object_ptr_ = object_ptr;
        return *this;
    }
    FORCEINLINE ArrayIteratorBase& operator=(const ArrayIteratorBase& iterator) {
        object_ptr_ = iterator.object_ptr_;
        return *this;
    }
    FORCEINLINE ArrayIteratorBase& operator=(ArrayIteratorBase&& iterator) {
        object_ptr_ = iterator.object_ptr_;
        iterator.MoveDestroy();
        return *this;
    }

    NODISCARD FORCEINLINE const Bool operator==(const ArrayIteratorBase& iterator) const {
        return object_ptr_ == iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator!=(const ArrayIteratorBase& iterator) const {
        return object_ptr_ != iterator.object_ptr_;
    }

    FORCEINLINE ~ArrayIteratorBase() {}

    FORCEINLINE Void MoveDestroy() { object_ptr_ = nullptr; }

protected:
     ObjectType* object_ptr_;
};

template<typename ObjectType>
class ArrayIterator : public ArrayIterator<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index) const { return SuperType::object_ptr_[index]; }
    NODISCARD FORCEINLINE ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE ArrayIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE ArrayIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }

    FORCEINLINE ArrayIterator& operator++() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ArrayIterator& operator--() {
        --SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE ArrayIterator operator+(const IndexType data_num) const {
        return ArrayIterator(SuperType::object_ptr_ + data_num);
    }
    NODISCARD FORCEINLINE ArrayIterator operator-(const IndexType data_num) const {
        return ArrayIterator(SuperType::object_ptr_ - data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const ArrayIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const ArrayIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const ArrayIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const ArrayIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const ArrayIterator& iterator) const {
        return static_cast<IndexType>(SuperType::object_ptr_ - iterator.SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = ArrayIteratorBase<ObjectType>;
};

template<typename ObjectType>
class ArrayConstIterator : public ArrayIteratorBase<ObjectType> {
public:
    FORCEINLINE ArrayConstIterator(const ObjectType* object_ptr) : SuperType(const_cast<ObjectType*>(object_ptr)) {}

    FORCEINLINE ArrayConstIterator& operator=(const ObjectType* object_ptr) {
        SuperType::object_ptr_ = const_cast<ObjectType*>(object_ptr);
        return *this;
    }

    NODISCARD FORCEINLINE const ObjectType& operator[](const IndexType index) const { 
        return SuperType::object_ptr_[index]; 
    }
    NODISCARD FORCEINLINE const ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE ArrayConstIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }
    FORCEINLINE ArrayConstIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }

    FORCEINLINE ArrayConstIterator& operator++() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ArrayConstIterator& operator--() {
        --SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE ArrayConstIterator operator+(const IndexType data_num) const {
        return ArrayConstIterator(SuperType::object_ptr_ + data_num);
    }
    NODISCARD FORCEINLINE ArrayConstIterator operator-(const IndexType data_num) const {
        return ArrayConstIterator(SuperType::object_ptr_ - data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const ArrayConstIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const ArrayConstIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const ArrayConstIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const ArrayConstIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const ArrayConstIterator& iterator) const {
        return static_cast<IndexType>(SuperType::object_ptr_ - iterator.SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE const ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = ArrayIteratorBase<ObjectType>;
};

template<typename ObjectType>
class ArrayReverseIterator : public ArrayIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](const IndexType index) const { return SuperType::object_ptr_[-index]; }
    NODISCARD FORCEINLINE ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE ArrayReverseIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE ArrayReverseIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }

    FORCEINLINE ArrayReverseIterator& operator++() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ArrayReverseIterator& operator--() {
        ++SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE ArrayReverseIterator operator+(const IndexType data_num) const {
        return ArrayReverseIterator(SuperType::object_ptr_ - data_num);
    }
    NODISCARD FORCEINLINE ArrayReverseIterator operator-(const IndexType data_num) const {
        return ArrayReverseIterator(SuperType::object_ptr_ + data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const ArrayReverseIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const ArrayReverseIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const ArrayReverseIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const ArrayReverseIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const ArrayReverseIterator& iterator) const {
        return static_cast<IndexType>(iterator.SuperType::object_ptr_ - SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = ArrayIteratorBase<ObjectType>;
};

template<typename ObjectType>
class ArrayConstReverseIterator : public ArrayIteratorBase<ObjectType> {
public:
    FORCEINLINE ArrayConstReverseIterator(const ObjectType* object_ptr) 
        : SuperType(const_cast<ObjectType*>(object_ptr)) {}

    FORCEINLINE ArrayConstReverseIterator& operator=(const ObjectType* object_ptr) {
        SuperType::object_ptr_ = const_cast<ObjectType*>(object_ptr);
        return *this;
    }

    NODISCARD FORCEINLINE const ObjectType& operator[](const IndexType index) const {
        return SuperType::object_ptr_[-index];
    }
    NODISCARD FORCEINLINE const ObjectType& operator*() const { return *SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const operator->() const { return SuperType::object_ptr_; }

    FORCEINLINE ArrayConstReverseIterator& operator+=(const IndexType data_num) {
        SuperType::object_ptr_ -= data_num;
        return *this;
    }
    FORCEINLINE ArrayConstReverseIterator& operator-=(const IndexType data_num) {
        SuperType::object_ptr_ += data_num;
        return *this;
    }

    FORCEINLINE ArrayConstReverseIterator& operator++() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ArrayConstReverseIterator& operator--() {
        ++SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE ArrayConstReverseIterator operator+(const IndexType data_num) const {
        return ArrayConstReverseIterator(SuperType::object_ptr_ - data_num);
    }
    NODISCARD FORCEINLINE ArrayConstReverseIterator operator-(const IndexType data_num) const {
        return ArrayConstReverseIterator(SuperType::object_ptr_ + data_num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const ArrayConstReverseIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const ArrayConstReverseIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const ArrayConstReverseIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const ArrayConstReverseIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE const IndexType operator-(const ArrayConstReverseIterator& iterator) const {
        return static_cast<IndexType>(iterator.SuperType::object_ptr_ - SuperType::object_ptr_);
    }

    NODISCARD FORCEINLINE const ObjectType* object_ptr() { return SuperType::object_ptr_; }
    NODISCARD FORCEINLINE const ObjectType* const object_ptr() const { return SuperType::object_ptr_; }

protected:
    using SuperType = ArrayIteratorBase<ObjectType>;
};

}//internal




/*
    The size of the array is fixed. It's stored in the stack memory, use new
    to store it in the heap memory. This class uses no extra size, doesn't
    have any other members except the objects.
*/
template<typename ObjectType, IndexType kCapacity, Bool kIfInitializeObject = kIsClass<ObjectType>>
class ZArray : public ZObject {
public:
    using IteratorType = internal::ArrayIterator<ObjectType>;
    using ConstIteratorType = internal::ArrayConstIterator<ObjectType>;
    using ReverseIteratorType = internal::ArrayReverseIterator<ObjectType>;
    using ConstReverseIteratorType = internal::ArrayConstReverseIterator<ObjectType>;

#pragma warning(disable : 26495)
    FORCEINLINE ZArray() : SuperType() {}
#pragma warning(default : 26495)
    ZArray(const ZArray& array) noexcept;
    ZArray(ZArray&& array) noexcept;

    /*
        Constexpr array, the work is done at compile time.
        The Constructor's fisrt parameter is the initial funtion of the array. The
        first parameter of the initial funtion must be ZArray*.
        Constructor Template Parameters:
        - InitFunction: The function type that initial the array.
        - ArgsType...: The parameters type.
        Constructor Parameters:
        - init_function: The function to initial the array.
        - ArgsType...: The parameters of the function except for the fisrt.
        Example:
        constexpr auto init_function = [](ZArray<Int32, 10>* array_ptr) {
            for (IndexType index = 0; index < array_ptr->size(); ++index) {
                (*array_ptr)(index) = 1;
            }
        };
        constexpr ZArray<Int32, 10> test(init_function);
    */
    template<typename InitFunction, typename... ArgsType>
    FORCEINLINE constexpr ZArray(InitFunction&& init_function, ArgsType&&... args) :SuperType() {
        init_function(this, std::forward<ArgsType>(args)...);
    }

    ZArray& operator=(const ZArray& array) noexcept;
    ZArray& operator=(ZArray&& array) noexcept;

    NODISCARD FORCEINLINE constexpr ObjectType& operator[](const IndexType index) { return this->data_[index]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& operator[](const IndexType index) const {
        return this->data_[index]; 
    }

    /*
        The iterator funcions.
    */
    NODISCARD FORCEINLINE IteratorType Begin() { return IteratorType(data_); }
    NODISCARD FORCEINLINE ConstIteratorType ConstBegin() const { return ConstIteratorType(data_); }
    NODISCARD FORCEINLINE ReverseIteratorType ReverseBegin() { return ReverseIteratorType(data_ + kCapacity - 1); }
    NODISCARD FORCEINLINE ConstReverseIteratorType ConstReverseBegin() const {
        return ConstReverseIteratorType(data_ + kCapacity - 1);
    }
    NODISCARD FORCEINLINE IteratorType End() { return IteratorType(data_ + kCapacity); }
    NODISCARD FORCEINLINE ConstIteratorType ConstEnd() const { return ConstIteratorType(data_ + kCapacity); }
    NODISCARD FORCEINLINE ReverseIteratorType ReverseEnd() { return ReverseIteratorType(data_ - 1); }
    NODISCARD FORCEINLINE ConstReverseIteratorType ConstReverseEnd() const {
        return ConstReverseIteratorType(data_ - 1);
    }

    NODISCARD FORCEINLINE static constexpr const IndexType size() { return kCapacity; }
    NODISCARD FORCEINLINE constexpr const ObjectType* data_ptr() const { return data_; }

    NODISCARD FORCEINLINE constexpr ObjectType& At(const IndexType index) { return data_[index]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& At(const IndexType index) const { return data_[index]; }
    NODISCARD FORCEINLINE constexpr ObjectType& Front() { return data_[0]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& Front() const { return data_[0]; }
    NODISCARD FORCEINLINE constexpr ObjectType& Back() { return data_[kCapacity - 1]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& Back() const { return data_[kCapacity - 1]; }

    template<typename... ArgsType>
    constexpr Void Fill(ArgsType&&... args) noexcept;

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
    Void Sort(const IndexType begin, const IndexType end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
        requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const IndexType begin, const IndexType end, CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(const IteratorType begin, const IteratorType end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
        requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const IteratorType begin, const IteratorType end, CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(const ReverseIteratorType begin, const ReverseIteratorType end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
        requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const ReverseIteratorType begin, const ReverseIteratorType end, 
              CompareFunction&& compare_function) noexcept;

protected:
    using SuperType = ZObject;

private:
    ObjectType data_[kCapacity];
};

template<typename ObjectType, IndexType kCapacity, Bool kIfInitializeObject>
ZArray<ObjectType, kCapacity, kIfInitializeObject>::ZArray(const ZArray& array) noexcept
        : SuperType() {
    if constexpr (kIfInitializeObject) {
        for (IndexType index = 0; index < array.size(); ++index) {
            data_[index] = array[index];
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZArray));
    }
}

template<typename ObjectType, IndexType kCapacity, Bool kIfInitializeObject>
ZArray<ObjectType, kCapacity, kIfInitializeObject>::ZArray(ZArray&& array) noexcept
        : SuperType() {
    if constexpr (kIfInitializeObject) {
        for (IndexType index = 0; index < array.size(); ++index) {
            data_[index] = array[index];
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZArray));
    }
}

template<typename ObjectType, IndexType kCapacity, Bool kIfInitializeObject>
ZArray<ObjectType, kCapacity, kIfInitializeObject>& ZArray<ObjectType, kCapacity, kIfInitializeObject>::operator=(
        const ZArray& array) noexcept {
    if constexpr (kIfInitializeObject) {
        for (IndexType index = 0; index < array.size(); ++index) {
            data_[index] = array[index];
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZArray));
    }
    return *this;
}

template<typename ObjectType, IndexType kCapacity, Bool kIfInitializeObject>
ZArray<ObjectType, kCapacity, kIfInitializeObject>& ZArray<ObjectType, kCapacity, kIfInitializeObject>::operator=(
        ZArray&& array) noexcept {
    if constexpr (kIfInitializeObject) {
        for (IndexType index = 0; index < array.size(); ++index) {
            data_[index] = array[index];
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZArray));
    }
    return *this;
}

template<typename ObjectType, IndexType kCapacity, Bool kIfInitializeObject>
template<typename... ArgsType>
constexpr Void ZArray<ObjectType, kCapacity, kIfInitializeObject>::Fill(ArgsType&&... args) noexcept {
    if constexpr (kIfInitializeObject) {
        for (IndexType index = 0; index < array.size(); ++index) {
            data_[index] = ObjectType(std::forward<ArgsType>(args)...);
        }
        if constexpr (sizeof...(args) == 0) {
            new(reinterpret_cast<Void*>(data_)) ObjectType[array.size()];
        }
        else {
            for (IndexType index = 0; index < array.size(); ++index) {
                new(reinterpret_cast<Void*>(&data_[index])) ObjectType(std::forward<ArgsType>(args)...);
            }
        }
    }
    else {
        for (IndexType index = 0; index < array.size(); ++index) {
            data_[index] = ObjectType(std::forward<ArgsType>(args)...);
        }
    }
}

}//zengine

#endif // !Z_CORE_Z_ARRAY_H_