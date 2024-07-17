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

template<typename... _ArgsType>
class TTuple : public ZObject {
public:
    using STDTuple_ = std::tuple<_ArgsType...>;
    template <IndexType kIndex>
    using TupleObjectType_ = std::tuple_element<kIndex, STDTuple_>::type;

    FORCEINLINE constexpr TTuple(const TTuple& _tuple) noexcept : SuperType_(_tuple), tuple_(_tuple.tuple_) {}
    FORCEINLINE constexpr TTuple(TTuple&& _tuple) noexcept 
        : SuperType_(std::forward<TTuple>(_tuple)), tuple_(std::move(_tuple.tuple_)) {}
    FORCEINLINE constexpr TTuple(_ArgsType&&... _args) noexcept 
        : SuperType_(), tuple_(std::forward<_ArgsType>(_args)...) {}
    FORCEINLINE constexpr ~TTuple() noexcept {}

    FORCEINLINE constexpr TTuple& operator=(const TTuple& _tuple) noexcept {
        SuperType_::operator=(_tuple);
        tuple_ = _tuple.tuple_;
        return *this;
    }
    FORCEINLINE constexpr TTuple& operator=(TTuple&& _tuple) noexcept {
        SuperType_::operator=(std::forward<TTuple>(_tuple));
        tuple_ = std::move(_tuple.tuple_);
        return *this;
    }
    template<typename... _OtherTupleArgsType>
    FORCEINLINE constexpr TTuple& operator=(const TTuple<_OtherTupleArgsType...>& _tuple) noexcept {
        tuple_ = _tuple.tuple_;
        return *this;
    }

    template<typename _ObjectType>
    FORCEINLINE constexpr Void Swap(TTuple& _tuple) noexcept {
        tuple_.swap(_tuple.tuple_);
    }

    template<IndexType kIndex>
    NODISCARD FORCEINLINE constexpr decltype(auto) Get() noexcept {
        return std::get<kIndex>(tuple_);
    }
    template<IndexType kIndex>
    NODISCARD FORCEINLINE constexpr decltype(auto) Get() const noexcept {
        return std::get<kIndex>(tuple_);
    }
    template<typename _ObjectType>
    NODISCARD FORCEINLINE constexpr _ObjectType& Get() noexcept {
        return std::get<_ObjectType>(tuple_);
    }
    template<typename _ObjectType>
    NODISCARD FORCEINLINE constexpr const _ObjectType& Get() const noexcept {
        return std::get<_ObjectType>(tuple_);
    }

    template<IndexType kIndex>
    FORCEINLINE constexpr Void Set(const TupleObjectType_<kIndex>& _object) noexcept {
        std::get<kIndex>(tuple_) = _object;
    }
    template<IndexType kIndex>
    FORCEINLINE constexpr Void Set(TupleObjectType_<kIndex>&& _object) noexcept {
        std::get<kIndex>(tuple_) = std::forward<TupleObjectType_<kIndex>>(_object);
    }
    template<typename _ObjectType>
    FORCEINLINE constexpr Void Set(const _ObjectType& _object) noexcept {
        std::get<_ObjectType>(tuple_) = _object;
    }
    template<typename _ObjectType>
    FORCEINLINE constexpr Void Set(_ObjectType&& _object) noexcept {
        std::get<_ObjectType>(tuple_) = std::forward<_ObjectType>(_object);
    }

    NODISCARD FORCEINLINE constexpr const IndexType Size() const noexcept {
        return static_cast<IndexType>(std::tuple_size<STDTuple_>::value);
    }
    template<typename _Function>
    NODISCARD FORCEINLINE constexpr decltype(auto) Apply(_Function&& func) noexcept {
        return std::apply(std::forward<_Function>(func), std::move(tuple_));
    }

protected:
    using SuperType_ = ZObject;

private:
    template<typename... _OtherArgsType>
    friend class TTuple;

    STDTuple_ tuple_;
};

namespace tuple {

template<typename... _ArgsType>
NODISCARD FORCEINLINE constexpr TTuple<_ArgsType...> MakeTuple(_ArgsType&&... _args) noexcept {
    return TTuple(std::forward<_ArgsType>(_args)...);
}

template<typename... _ArgsType>
NODISCARD FORCEINLINE constexpr TTuple<_ArgsType&...> Tie(_ArgsType&... _args) noexcept {
    return TTuple<_ArgsType&...>(_args...);
}

template<IndexType kIndex, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(TTuple<_ArgsType...>& _tuple) noexcept {
    return _tuple.Get<kIndex>();
}
template<IndexType kIndex, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(const TTuple<_ArgsType...>& _tuple) noexcept {
    return _tuple.Get<kIndex>();
}
template<IndexType kIndex, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(TTuple<_ArgsType...>&& _tuple) noexcept {
    return std::move(_tuple.Get<kIndex>());
}
template<IndexType kIndex, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(const TTuple<_ArgsType...>&& _tuple) noexcept {
    return std::move(_tuple.Get<kIndex>());
}
template<typename _ObjectType, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr _ObjectType& Get(TTuple<_ArgsType...>& _tuple) noexcept {
    return _tuple.Get<_ObjectType>();
}
template<typename _ObjectType, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr const _ObjectType& Get(const TTuple<_ArgsType...>& _tuple) noexcept {
    return _tuple.Get<_ObjectType>();
}
template<typename _ObjectType, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr _ObjectType&& Get(TTuple<_ArgsType...>&& _tuple) noexcept {
    return std::move(_tuple.Get<_ObjectType>());
}
template<typename _ObjectType, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr const _ObjectType&& Get(const TTuple<_ArgsType...>&& _tuple) noexcept {
    return std::move(_tuple.Get<_ObjectType>());
}

template<IndexType kIndex, typename _ObjectType, typename... _ArgsType>
FORCEINLINE constexpr Void Set(TTuple<_ArgsType...>* _tuple, _ObjectType&& _object) noexcept {
    _tuple->Set<kIndex>(std::forward<_ObjectType>(_object));
}

template<typename _ObjectType, typename... _ArgsType>
FORCEINLINE constexpr Void Set(TTuple<_ArgsType...>* _tuple, _ObjectType&& _object) noexcept {
    _tuple->Set<_ObjectType>(std::forward<_ObjectType>(_object));
}

template<typename... _ArgsType>
NODISCARD FORCEINLINE constexpr const IndexType Size(const TTuple<_ArgsType...>& _tuple) noexcept {
    return _tuple.Size();
}

template<typename _Function,typename... _ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Apply(_Function&& _func, TTuple<_ArgsType...>&& _tuple) noexcept {
    return _tuple.Apply(std::forward<_Function>(_func));
}

}//tuple
}//zengine

#endif //!Z_CORE_T_TUPLE_H_