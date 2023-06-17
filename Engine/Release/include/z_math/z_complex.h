#ifndef Z_MATH_Z_COMPLEX_H_
#define Z_MATH_Z_COMPLEX_H_

#include "internal/drive.h"

namespace zengine {
namespace math {

/*
    Tests if the type is complex.
*/
template<typename NumberType>
concept kIsComplex = requires(NumberType number) {
    number.real;
    number.imag;
};

template<typename NumberType>
concept kCanCalculateWithComplex = std::is_arithmetic_v<NumberType> || kIsComplex<NumberType>;

/*
    Complex number.
*/
template<typename NumberType> 
requires std::is_arithmetic_v<NumberType>
struct ZComplex {   

    /*
        Used for the temp values of the composite computes.
    */
    struct ZTempComplex :public ZComplex<NumberType> {

#pragma warning(disable : 26495)
        FORCEINLINE constexpr ZTempComplex() : SuperType() {}
#pragma warning(default : 26495)        
        FORCEINLINE constexpr ZTempComplex(const ZComplex& number) : SuperType(number) {}
        template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
        FORCEINLINE constexpr ZTempComplex(const CalculateNumberType number) : SuperType(number) {}
        template<typename CalculateNumberType>
        requires std::is_arithmetic_v<CalculateNumberType>
        FORCEINLINE constexpr ZTempComplex(const CalculateNumberType real, const CalculateNumberType imag)
            : SuperType(real, imag) {}

        template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
        NODISCARD FORCEINLINE constexpr ZTempComplex& operator+(const CalculateNumberType number);
        template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
        NODISCARD FORCEINLINE constexpr ZTempComplex& operator-(const CalculateNumberType number);
        template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
        NODISCARD FORCEINLINE constexpr ZTempComplex& operator*(const CalculateNumberType number);
        template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
        NODISCARD constexpr ZTempComplex& operator/(const CalculateNumberType number) noexcept;

        NODISCARD FORCEINLINE constexpr ZTempComplex& conjugate();

    protected:
        using SuperType = ZComplex<NumberType>;
    };

#pragma warning(disable : 26495)
    FORCEINLINE constexpr ZComplex() {}
#pragma warning(default : 26495)    
    FORCEINLINE constexpr ZComplex(const ZComplex& number);
    template<typename CalculateNumberType>
    requires kCanCalculateWithComplex<CalculateNumberType>
    FORCEINLINE constexpr ZComplex(const CalculateNumberType number);
    template<typename CalculateNumberType>
    requires std::is_arithmetic_v<CalculateNumberType>
    FORCEINLINE constexpr ZComplex(const CalculateNumberType real, const CalculateNumberType imag);

    template<typename CalculateNumberType> 
    requires kCanCalculateWithComplex<CalculateNumberType>
    FORCEINLINE constexpr ZComplex& operator=(const CalculateNumberType number);
    FORCEINLINE constexpr ZComplex& operator=(const ZComplex& number);

    template<typename CalculateNumberType>
    requires kCanCalculateWithComplex<CalculateNumberType>
    NODISCARD FORCEINLINE constexpr const Bool operator==(const CalculateNumberType number) const;

    template<typename CalculateNumberType>
    requires kCanCalculateWithComplex<CalculateNumberType>
    NODISCARD FORCEINLINE constexpr ZTempComplex operator+(const CalculateNumberType number) const;
    template<typename CalculateNumberType>
    requires kCanCalculateWithComplex<CalculateNumberType>
    NODISCARD FORCEINLINE constexpr ZTempComplex operator-(const CalculateNumberType number) const;
    template<typename CalculateNumberType>
    requires kCanCalculateWithComplex<CalculateNumberType>
    NODISCARD FORCEINLINE constexpr ZTempComplex operator*(const CalculateNumberType number) const;
    template<typename CalculateNumberType>
    requires kCanCalculateWithComplex<CalculateNumberType>
    NODISCARD constexpr ZTempComplex operator/(const CalculateNumberType number) const noexcept;

    template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
    FORCEINLINE constexpr ZComplex& operator+=(const CalculateNumberType number);
    template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
    FORCEINLINE constexpr ZComplex& operator-=(const CalculateNumberType number);
    template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
    FORCEINLINE constexpr ZComplex& operator*=(const CalculateNumberType number);
    template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
    constexpr ZComplex& operator/=(const CalculateNumberType number) noexcept;

    NODISCARD FORCEINLINE constexpr ZTempComplex conjugate();

    NODISCARD FORCEINLINE constexpr const NumberType magnitude();

