#ifndef Z_FIXED_ARRAY_H_
#define Z_FIXED_ARRAY_H_

#include"internal/drive.h"

#include"z_object.h"

namespace zengine {

/*
    The size of the array is fixed. It's stored in the stack memory, use new
    to store it in the heap memory. This class uses no extra size, doesn't
    have any other members except the objects.
*/
template<typename ObjectType, IndexType kCapacity>
class ZFixedArray : public ZObject {
public:

#pragma warning(disable : 26495)
    __forceinline constexpr ZFixedArray() :SuperType() {}
#pragma warning(default : 26495)
    ZFixedArray(const ZFixedArray& array) noexcept;
    ZFixedArray(ZFixedArray&& array) noexcept;

    /*
        Constexpr array, the work is done at compile time.
        The Constructor's fisrt parameter is the initial funtion of the array. The
        first parameter of the initial funtion must be ZFixedArray*.
        Constructor Template Parameters:
        - InitFunction: The function type that initial the array.
        - ArgsType...: The parameters type.
        Constructor Parameters:
        - init_function: The function to initial the array.
        - ArgsType...: The parameters of the function except for the fisrt.
        Example:
        constexpr auto init_function = [](ZFixedArray<Int32, 10>* array_ptr) {
            for (IndexType index = 0; index < array_ptr->size(); ++index) {
                (*array_ptr)(index) = 1;
            }
        };
        constexpr ZFixedArray<Int32, 10> test(init_function);
    */
    template<typename InitFunction, typename... ArgsType>
    __forceinline constexpr ZFixedArray(InitFunction&& init_function, ArgsType&&... args) :SuperType() {
        init_function(this, std::forward<ArgsType>(args)...);
    }

    ZFixedArray& operator=(const ZFixedArray& array) noexcept;
    ZFixedArray& operator=(ZFixedArray&& array) noexcept;

    __forceinline constexpr ObjectType& operator()(const IndexType index) { return this->data_[index]; }
    __forceinline constexpr const ObjectType& operator()(const IndexType index) const { return this->data_[index]; }

    static constexpr const IndexType size() { return kCapacity; }
    constexpr ObjectType* data_ptr() { return data_; }
    constexpr const ObjectType* data_ptr() const { return data_; }

protected:
    using SuperType = ZObject;

private:
    ObjectType data_[kCapacity];
};

template<typename ObjectType, IndexType kCapacity>
ZFixedArray<ObjectType, kCapacity>::ZFixedArray(const ZFixedArray& array) noexcept
        : SuperType() {
    if constexpr (std::is_class_v<ObjectType>) {
        for (IndexType index = 0; index < array.size(); ++index) {
            (*this)(index) = array(index);
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZFixedArray));
    }
}

template<typename ObjectType, IndexType kCapacity>
ZFixedArray<ObjectType, kCapacity>::ZFixedArray(ZFixedArray&& array)noexcept
        : SuperType() {
    if constexpr (std::is_class_v<ObjectType>) {
        for (IndexType index = 0; index < array.size(); ++index) {
            (*this)(index) = array(index);
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZFixedArray));
    }
}

template<typename ObjectType, IndexType kCapacity>
ZFixedArray<ObjectType, kCapacity>& ZFixedArray<ObjectType, kCapacity>::operator=(const ZFixedArray& array) noexcept {
    if constexpr (std::is_class_v<ObjectType>) {
        for (IndexType index = 0; index < array.size(); ++index) {
            (*this)(index) = array(index);
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZFixedArray));
    }
    return *this;
}

template<typename ObjectType, IndexType kCapacity>
ZFixedArray<ObjectType, kCapacity>& ZFixedArray<ObjectType, kCapacity>::operator=(ZFixedArray&& array) noexcept {
    if constexpr (std::is_class_v<ObjectType>) {
        for (IndexType index = 0; index < array.size(); ++index) {
            (*this)(index) = array(index);
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZFixedArray));
    }
    return *this;
}

}//zengine

#endif // !Z_FIXED_ARRAY_H_
