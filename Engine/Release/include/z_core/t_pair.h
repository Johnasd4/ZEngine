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
template<typename ObjectType1, typename ObjectType2>
struct TPair : public ZObject {
public:
    constexpr TPair() noexcept : SuperType(), first(), second() {}
    template<typename OtherObjectType1, typename OtherObjectType2>
    constexpr TPair(const TPair<OtherObjectType1, OtherObjectType2>& pair) noexcept
            : SuperType(), first(pair.first), second(pair.second) {}
    template<typename OtherObjectType1, typename OtherObjectType2>
    constexpr TPair(TPair<OtherObjectType1, OtherObjectType2>&& pair) noexcept
            : SuperType(), first(std::move(pair.first)), second(std::move(pair.second)) {}
    template<typename OtherObjectType1, typename OtherObjectType2>
    constexpr TPair(const OtherObjectType1& object1, const OtherObjectType2& object2) noexcept
            : SuperType(), first(object1), second(object2) {}
    template<typename OtherObjectType1, typename OtherObjectType2>
    constexpr TPair(const OtherObjectType1&& object1, const OtherObjectType2&& object2) noexcept
            : SuperType()
            , first(std::forward<OtherObjectType1>(object1))
            , second(std::forward<OtherObjectType2>(object2)) {}

    constexpr ~TPair() noexcept {}

    template<typename OtherObjectType1, typename OtherObjectType2>
    constexpr TPair& operator=(const TPair<OtherObjectType1, OtherObjectType2>& pair) noexcept {
        first = pair.first;
        second = pair.second;
        return *this;
    }
    template<typename OtherObjectType1, typename OtherObjectType2>
    constexpr TPair& operator=(TPair<OtherObjectType1, OtherObjectType2>&& pair) noexcept {
        first = std::move(pair.first);
        second = std::move(pair.second);
        return *this;
    }

    template<typename OtherObjectType1, typename OtherObjectType2>
    NODISCARD constexpr Bool operator==(const TPair<OtherObjectType1, OtherObjectType2>& pair) noexcept {
        return first == pair.first && second == pair.second;
    }
    template<typename OtherObjectType1, typename OtherObjectType2>
    NODISCARD constexpr Bool operator!=(const TPair<OtherObjectType1, OtherObjectType2>& pair) noexcept {
        return first != pair.first || second != pair.second;
    }
    template<typename OtherObjectType1, typename OtherObjectType2>
    NODISCARD constexpr Bool operator>(const TPair<OtherObjectType1, OtherObjectType2>& pair) noexcept {
        return first == pair.first ? true : second > pair.second;
    }
    template<typename OtherObjectType1, typename OtherObjectType2>
    NODISCARD constexpr Bool operator>=(const TPair<OtherObjectType1, OtherObjectType2>& pair) noexcept {
        return first == pair.first ? true : second >= pair.second;
    }
    template<typename OtherObjectType1, typename OtherObjectType2>
    NODISCARD constexpr Bool operator<(const TPair<OtherObjectType1, OtherObjectType2>& pair) noexcept {
        return first == pair.first ? true : second < pair.second;
    }
    template<typename OtherObjectType1, typename OtherObjectType2>
    NODISCARD constexpr Bool operator<=(const TPair<OtherObjectType1, OtherObjectType2>& pair) noexcept {
        return first == pair.first ? true : second <= pair.second;
    }

    ObjectType1 first;
    ObjectType2 second;
protected:
    using SuperType = ZObject;
};

}//zengine

#endif // !Z_CORE_T_PAIR_H_