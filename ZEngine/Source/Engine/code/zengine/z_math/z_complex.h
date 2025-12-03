/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "drive.h"

namespace zengine {
namespace math {

/*
    Tests if the type is complex.
*/
template<typename NumberType>
concept kIsComplex = requires(NumberType _number) {
    _number.real_;
    _number.imag_;
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
        FORCEINLINE constexpr ZTempComplex() noexcept : SuperType() {}
#pragma warning(default : 26495)        
        FORCEINLINE constexpr ZTempComplex(const ZComplex& _number) noexcept : SuperType(_number) {}
        template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
        FORCEINLINE constexpr ZTempComplex(const CalculateNumberType _number) noexcept : SuperType(_number) {}
        template<typename CalculateNumberType>
        requires std::is_arithmetic_v<CalculateNumberType>
        FORCEINLINE constexpr ZTempComplex(const CalculateNumberType _real, const CalculateNumberType _imag) noexcept
            : SuperType(_real, _imag) {}

        template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
        NODISCARD FORCEINLINE constexpr ZTempComplex& operator+(const CalculateNumberType _number) noexcept;
        template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
        NODISCARD FORCEINLINE constexpr ZTempComplex& operator-(const CalculateNumberType _number) noexcept;
        template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
        NODISCARD FORCEINLINE constexpr ZTempComplex& operator*(const CalculateNumberType _number) noexcept;
        template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
        NODISCARD constexpr ZTempComplex& operator/(const CalculateNumberType _number) noexcept;

        NODISCARD FORCEINLINE constexpr ZTempComplex& Conjugate() noexcept;

    protected:
        using SuperType = ZComplex<NumberType>;
    };

#pragma warning(disable : 26495)
    FORCEINLINE constexpr ZComplex() noexcept {}
#pragma warning(default : 26495)    
    FORCEINLINE constexpr ZComplex(const ZComplex& _number) noexcept;
    template<typename CalculateNumberType>
    requires kCanCalculateWithComplex<CalculateNumberType>
    FORCEINLINE constexpr ZComplex(const CalculateNumberType _number) noexcept;
    template<typename CalculateNumberType>
    requires std::is_arithmetic_v<CalculateNumberType>
    FORCEINLINE constexpr ZComplex(const CalculateNumberType _real, const CalculateNumberType _imag) noexcept;

    template<typename CalculateNumberType> 
    requires kCanCalculateWithComplex<CalculateNumberType>
    FORCEINLINE constexpr ZComplex& operator=(const CalculateNumberType _number) noexcept;
    FORCEINLINE constexpr ZComplex& operator=(const ZComplex& _number) noexcept;

    template<typename CalculateNumberType>
    requires kCanCalculateWithComplex<CalculateNumberType>
    NODISCARD FORCEINLINE constexpr const Bool operator==(const CalculateNumberType _number) const noexcept;

    template<typename CalculateNumberType>
    requires kCanCalculateWithComplex<CalculateNumberType>
    NODISCARD FORCEINLINE constexpr ZTempComplex operator+(const CalculateNumberType _number) const noexcept;
    template<typename CalculateNumberType>
    requires kCanCalculateWithComplex<CalculateNumberType>
    NODISCARD FORCEINLINE constexpr ZTempComplex operator-(const CalculateNumberType _number) const noexcept;
    template<typename CalculateNumberType>
    requires kCanCalculateWithComplex<CalculateNumberType>
    NODISCARD FORCEINLINE constexpr ZTempComplex operator*(const CalculateNumberType _number) const noexcept;
    template<typename CalculateNumberType>
    requires kCanCalculateWithComplex<CalculateNumberType>
    NODISCARD constexpr ZTempComplex operator/(const CalculateNumberType _number) const noexcept;

    template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
    FORCEINLINE constexpr ZComplex& operator+=(const CalculateNumberType _number) noexcept;
    template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
    FORCEINLINE constexpr ZComplex& operator-=(const CalculateNumberType _number) noexcept;
    template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
    FORCEINLINE constexpr ZComplex& operator*=(const CalculateNumberType _number) noexcept;
    template<typename CalculateNumberType>
        requires kCanCalculateWithComplex<CalculateNumberType>
    constexpr ZComplex& operator/=(const CalculateNumberType _number) noexcept;

    NODISCARD FORCEINLINE constexpr ZTempComplex Conjugate() noexcept;

