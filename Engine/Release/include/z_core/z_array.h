#ifndef Z_CORE_Z_ARRAY_H_
#define Z_CORE_Z_ARRAY_H_

#include "internal/drive.h"

#include "z_object.h"

namespace zengine {

/*
    The size of the array is fixed. It's stored in the stack memory, use new
    to store it in the heap memory. This class uses no extra size, doesn't
    have any other members except the objects.
*/
template<typename ObjectType, IndexType kCapacity>
class ZArray : public ZObject {
public:

#pragma warning(disable : 26495)
    FORCEINLINE ZArray() :SuperType() {}
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

    NODISCARD FORCEINLINE static constexpr const IndexType size() { return kCapacity; }
    NODISCARD FORCEINLINE constexpr ObjectType* data_ptr() { return data_; }
    NODISCARD FORCEINLINE constexpr const ObjectType* data_ptr() const { return data_; }

protected:
    using SuperType = ZObject;

private:
    ObjectType data_[kCapacity];
};

template<typename ObjectType, IndexType kCapacity>
ZArray<ObjectType, kCapacity>::ZArray(const ZArray& array) noexcept
        : SuperType() {
    if constexpr (std::is_class_v<ObjectType>) {
        for (IndexType index = 0; index < array.size(); ++index) {
            (*this)(index) = array(index);
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZArray));
    }
}

template<typename ObjectType, IndexType kCapacity>
ZArray<ObjectType, kCapacity>::ZArray(ZArray&& array)noexcept
        : SuperType() {
    if constexpr (std::is_class_v<ObjectType>) {
        for (IndexType index = 0; index < array.size(); ++index) {
            (*this)(index) = array(index);
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZArray));
    }
}

template<typename ObjectType, IndexType kCapacity>
ZArray<ObjectType, kCapacity>& ZArray<ObjectType, kCapacity>::operator=(const ZArray& array) noexcept {
    if constexpr (std::is_class_v<ObjectType>) {
        for (IndexType index = 0; index < array.size(); ++index) {
            (*this)(index) = array(index);
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZArray));
    }
    return *this;
}

template<typename ObjectType, IndexType kCapacity>
ZArray<ObjectType, kCapacity>& ZArray<ObjectType, kCapacity>::operator=(ZArray&& array) noexcept {
    if constexpr (std::is_class_v<ObjectType>) {
        for (IndexType index = 0; index < array.size(); ++index) {
            (*this)(index) = array(index);
        }
    }
    else {
        memcpy(reinterpret_cast<Void*>(data_), reinterpret_cast<Void*>(array.data_), sizeof(ZArray));
    }
    return *this;
}

}//zengine

#endif // !Z_CORE_Z_ARRAY_H_
