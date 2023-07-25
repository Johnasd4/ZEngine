#ifndef Z_CORE_Z_ARRAY_H_
#define Z_CORE_Z_ARRAY_H_

#include "internal/drive.h"

#include "m_error_message.h"
#include "z_object.h"

namespace zengine {

namespace internal {

template<typename ObjectType>
class ArrayIteratorBase {
public:
    FORCEINLINE ArrayIteratorBase(ObjectType* object_ptr) : object_ptr_(object_ptr) {}
    FORCEINLINE ArrayIteratorBase(const ArrayIteratorBase& iterator) : object_ptr_(iterator.object_ptr_) {}
    FORCEINLINE ArrayIteratorBase(ArrayIteratorBase&& iterator) : object_ptr_(iterator.object_ptr_) {
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

    NODISCARD FORCEINLINE Bool operator==(const ArrayIteratorBase& iterator) const {
        return object_ptr_ == iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const ArrayIteratorBase& iterator) const {
        return object_ptr_ != iterator.object_ptr_;
    }

    NODISCARD FORCEINLINE ObjectType& operator*() const { return *object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* operator->() const { return object_ptr_; }

    FORCEINLINE ~ArrayIteratorBase() {}

    NODISCARD FORCEINLINE ObjectType* object_ptr() const { return object_ptr_; }

    FORCEINLINE Void MoveDestroy() { object_ptr_ = nullptr; }

protected:
     ObjectType* object_ptr_;
};

template<typename ObjectType>
class ArrayIterator : public ArrayIterator<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) const { return SuperType::object_ptr_[index]; }

    FORCEINLINE ArrayIterator& operator+=(IndexType num) {
        SuperType::object_ptr_ += num;
        return *this;
    }
    FORCEINLINE ArrayIterator& operator-=(IndexType num) {
        SuperType::object_ptr_ -= num;
        return *this;
    }