    NODISCARD FORCEINLINE constexpr const NumberType Magnitude() noexcept;

    NumberType real_;
    NumberType imag_;
};

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>::ZComplex(const CalculateNumberType _number) noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real_ = _number;
        imag_ = static_cast<NumberType>(0.0);
    }
    else {
        real_ = static_cast<NumberType>(_number.real_);
        imag_ = static_cast<NumberType>(_number.imag_);
    }
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
FORCEINLINE constexpr ZComplex<NumberType>::ZComplex(const ZComplex& _number) noexcept {
    real_ = _number.real_;
    imag_ = _number.imag_;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires std::is_arithmetic_v<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>::ZComplex(
    const CalculateNumberType _real,
    const CalculateNumberType _imag
) noexcept {
    this->real_ = static_cast<NumberType>(_real);
    this->imag_ = static_cast<NumberType>(_imag);
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>& ZComplex<NumberType>::operator=(
    const CalculateNumberType _number
) noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real_ = static_cast<NumberType>(_number);
        imag_ = static_cast<NumberType>(0);
    }
    else {
        real_ = static_cast<NumberType>(_number.real_);
        imag_ = static_cast<NumberType>(_number.imag_);
    }
    return *this;
}
template<typename NumberType> 
requires std::is_arithmetic_v<NumberType>
FORCEINLINE constexpr ZComplex<NumberType>& ZComplex<NumberType>::operator=(const ZComplex& _number) noexcept {
    real_ = _number.real_;
    imag_ = _number.imag_;
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr const Bool ZComplex<NumberType>::operator==(
    const CalculateNumberType _number
) const noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        return real_ == static_cast<NumberType>(_number) && imag_ == static_cast<NumberType>(0);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        return real_ == static_cast<NumberType>(_number.real_) && imag_ == static_cast<NumberType>(_number.imag_);
    }
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex ZComplex<NumberType>::operator+(
        const CalculateNumberType _number
) const noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        ZTempComplex complex(real_ + static_cast<NumberType>(_number), imag_);
        return complex;
    }
    else if constexpr (kIsComplex<CalculateNumberType>){
        ZTempComplex complex(
            real_ + static_cast<NumberType>(_number.real_), 
            imag_ + static_cast<NumberType>(_number.imag_)
        );
        return complex;
    }
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex ZComplex<NumberType>::operator-(
    const CalculateNumberType _number
) const noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        ZTempComplex complex(real_ - static_cast<NumberType>(_number), imag_);
        return complex;
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        ZTempComplex complex(
            real_ - static_cast<NumberType>(_number.real_), 
            imag_ - static_cast<NumberType>(_number.imag_)
        );
        return complex;
    }
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex ZComplex<NumberType>::operator*(
    const CalculateNumberType _number
) const noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        ZTempComplex complex(real_ * static_cast<NumberType>(_number), imag_ * static_cast<NumberType>(_number));
        return complex;
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        ZTempComplex complex(
            real_ * static_cast<NumberType>(_number.real_) - imag_ * static_cast<NumberType>(_number.imag_),
            real_ * static_cast<NumberType>(_number.imag_) + imag_ * static_cast<NumberType>(_number.real_)
        );
        return complex;
    }
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD constexpr ZComplex<NumberType>::ZTempComplex ZComplex<NumberType>::operator/(
    const CalculateNumberType _number
) const noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        ZTempComplex complex(real_ / static_cast<NumberType>(_number), imag_ / static_cast<NumberType>(_number));
        return complex;
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        NumberType devide_number =
            1.0F / (imag_ * imag_ + static_cast<NumberType>(_number.imag_) * static_cast<NumberType>(_number.imag_));
        ZTempComplex complex(
            (real_ * static_cast<NumberType>(_number.real_) + imag_ * static_cast<NumberType>(_number.imag_)) * devide_number,
            (imag_ * static_cast<NumberType>(_number.real_) - real_ * static_cast<NumberType>(_number.imag_)) * devide_number
        );
        return complex;
    }
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>& ZComplex<NumberType>::operator+=(
    const CalculateNumberType _number
) noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real_ += static_cast<NumberType>(_number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        real_ += static_cast<NumberType>(_number.real_);
        real_ += static_cast<NumberType>(_number.imag_);
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>& ZComplex<NumberType>::operator-=(
    const CalculateNumberType _number
) noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real_ -= static_cast<NumberType>(_number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        real_ -= static_cast<NumberType>(_number.real_);
        real_ -= static_cast<NumberType>(_number.imag_);
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>& ZComplex<NumberType>::operator*=(
    const CalculateNumberType _number
) noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real_ *= static_cast<NumberType>(_number);
        imag_ *= static_cast<NumberType>(_number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        NumberType real_temp = real_ * static_cast<NumberType>(_number.real_) - imag_ * static_cast<NumberType>(_number.imag_);
        imag_ = real_ * static_cast<NumberType>(_number.imag_) + imag_ * static_cast<NumberType>(_number.real_);
        real_ = real_temp;
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
FORCEINLINE constexpr ZComplex<NumberType>& ZComplex<NumberType>::operator/=(
    const CalculateNumberType _number
) noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real_ /= static_cast<NumberType>(_number);
        imag_ /= static_cast<NumberType>(_number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        NumberType devide_number = 
            1.0F / (imag_ * imag_ + static_cast<NumberType>(_number.imag_) * static_cast<NumberType>(_number.imag_));
        NumberType real_temp = 
            (real_ * static_cast<NumberType>(_number.real_) + imag_ * static_cast<NumberType>(_number.imag_)) *
            devide_number;
        imag_ = (imag_ * static_cast<NumberType>(_number.real_) - real_ * static_cast<NumberType>(_number.imag_)) *
               devide_number;
        real_ = real_temp;
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex ZComplex<NumberType>::Conjugate() noexcept {
    return ZTempComplex(real_, -imag_);
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
FORCEINLINE constexpr const NumberType ZComplex<NumberType>::Magnitude() noexcept {
    return sqrt(real_ * real_ + imag_ * imag_);
}




template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex& ZComplex<NumberType>::ZTempComplex::operator+(
    const CalculateNumberType _number
) noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real_ += static_cast<NumberType>(_number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>){
        real_ += static_cast<NumberType>(_number.real_);
        imag_ += static_cast<NumberType>(_number.imag_);
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex& ZComplex<NumberType>::ZTempComplex::operator-(
    const CalculateNumberType _number
) noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real_ -= static_cast<NumberType>(_number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        real_ -= static_cast<NumberType>(_number.real_);
        imag_ -= static_cast<NumberType>(_number.imag_);
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex& ZComplex<NumberType>::ZTempComplex::operator*(
    const CalculateNumberType _number
) noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real_ = real_ * static_cast<NumberType>(_number);
        imag_ = imag_ * static_cast<NumberType>(_number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        NumberType real_temp = 
            real_ * static_cast<NumberType>(_number.real_) - imag_ * static_cast<NumberType>(_number.imag_);
        imag_ = real_ * static_cast<NumberType>(_number.imag_) + imag_ * static_cast<NumberType>(_number.real_);
        real_ = real_temp;
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
template<typename CalculateNumberType>
requires kCanCalculateWithComplex<CalculateNumberType>
NODISCARD constexpr ZComplex<NumberType>::ZTempComplex& ZComplex<NumberType>::ZTempComplex::operator/(
    const CalculateNumberType _number
) noexcept {
    if constexpr (std::is_arithmetic_v<CalculateNumberType>) {
        real_ = real_ / static_cast<NumberType>(_number);
        imag_ = imag_ / static_cast<NumberType>(_number);
    }
    else if constexpr (kIsComplex<CalculateNumberType>) {
        NumberType devide_number =
            1.0F / (imag_ * imag_ + static_cast<NumberType>(_number.imag_) * static_cast<NumberType>(_number.imag_));
        NumberType real_temp = 
            (real_ * static_cast<NumberType>(_number.real_) + imag_ * static_cast<NumberType>(_number.imag_)) * devide_number;
        imag_ = 
            (imag_ * static_cast<NumberType>(_number.real_) - real_ * static_cast<NumberType>(_number.imag_)) * devide_number;
        real_ = real_temp;
    }
    return *this;
}

template<typename NumberType>
requires std::is_arithmetic_v<NumberType>
NODISCARD FORCEINLINE constexpr ZComplex<NumberType>::ZTempComplex& ZComplex<NumberType>::ZTempComplex::Conjugate() noexcept {
    imag_ = -imag_;
    return *this;
}

}//math
}//zengine