    NumberType real;
    NumberType imag;
};

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>::ZComplex(const CalculateNumberType number) {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real = number;
        imag = static_cast<NumberType>(0.0);
    }
    else {
        real = static_cast<NumberType>(number.real);
        imag = static_cast<NumberType>(number.imag);
    }
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
FORCEINLINE constexpr ZComplex<NumberType>::ZComplex(const ZComplex& number) {
    real = number.real;
    imag = number.imag;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires std::is_arithmetic_v<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>::ZComplex(const CalculateNumberType real, const CalculateNumberType imag) {
    this->real = static_cast<NumberType>(real);
    this->imag = static_cast<NumberType>(imag);
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>& ZComplex<NumberType>::operator=(const CalculateNumberType number) {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real = static_cast<NumberType>(number);
        imag = static_cast<NumberType>(0);
    }
    else {
        real = static_cast<NumberType>(number.real);
        imag = static_cast<NumberType>(number.imag);
    }
    return *this;
}
template<typename NumberType> 
requires std::is_arithmetic_v<NumberType>
FORCEINLINE constexpr ZComplex<NumberType>& ZComplex<NumberType>::operator=(const ZComplex& number) {
    real = number.real;
    imag = number.imag;
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr const Bool ZComplex<NumberType>::operator==(const CalculateNumberType number) const {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        return real == static_cast<NumberType>(number) && imag == static_cast<NumberType>(0);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        return real == static_cast<NumberType>(number.real) && imag == static_cast<NumberType>(number.imag);
    }
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex ZComplex<NumberType>::operator+(
        const CalculateNumberType number) const {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        ZTempComplex complex(real + static_cast<NumberType>(number), imag);
        return complex;
    }
    else if constexpr (kIsComplex<CalculateNumberType>){
        ZTempComplex complex(real + static_cast<NumberType>(number.real), imag + static_cast<NumberType>(number.imag));
        return complex;
    }
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex ZComplex<NumberType>::operator-(
        const CalculateNumberType number) const {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        ZTempComplex complex(real - static_cast<NumberType>(number), imag);
        return complex;
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        ZTempComplex complex(real - static_cast<NumberType>(number.real), imag - static_cast<NumberType>(number.imag));
        return complex;
    }
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex ZComplex<NumberType>::operator*(
        const CalculateNumberType number) const {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        ZTempComplex complex(real * static_cast<NumberType>(number), imag * static_cast<NumberType>(number));
        return complex;
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        ZTempComplex complex(real * static_cast<NumberType>(number.real) - imag * static_cast<NumberType>(number.imag), 
                             real * static_cast<NumberType>(number.imag) + imag * static_cast<NumberType>(number.real));
        return complex;
    }
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD constexpr ZComplex<NumberType>::ZTempComplex ZComplex<NumberType>::operator/(
        const CalculateNumberType number) const noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        ZTempComplex complex(real / static_cast<NumberType>(number), imag / static_cast<NumberType>(number));
        return complex;
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        NumberType devide_number =
            1.0F / (imag * imag + static_cast<NumberType>(number.imag) * static_cast<NumberType>(number.imag));
        ZTempComplex complex(
            (real * static_cast<NumberType>(number.real) + imag * static_cast<NumberType>(number.imag)) * devide_number, 
            (imag * static_cast<NumberType>(number.real) - real * static_cast<NumberType>(number.imag)) * devide_number
        );
        return complex;
    }
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>& ZComplex<NumberType>::operator+=(const CalculateNumberType number) {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real += static_cast<NumberType>(number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        real += static_cast<NumberType>(number.real);
        real += static_cast<NumberType>(number.imag);
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>& ZComplex<NumberType>::operator-=(const CalculateNumberType number) {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real -= static_cast<NumberType>(number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        real -= static_cast<NumberType>(number.real);
        real -= static_cast<NumberType>(number.imag);
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>& ZComplex<NumberType>::operator*=(const CalculateNumberType number) {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real *= static_cast<NumberType>(number);
        imag *= static_cast<NumberType>(number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        NumberType real_temp = real * static_cast<NumberType>(number.real) - imag * static_cast<NumberType>(number.imag);
        imag = real * static_cast<NumberType>(number.imag) + imag * static_cast<NumberType>(number.real);
        real = real_temp;
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>& ZComplex<NumberType>::operator/=(const CalculateNumberType number) noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real /= static_cast<NumberType>(number);
        imag /= static_cast<NumberType>(number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        NumberType devide_number = 
            1.0F / (imag * imag + static_cast<NumberType>(number.imag) * static_cast<NumberType>(number.imag));       
        NumberType real_temp = 
            (real * static_cast<NumberType>(number.real) + imag * static_cast<NumberType>(number.imag)) *
            devide_number;
        imag = (imag * static_cast<NumberType>(number.real) - real * static_cast<NumberType>(number.imag)) * 
               devide_number;
        real = real_temp;
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex ZComplex<NumberType>::conjugate() {
    return ZTempComplex(real, -imag);
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
FORCEINLINE constexpr const NumberType ZComplex<NumberType>::magnitude() {
    return sqrt(real * real + imag * imag);
}




template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex& ZComplex<NumberType>::ZTempComplex::operator+(
        const CalculateNumberType number) {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real += static_cast<NumberType>(number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>){
        real += static_cast<NumberType>(number.real);
        imag += static_cast<NumberType>(number.imag);
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex& ZComplex<NumberType>::ZTempComplex::operator-(
    const CalculateNumberType number) {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real -= static_cast<NumberType>(number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        real -= static_cast<NumberType>(number.real);
        imag -= static_cast<NumberType>(number.imag);
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex& ZComplex<NumberType>::ZTempComplex::operator*(
    const CalculateNumberType number) {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real = real * static_cast<NumberType>(number);
        imag = imag * static_cast<NumberType>(number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        NumberType real_temp = real * static_cast<NumberType>(number.real) - 
                               imag * static_cast<NumberType>(number.imag);
        imag = real * static_cast<NumberType>(number.imag) + imag * static_cast<NumberType>(number.real);
        real = real_temp;
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD constexpr ZComplex<NumberType>::ZTempComplex& ZComplex<NumberType>::ZTempComplex::operator/(
    const CalculateNumberType number) noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real = real / static_cast<NumberType>(number);
        imag = imag / static_cast<NumberType>(number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        NumberType devide_number =
            1.0F / (imag * imag + static_cast<NumberType>(number.imag) * static_cast<NumberType>(number.imag));
        NumberType real_temp = 
            (real * static_cast<NumberType>(number.real) + imag * static_cast<NumberType>(number.imag)) * devide_number;
        imag = 
            (imag * static_cast<NumberType>(number.real) - real * static_cast<NumberType>(number.imag)) * devide_number;
        real = real_temp;
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex& ZComplex<NumberType>::ZTempComplex::conjugate() {
    imag = -imag;
    return *this;
}

}//math
}//zengine

#endif //!Z_MATH_Z_COMPLEX_H_