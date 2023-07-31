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
#ifndef Z_CORE_Z_ARRAY_H_
#define Z_CORE_Z_ARRAY_H_

#include "internal/drive.h"

#include "m_error_message.h"
#include "z_object.h"

namespace zengine {

namespace internal {

template<typename ObjectType>
class ZArrayIteratorBase {
public:
    FORCEINLINE ZArrayIteratorBase(ObjectType* object_ptr) : object_ptr_(object_ptr) {}
    FORCEINLINE ZArrayIteratorBase(const ZArrayIteratorBase& iterator) : object_ptr_(iterator.object_ptr_) {}
    FORCEINLINE ZArrayIteratorBase(ZArrayIteratorBase&& iterator) {
        MoveP(std::forward<ZArrayIteratorBase>(iterator));
    }

    FORCEINLINE ZArrayIteratorBase& operator=(ObjectType* object_ptr) {
        object_ptr_ = object_ptr;
        return *this;
    }
    FORCEINLINE ZArrayIteratorBase& operator=(const ZArrayIteratorBase& iterator) {
        object_ptr_ = iterator.object_ptr_;
        return *this;
    }
    FORCEINLINE ZArrayIteratorBase& operator=(ZArrayIteratorBase&& iterator) {
        MoveP(std::forward<ZArrayIteratorBase>(iterator));
        return *this;
    }

    NODISCARD FORCEINLINE Bool operator==(const ZArrayIteratorBase& iterator) const {
        return object_ptr_ == iterator.object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator!=(const ZArrayIteratorBase& iterator) const {
        return object_ptr_ != iterator.object_ptr_;
    }

    NODISCARD FORCEINLINE ObjectType& operator*() const { return *object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* operator->() const { return object_ptr_; }

    FORCEINLINE ~ZArrayIteratorBase() {}

    NODISCARD FORCEINLINE ObjectType& object() const { return *object_ptr_; }
    NODISCARD FORCEINLINE ObjectType* object_ptr() const { return object_ptr_; }

protected:
     ObjectType* object_ptr_;

private:
    FORCEINLINE Void MoveP(ZArrayIteratorBase&& iterator) {
        object_ptr_ = iterator.object_ptr_;
        iterator.object_ptr_ = nullptr;
    }
};

template<typename ObjectType>
class ZArrayIterator : public ZArrayIterator<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) const { return SuperType::object_ptr_[index]; }

    FORCEINLINE ZArrayIterator& operator+=(IndexType num) {
        SuperType::object_ptr_ += num;
        return *this;
    }
    FORCEINLINE ZArrayIterator& operator-=(IndexType num) {
        SuperType::object_ptr_ -= num;
        return *this;
    }