    FORCEINLINE ArrayIterator& operator++() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ArrayIterator& operator++(IndexType) {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ArrayIterator& operator--() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ArrayIterator& operator--(IndexType) {
        --SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE ArrayIterator operator+(IndexType num) const {
        return ArrayIterator(SuperType::object_ptr_ + num);
    }
    NODISCARD FORCEINLINE ArrayIterator operator-(IndexType num) const {
        return ArrayIterator(SuperType::object_ptr_ - num);
    }

    NODISCARD FORCEINLINE Bool operator>(const ArrayIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator>=(const ArrayIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator<(const ArrayIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator<=(const ArrayIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE IndexType operator-(const ArrayIterator& iterator) const {
        return static_cast<IndexType>(SuperType::object_ptr_ - iterator.SuperType::object_ptr_);
    }

protected:
    using SuperType = ArrayIteratorBase<ObjectType>;
};

template<typename ObjectType>
class ArrayReverseIterator : public ArrayIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) const { return SuperType::object_ptr_[-index]; }

    FORCEINLINE ArrayReverseIterator& operator+=(IndexType num) {
        SuperType::object_ptr_ -= num;
        return *this;
    }
    FORCEINLINE ArrayReverseIterator& operator-=(IndexType num) {
        SuperType::object_ptr_ += num;
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

    NODISCARD FORCEINLINE ArrayReverseIterator operator+(const IndexType num) const {
        return ArrayReverseIterator(SuperType::object_ptr_ - num);
    }
    NODISCARD FORCEINLINE ArrayReverseIterator operator-(const IndexType num) const {
        return ArrayReverseIterator(SuperType::object_ptr_ + num);
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

    FORCEINLINE IndexType operator-(const ArrayReverseIterator& iterator) const {
        return static_cast<IndexType>(iterator.SuperType::object_ptr_ - SuperType::object_ptr_);
    }



protected:
    using SuperType = ArrayIteratorBase<ObjectType>;
};

template<typename ArrayType, typename Function, typename... ArgsType>
concept kIsArrayInitFunction = requires(ArrayType* array, Function function, ArgsType&&... args) {
    function(array, std::forward<ArgsType>(args)...);
};

}//internal




/*
    The size of the array is fixed. It's stored in the stack memory, use new
    to store it in the heap memory. This class uses no extra size, doesn't
    have any other members except the objects.
    If kIfUnique is true, will call the constuctor when the object is created.
*/
template<typename ObjectType, IndexType kCapacity, Bool kIfUnique = kIsClass<ObjectType>>
requires kIsNotZero<kCapacity>
class ZArray : public ZObject {
public:
    using IteratorType = internal::ArrayIterator<ObjectType>;
    using ConstIteratorType = internal::ArrayIterator<const ObjectType>;
    using ReverseIteratorType = internal::ArrayReverseIterator<ObjectType>;
    using ConstReverseIteratorType = internal::ArrayReverseIterator<const ObjectType>;

#pragma warning(disable : 26495)
    FORCEINLINE ZArray() : SuperType() {}
#pragma warning(default : 26495)
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
                (*array_ptr)[index] = 1;
            }
        };
        constexpr ZArray<Int32, 10> test(init_function);
    */
    template<typename InitFunction, typename... ArgsType>
    requires internal::kIsArrayInitFunction<ZArray<ObjectType, kCapacity, kIfUnique>, 
                                            InitFunction, ArgsType...>
    FORCEINLINE constexpr ZArray(InitFunction&& init_function, ArgsType&&... args) : SuperType() {
        init_function(this, std::forward<ArgsType>(args)...);
    }
    inline ZArray(const ZArray& array) noexcept;
    inline ZArray(ZArray&& array) noexcept;

    inline ZArray& operator=(const ZArray& array) noexcept;
    inline ZArray& operator=(ZArray&& array) noexcept;

    NODISCARD FORCEINLINE constexpr ObjectType& operator[](IndexType index) { 
        DEBUG(index < 0 || index >= kCapacity, "Index out of bounds!");
        return this->data_[index]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& operator[](IndexType index) const {
        DEBUG(index < 0 || index >= kCapacity, "Index out of bounds!");
        return this->data_[index]; 
    }

    NODISCARD FORCEINLINE constexpr ObjectType& At(IndexType index) {
        DEBUG(index < 0 || index >= kCapacity, "Index out of bounds!");
        return data_[index];
    }
    NODISCARD FORCEINLINE constexpr const ObjectType& At(IndexType index) const {
        DEBUG(index < 0 || index >= kCapacity, "Index out of bounds!");
        return data_[index];
    }
    NODISCARD FORCEINLINE constexpr ObjectType& Front() { return data_[0]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& Front() const { return data_[0]; }
    NODISCARD FORCEINLINE constexpr ObjectType& Back() { return data_[kCapacity - 1]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& Back() const { return data_[kCapacity - 1]; }

    NODISCARD FORCEINLINE constexpr const ObjectType* data_ptr() const { return data_; }

    NODISCARD static constexpr IndexType size() { return kCapacity; }

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



    template<typename... ArgsType>
    Void Fill(ArgsType&&... args) noexcept;

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
    Void Sort(const IteratorType& begin, const IteratorType& end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
        requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const IteratorType& begin, const IteratorType& end, CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(const ReverseIteratorType& begin, const ReverseIteratorType& end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
        requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const ReverseIteratorType& begin, const ReverseIteratorType& end, 
              CompareFunction&& compare_function) noexcept;

protected:
    using SuperType = ZObject;

private:
    ObjectType data_[kCapacity];
};

template<typename ObjectType, IndexType kCapacity, Bool kIfUnique>
requires kIsNotZero<kCapacity>
inline ZArray<ObjectType, kCapacity, kIfUnique>::ZArray(const ZArray& array) noexcept
        : SuperType(array) {
    if constexpr (kIfUnique) {
        for (IndexType index = 0; index < kCapacity; ++index) {
            data_[index] = array[index];
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), 
            reinterpret_cast<Void*>(const_cast<ObjectType*>(array.data_)),
               sizeof(ZArray));
    }
}

template<typename ObjectType, IndexType kCapacity, Bool kIfUnique>
requires kIsNotZero<kCapacity>
inline ZArray<ObjectType, kCapacity, kIfUnique>::ZArray(ZArray&& array) noexcept
        : SuperType(std::forward<ZArray>(array)) {
    if constexpr (kIfUnique) {
        for (IndexType index = 0; index < kCapacity; ++index) {
            data_[index] = array[index];
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZArray));
    }
}

template<typename ObjectType, IndexType kCapacity, Bool kIfUnique>
requires kIsNotZero<kCapacity>
inline ZArray<ObjectType, kCapacity, kIfUnique>& ZArray<ObjectType, kCapacity, kIfUnique>::operator=(
        const ZArray& array) noexcept {
    if constexpr (kIfUnique) {
        for (IndexType index = 0; index < kCapacity; ++index) {
            data_[index] = array[index];
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZArray));
    }
    return *this;
}

template<typename ObjectType, IndexType kCapacity, Bool kIfUnique>
requires kIsNotZero<kCapacity>
inline ZArray<ObjectType, kCapacity, kIfUnique>& ZArray<ObjectType, kCapacity, kIfUnique>::operator=(
        ZArray&& array) noexcept {
    if constexpr (kIfUnique) {
        for (IndexType index = 0; index < kCapacity; ++index) {
            data_[index] = array[index];
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZArray));
    }
    return *this;
}

template<typename ObjectType, IndexType kCapacity, Bool kIfUnique>
requires kIsNotZero<kCapacity>
template<typename... ArgsType>
Void ZArray<ObjectType, kCapacity, kIfUnique>::Fill(ArgsType&&... args) noexcept {
    if constexpr (kIfUnique) {
        ObjectType* end_ptr = data_ + kCapacity;
        for (ObjectType* object_ptr = data_; object_ptr < end_ptr; ++object_ptr) {
            object_ptr->~ObjectType();
        }
        if constexpr (sizeof...(args) == 0) {
            new(reinterpret_cast<Void*>(data_)) ObjectType[kCapacity];
        }
        else {
            for (ObjectType* object_ptr = data_; object_ptr < end_ptr; ++object_ptr) {
                new(reinterpret_cast<Void*>(object_ptr)) ObjectType(std::forward<ArgsType>(args)...);
            }
        }
    }
    else {
        if constexpr (sizeof...(args) != 0) {
            ObjectType* end_ptr = data_ + kCapacity;
            for (ObjectType* object_ptr = data_; object_ptr < end_ptr; ++object_ptr) {
                new(reinterpret_cast<Void*>(object_ptr)) ObjectType(std::forward<ArgsType>(args)...);
            }
        }
    }
}

}//zengine

#endif // !Z_CORE_Z_ARRAY_H_