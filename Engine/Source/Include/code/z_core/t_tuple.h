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
#ifndef Z_CORE_T_TUPLE_H_
#define Z_CORE_T_TUPLE_H_

#include "internal/z_drive.h"

#include <tuple>

#include "z_object.h"

namespace zengine {

template<typename... ArgsType>
class TTuple : public ZObject {
public:
    FORCEINLINE constexpr TTuple() noexcept : SuperType(), tuple_() {}
    FORCEINLINE constexpr TTuple(const TTuple& tuple) noexcept :
        SuperType(), tuple_(tuple.tuple_) {}
    FORCEINLINE constexpr TTuple(TTuple&& tuple) noexcept :
        SuperType(), tuple_(std::move(tuple.tuple_)) {}
    FORCEINLINE constexpr TTuple(ArgsType&&... args) noexcept :
        SuperType(), tuple_(std::forward<ArgsType>(args)...) {}
    FORCEINLINE constexpr ~TTuple() noexcept {}

    FORCEINLINE constexpr TTuple& operator=(const TTuple& tuple) noexcept {
        tuple_.operator=(tuple.tuple_);
        return *this;
    }
    FORCEINLINE constexpr TTuple& operator=(TTuple&& tuple) noexcept {
        tuple_.operator=(std::move(tuple.tuple_));
        return *this;
    }
    template<typename... OtherTupleArgsType>
    FORCEINLINE constexpr TTuple& operator=(const TTuple<OtherTupleArgsType...>& tuple) noexcept {
        tuple_.operator=(tuple.tuple_);
        return *this;
    }

protected:
    using SuperType = ZObject;

private:
    template<typename... OtherArgsType>
    friend class TTuple;

    std::tuple<ArgsType...> tuple_;
};

template<typename... ArgsType>
FORCEINLINE constexpr TTuple<ArgsType...> MakeTuple(ArgsType&&... args) noexcept {
    return TTuple(std::forward<ArgsType>(args)...);
}

template<typename... ArgsType>
FORCEINLINE constexpr TTuple<ArgsType&...> Tie(ArgsType&... args) noexcept {
    return TTuple<ArgsType&...>(args...);
}

}//zengine


#endif //!Z_CORE_T_TUPLE_H_