    FORCEINLINE ZArrayIterator& operator++() {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZArrayIterator& operator++(IndexType) {
        ++SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZArrayIterator& operator--() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZArrayIterator& operator--(IndexType) {
        --SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE ZArrayIterator operator+(IndexType num) const {
        return ZArrayIterator(SuperType::object_ptr_ + num);
    }
    NODISCARD FORCEINLINE ZArrayIterator operator-(IndexType num) const {
        return ZArrayIterator(SuperType::object_ptr_ - num);
    }

    NODISCARD FORCEINLINE Bool operator>(const ZArrayIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator>=(const ZArrayIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator<(const ZArrayIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE Bool operator<=(const ZArrayIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE IndexType operator-(const ZArrayIterator& iterator) const {
        return static_cast<IndexType>(SuperType::object_ptr_ - iterator.SuperType::object_ptr_);
    }

protected:
    using SuperType = ZArrayIteratorBase<ObjectType>;
};

template<typename ObjectType>
class ZArrayReverseIterator : public ZArrayIteratorBase<ObjectType> {
public:
    NODISCARD FORCEINLINE ObjectType& operator[](IndexType index) const { return SuperType::object_ptr_[-index]; }

    FORCEINLINE ZArrayReverseIterator& operator+=(IndexType num) {
        SuperType::object_ptr_ -= num;
        return *this;
    }
    FORCEINLINE ZArrayReverseIterator& operator-=(IndexType num) {
        SuperType::object_ptr_ += num;
        return *this;
    }

    FORCEINLINE ZArrayReverseIterator& operator++() {
        --SuperType::object_ptr_;
        return *this;
    }
    FORCEINLINE ZArrayReverseIterator& operator--() {
        ++SuperType::object_ptr_;
        return *this;
    }

    NODISCARD FORCEINLINE ZArrayReverseIterator operator+(IndexType num) const {
        return ZArrayReverseIterator(SuperType::object_ptr_ - num);
    }
    NODISCARD FORCEINLINE ZArrayReverseIterator operator-(IndexType num) const {
        return ZArrayReverseIterator(SuperType::object_ptr_ + num);
    }

    NODISCARD FORCEINLINE const Bool operator>(const ZArrayReverseIterator& iterator) const {
        return SuperType::object_ptr_ < iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator>=(const ZArrayReverseIterator& iterator) const {
        return SuperType::object_ptr_ <= iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<(const ZArrayReverseIterator& iterator) const {
        return SuperType::object_ptr_ > iterator.SuperType::object_ptr_;
    }
    NODISCARD FORCEINLINE const Bool operator<=(const ZArrayReverseIterator& iterator) const {
        return SuperType::object_ptr_ >= iterator.SuperType::object_ptr_;
    }

    FORCEINLINE IndexType operator-(const ZArrayReverseIterator& iterator) const {
        return static_cast<IndexType>(iterator.SuperType::object_ptr_ - SuperType::object_ptr_);
    }



protected:
    using SuperType = ZArrayIteratorBase<ObjectType>;
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
    using Iterator = internal::ZArrayIterator<ObjectType>;
    using ConstIterator = internal::ZArrayIterator<const ObjectType>;
    using ReverseIterator = internal::ZArrayReverseIterator<ObjectType>;
    using ConstReverseIterator = internal::ZArrayReverseIterator<const ObjectType>;

#pragma warning(disable : 26495)
    FORCEINLINE ZArray() : SuperType() {}
    inline ZArray(const ZArray& array) noexcept;
    inline ZArray(ZArray&& array) noexcept;

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

    inline ZArray& operator=(const ZArray& array) noexcept;
    inline ZArray& operator=(ZArray&& array) noexcept;

    NODISCARD FORCEINLINE Bool operator==(const ZArray& array) noexcept {
        return this == &array;
    }
    NODISCARD FORCEINLINE Bool operator!=(const ZArray& array) noexcept {
        return this != &array;
    }

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
    NODISCARD FORCEINLINE ObjectType* AtPtr(IndexType index) {
        DEBUG(index < 0 || index >= kCapacity, "Index out of bounds!");
        return data_ + index;
    }
    NODISCARD FORCEINLINE const ObjectType* AtPtr(IndexType index) const {
        DEBUG(index < 0 || index >= kCapacity, "Index out of bounds!");
        return data_ + index;
    }
    NODISCARD FORCEINLINE constexpr ObjectType& Front() { return data_[0]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& Front() const { return data_[0]; }
    NODISCARD FORCEINLINE ObjectType* FrontPtr() { return data_; }
    NODISCARD FORCEINLINE const ObjectType* FrontPtr() const { return data_; }
    NODISCARD FORCEINLINE constexpr ObjectType& Back() { return data_[kCapacity - 1]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& Back() const { return data_[kCapacity - 1]; }
    NODISCARD FORCEINLINE ObjectType* BackPtr() { return data_ + (kCapacity - 1); }
    NODISCARD FORCEINLINE const ObjectType* BackPtr() const { return data_ + (kCapacity - 1); }

    NODISCARD FORCEINLINE constexpr const ObjectType* data_ptr() const { return data_; }

    NODISCARD static constexpr IndexType size() { return kCapacity; }

    /*
        The iterator funcions.
    */
    NODISCARD FORCEINLINE Iterator Begin() { return Iterator(data_); }
    NODISCARD FORCEINLINE ConstIterator ConstBegin() const { return ConstIterator(data_); }
    NODISCARD FORCEINLINE ReverseIterator ReverseBegin() { return ReverseIterator(data_ + kCapacity - 1); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseBegin() const {
        return ConstReverseIterator(data_ + kCapacity - 1);
    }
    NODISCARD FORCEINLINE Iterator End() { return Iterator(data_ + kCapacity); }
    NODISCARD FORCEINLINE ConstIterator ConstEnd() const { return ConstIterator(data_ + kCapacity); }
    NODISCARD FORCEINLINE ReverseIterator ReverseEnd() { return ReverseIterator(data_ - 1); }
    NODISCARD FORCEINLINE ConstReverseIterator ConstReverseEnd() const {
        return ConstReverseIterator(data_ - 1);
    }



    template<typename... ArgsType>
    inline Void Fill(ArgsType&&... args) noexcept;

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
    Void Sort(const Iterator& begin, const Iterator& end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
        requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const Iterator& begin, const Iterator& end, CompareFunction&& compare_function) noexcept;
    /*
        The small object on the front.
    */
    Void Sort(const ReverseIterator& begin, const ReverseIterator& end) noexcept;
    /*
        Will exchange the two objects when the function is true.
    */
    template<typename CompareFunction>
        requires kIsCompareFunction<CompareFunction, ObjectType>
    Void Sort(const ReverseIterator& begin, const ReverseIterator& end, 
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
inline Void ZArray<ObjectType, kCapacity, kIfUnique>::Fill(ArgsType&&... args) noexcept {
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