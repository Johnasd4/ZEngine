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
    using STDTuple = std::tuple<ArgsType...>;
    template <IndexType kIndex>
    using TupleObjectType = std::tuple_element<kIndex, STDTuple>::type;

    FORCEINLINE constexpr TTuple(const TTuple& tuple) noexcept : SuperType(), tuple_(tuple.tuple_) {}
    FORCEINLINE constexpr TTuple(TTuple&& tuple) noexcept : SuperType(), tuple_(std::move(tuple.tuple_)) {}
    FORCEINLINE constexpr TTuple(ArgsType&&... args) noexcept : SuperType(), tuple_(std::forward<ArgsType>(args)...) {}
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

    template<typename ObjectType>
    FORCEINLINE constexpr Void Swap(TTuple& tuple) noexcept {
        tuple_.swap(tuple.tuple_);
    }

    template<IndexType kIndex>
    NODISCARD FORCEINLINE constexpr decltype(auto) Get() noexcept {
        return std::get<kIndex>(tuple_);
    }
    template<IndexType kIndex>
    NODISCARD FORCEINLINE constexpr decltype(auto) Get() const noexcept {
        return std::get<kIndex>(tuple_);
    }
    template<typename ObjectType>
    NODISCARD FORCEINLINE constexpr ObjectType& Get() noexcept {
        return std::get<ObjectType>(tuple_);
    }
    template<typename ObjectType>
    NODISCARD FORCEINLINE constexpr const ObjectType& Get() const noexcept {
        return std::get<ObjectType>(tuple_);
    }

    template<IndexType kIndex>
    FORCEINLINE constexpr Void Set(const TupleObjectType<kIndex>& object) noexcept {
        std::get<kIndex>(tuple_) = object;
    }
    template<IndexType kIndex>
    FORCEINLINE constexpr Void Set(TupleObjectType<kIndex>&& object) noexcept {
        std::get<kIndex>(tuple_) = std::forward<TupleObjectType<kIndex>>(object);
    }
    template<typename ObjectType>
    FORCEINLINE constexpr Void Set(const ObjectType& object) noexcept {
        std::get<ObjectType>(tuple_) = object;
    }
    template<typename ObjectType>
    FORCEINLINE constexpr Void Set(ObjectType&& object) noexcept {
        std::get<ObjectType>(tuple_) = std::forward<ObjectType>(object);
    }

    NODISCARD FORCEINLINE constexpr const IndexType Size() const noexcept {
        return static_cast<IndexType>(std::tuple_size<STDTuple>::value);
    }
    template<typename Function>
    NODISCARD FORCEINLINE constexpr decltype(auto) Apply(Function&& func) noexcept {
        return std::apply(std::forward<Function>(func), std::move(tuple_));
    }


protected:
    using SuperType = ZObject;

private:
    template<typename... OtherArgsType>
    friend class TTuple;

    STDTuple tuple_;
};

namespace tuple {

template<typename... ArgsType>
NODISCARD FORCEINLINE constexpr TTuple<ArgsType...> MakeTuple(ArgsType&&... args) noexcept {
    return TTuple(std::forward<ArgsType>(args)...);
}

template<typename... ArgsType>
NODISCARD FORCEINLINE constexpr TTuple<ArgsType&...> Tie(ArgsType&... args) noexcept {
    return TTuple<ArgsType&...>(args...);
}

template<IndexType kIndex, typename... ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(TTuple<ArgsType...>& tuple) noexcept {
    return tuple.Get<kIndex>();
}
template<IndexType kIndex, typename... ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(const TTuple<ArgsType...>& tuple) noexcept {
    return tuple.Get<kIndex>();
}
template<IndexType kIndex, typename... ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(TTuple<ArgsType...>&& tuple) noexcept {
    return std::move(tuple.Get<kIndex>());
}
template<IndexType kIndex, typename... ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(const TTuple<ArgsType...>&& tuple) noexcept {
    return std::move(tuple.Get<kIndex>());
}
template<typename ObjectType, typename... ArgsType>
NODISCARD FORCEINLINE constexpr ObjectType& Get(TTuple<ArgsType...>& tuple) noexcept {
    return tuple.Get<ObjectType>();
}
template<typename ObjectType, typename... ArgsType>
NODISCARD FORCEINLINE constexpr const ObjectType& Get(const TTuple<ArgsType...>& tuple) noexcept {
    return tuple.Get<ObjectType>();
}
template<typename ObjectType, typename... ArgsType>
NODISCARD FORCEINLINE constexpr ObjectType&& Get(TTuple<ArgsType...>&& tuple) noexcept {
    return std::move(tuple.Get<ObjectType>());
}
template<typename ObjectType, typename... ArgsType>
NODISCARD FORCEINLINE constexpr const ObjectType&& Get(const TTuple<ArgsType...>&& tuple) noexcept {
    return std::move(tuple.Get<ObjectType>());
}

template<IndexType kIndex, typename ObjectType, typename... ArgsType>
FORCEINLINE constexpr Void Set(TTuple<ArgsType...>* tuple, ObjectType&& object) noexcept {
    tuple->Set<kIndex>(std::forward<ObjectType>(object));
}

template<typename ObjectType, typename... ArgsType>
FORCEINLINE constexpr Void Set(TTuple<ArgsType...>* tuple, ObjectType&& object) noexcept {
    tuple->Set<ObjectType>(std::forward<ObjectType>(object));
}

template<typename... ArgsType>
NODISCARD FORCEINLINE constexpr const IndexType Size(const TTuple<ArgsType...>& tuple) noexcept {
    return tuple.Size();
}

template<typename Function,typename... ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Apply(Function&& func, TTuple<ArgsType...>&& tuple) noexcept {
    return tuple.Apply(std::forward<Function>(func));
}

}//tuple
}//zengine

#endif //!Z_CORE_T_TUPLE_H_