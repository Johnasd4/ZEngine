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
#ifndef Z_CORE_T_PAIR_H_
#define Z_CORE_T_PAIR_H_

#include "internal/z_drive.h"

#include "z_object.h"

namespace zengine {

/*
    Pair struct.
*/
template<typename _LeftObjectType, typename _RightObjectType>
struct TPair : public ZObject {
public:
    constexpr TPair() noexcept : SuperType_(), first_(), second_() {}
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    constexpr TPair(const TPair<_OtherObjectType1, _OtherObjectType2>& _pair) noexcept
            : SuperType_(), first_(_pair.first_), second_(_pair.second_) {}
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    constexpr TPair(TPair<_OtherObjectType1, _OtherObjectType2>&& _pair) noexcept
            : SuperType_(), first_(std::move(_pair.first_)), second_(std::move(_pair.second_)) {}
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    constexpr TPair(const _OtherObjectType1& _obj_1, const _OtherObjectType2& _obj_2) noexcept
            : SuperType_(), first_(_obj_1), second_(_obj_2) {}
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    constexpr TPair(const _OtherObjectType1&& _obj_1, const _OtherObjectType2&& _obj_2) noexcept
            : SuperType_()
            , first_(std::forward<_OtherObjectType1>(_obj_1))
            , second_(std::forward<_OtherObjectType2>(_obj_2)) {}

    constexpr ~TPair() noexcept {}

    template<typename _OtherObjectType1, typename _OtherObjectType2>
    constexpr TPair& operator=(const TPair<_OtherObjectType1, _OtherObjectType2>& _pair) noexcept {
        first_ = _pair.first_;
        second_ = _pair.second_;
        return *this;
    }
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    constexpr TPair& operator=(TPair<_OtherObjectType1, _OtherObjectType2>&& _pair) noexcept {
        first_ = std::move(_pair.first_);
        second_ = std::move(_pair.second_);
        return *this;
    }

    template<typename _OtherObjectType1, typename _OtherObjectType2>
    NODISCARD constexpr Bool operator==(const TPair<_OtherObjectType1, _OtherObjectType2>& _pair) noexcept {
        return first_ == _pair.first_ && second_ == _pair.second_;
    }
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    NODISCARD constexpr Bool operator!=(const TPair<_OtherObjectType1, _OtherObjectType2>& _pair) noexcept {
        return first_ != _pair.first_ || second_ != _pair.second_;
    }
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    NODISCARD constexpr Bool operator>(const TPair<_OtherObjectType1, _OtherObjectType2>& _pair) noexcept {
        return first_ == _pair.first_ ? true : second_ > _pair.second_;
    }
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    NODISCARD constexpr Bool operator>=(const TPair<_OtherObjectType1, _OtherObjectType2>& _pair) noexcept {
        return first_ == _pair.first_ ? true : second_ >= _pair.second_;
    }
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    NODISCARD constexpr Bool operator<(const TPair<_OtherObjectType1, _OtherObjectType2>& _pair) noexcept {
        return first_ == _pair.first_ ? true : second_ < _pair.second_;
    }
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    NODISCARD constexpr Bool operator<=(const TPair<_OtherObjectType1, _OtherObjectType2>& _pair) noexcept {
        return first_ == _pair.first_ ? true : second_ <= _pair.second_;
    }

    _LeftObjectType first_;
    _RightObjectType second_;
protected:
    using SuperType_ = ZObject;
};

}//zengine

#endif // !Z_CORE_T_PAIR_H_