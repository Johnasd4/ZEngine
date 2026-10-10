/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "drive.h"

#include <tuple>

#include "z_object.h"

namespace zengine {

/**
 * @brief A fixed-size collection of heterogeneous values inheriting from ZObject.
 *
 * Principle: This class wraps the standard library's std::tuple to provide a container that integrates
 * with the ZEngine's object system. It provides mechanisms for type-safe storage, retrieval, and manipulation
 * of a sequence of objects of specified types. It ensures proper interaction with the engine's memory management
 * via ZObject inheritance.
 *
 * @tparam _ArgsType The types of the elements that the tuple stores.
 */
template<typename... _ArgsType>
class TTuple : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;
    /** @brief Alias for the underlying standard tuple type. */
    using STDTuple_ = std::tuple<_ArgsType...>;
    /** @brief Alias for the specific index element type. */
    template <SizeType kIndex>
    using TupleObjectType_ = std::tuple_element<kIndex, STDTuple_>::type;

    /**
     * @brief Default constructor.
     * Constructs an empty tuple or a tuple with default-initialized elements.
     */
    FORCEINLINE constexpr TTuple() noexcept : SuperType_(), tuple_() {}

    /**
     * @brief Copy constructor.
     * @param _tuple The tuple to copy from.
     */
    FORCEINLINE constexpr TTuple(const TTuple& _tuple) noexcept : SuperType_(_tuple), tuple_(_tuple.tuple_) {}

    /**
     * @brief Non-const copy constructor.
     * Provided to prevent the generic constructor from shadowing the copy constructor for non-const lvalues.
     * @param _tuple The tuple to copy from.
     */
    FORCEINLINE constexpr TTuple(TTuple& _tuple) noexcept : SuperType_(_tuple), tuple_(_tuple.tuple_) {}

    /**
     * @brief Move constructor.
     * @param _tuple The tuple to move from.
     */
    FORCEINLINE constexpr TTuple(TTuple&& _tuple) noexcept 
        : SuperType_(std::move(_tuple))
        , tuple_(std::move(_tuple.tuple_)) 
    {}

    /**
     * @brief Generic forwarding constructor.
     * Initializes the tuple with the given arguments. This constructor participates in overload resolution
     * only if the number of arguments matches and it is not a copy/move construction of the Tuple itself.
     * @tparam _InitArgsType The types of the initialization arguments.
     * @param _args The arguments to initialize the tuple elements.
     */
    template<
        typename... _InitArgsType, 
        typename = std::enable_if_t<
            sizeof...(_InitArgsType) == sizeof...(_ArgsType) && !(
                sizeof...(_InitArgsType) == 1 && 
                kIsSameType<TTuple, kDecayType<std::tuple_element_t<0, std::tuple<_InitArgsType..., Void>>>>
            )
        >
    >
    FORCEINLINE constexpr explicit TTuple(_InitArgsType&&... _args) noexcept
        : SuperType_()
        , tuple_(std::forward<_InitArgsType>(_args)...) 
    {}

    /** @brief Destructor. */
    FORCEINLINE constexpr ~TTuple() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param _tuple The tuple to assign from.
     * @return A reference to this tuple.
     */
    FORCEINLINE constexpr TTuple& operator=(const TTuple& _tuple) noexcept {
        SuperType_::operator=(_tuple);
        tuple_ = _tuple.tuple_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param _tuple The tuple to assign from.
     * @return A reference to this tuple.
     */
    FORCEINLINE constexpr TTuple& operator=(TTuple&& _tuple) noexcept {
        SuperType_::operator=(std::move(_tuple));
        tuple_ = std::move(_tuple.tuple_);
        return *this;
    }

    /**
     * @brief Templated copy assignment operator.
     * Allows assignment from a tuple with convertible types.
     * @tparam _OtherTupleArgsType The types of the other tuple's elements.
     * @param _tuple The other tuple to assign from.
     * @return A reference to this tuple.
     */
    template<typename... _OtherTupleArgsType>
    FORCEINLINE constexpr TTuple& operator=(const TTuple<_OtherTupleArgsType...>& _tuple) noexcept {
        SuperType_::operator=(_tuple);
        tuple_ = _tuple.tuple_;
        return *this;
    }

    /**
     * @brief Returns the number of elements in the tuple.
     * @return The size of the tuple.
     */
    NODISCARD FORCEINLINE constexpr const SizeType GetSize() const noexcept {
        return static_cast<SizeType>(std::tuple_size<STDTuple_>::value);
    }

    /**
     * @brief Gets a reference to the element at the specified index.
     * @tparam kIndex The index of the element to retrieve.
     * @return A reference to the element at the specified index.
     */
    template<SizeType kIndex>
    NODISCARD FORCEINLINE constexpr decltype(auto) Get() & noexcept {
        return std::get<kIndex>(tuple_);
    }

    /**
     * @brief Gets a const reference to the element at the specified index.
     * @tparam kIndex The index of the element to retrieve.
     * @return A const reference to the element at the specified index.
     */
    template<SizeType kIndex>
    NODISCARD FORCEINLINE constexpr decltype(auto) Get() const & noexcept {
        return std::get<kIndex>(tuple_);
    }

    /**
     * @brief Gets an rvalue reference to the element at the specified index (moves the element).
     * @tparam kIndex The index of the element to retrieve.
     * @return An rvalue reference to the element at the specified index.
     */
    template<SizeType kIndex>
    NODISCARD FORCEINLINE constexpr decltype(auto) Get() && noexcept {
        return std::get<kIndex>(std::move(tuple_));
    }

    /**
     * @brief Gets a reference to the element of the specified type.
     * @tparam _ObjectType The type of the element to retrieve.
     * @return A reference to the element of the specified type.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE constexpr _ObjectType& Get() & noexcept {
        return std::get<_ObjectType>(tuple_);
    }

    /**
     * @brief Gets a const reference to the element of the specified type.
     * @tparam _ObjectType The type of the element to retrieve.
     * @return A const reference to the element of the specified type.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE constexpr const _ObjectType& Get() const & noexcept {
        return std::get<_ObjectType>(tuple_);
    }

    /**
     * @brief Gets an rvalue reference to the element of the specified type (moves the element).
     * @tparam _ObjectType The type of the element to retrieve.
     * @return An rvalue reference to the element of the specified type.
     */
    template<typename _ObjectType>
    NODISCARD FORCEINLINE constexpr _ObjectType&& Get() && noexcept {
        return std::get<_ObjectType>(std::move(tuple_));
    }

    /**
     * @brief Sets the value of the element at the specified index.
     * @tparam kIndex The index of the element to set.
     * @param _object The value to set.
     */
    template<SizeType kIndex>
    FORCEINLINE constexpr Void Set(const TupleObjectType_<kIndex>& _object) noexcept {
        std::get<kIndex>(tuple_) = _object;
    }

    /**
     * @brief Sets the value of the element at the specified index using move semantics.
     * @tparam kIndex The index of the element to set.
     * @param _object The value to move into the tuple.
     */
    template<SizeType kIndex>
    FORCEINLINE constexpr Void Set(TupleObjectType_<kIndex>&& _object) noexcept {
        std::get<kIndex>(tuple_) = std::forward<TupleObjectType_<kIndex>>(_object);
    }

    /**
     * @brief Sets the value of the element of the specified type.
     * @tparam _ObjectType The type of the element to set.
     * @param _object The value to set.
     */
    template<typename _ObjectType>
    FORCEINLINE constexpr Void Set(const _ObjectType& _object) noexcept {
        std::get<_ObjectType>(tuple_) = _object;
    }

    /**
     * @brief Sets the value of the element of the specified type using move semantics.
     * @tparam _ObjectType The type of the element to set.
     * @param _object The value to move into the tuple.
     */
    template<typename _ObjectType>
    FORCEINLINE constexpr Void Set(_ObjectType&& _object) noexcept {
        std::get<_ObjectType>(tuple_) = std::forward<_ObjectType>(_object);
    }

    /**
     * @brief Applies a function to the elements of the tuple (L-value reference).
     * @tparam _Function The type of the function to apply.
     * @param func The function to apply.
     * @return The result of the function application.
     */
    template<typename _Function>
    NODISCARD FORCEINLINE constexpr decltype(auto) Apply(_Function&& func) & noexcept {
        return std::apply(std::forward<_Function>(func), tuple_);
    }

    /**
     * @brief Applies a function to the elements of the tuple (Const L-value reference).
     * @tparam _Function The type of the function to apply.
     * @param func The function to apply.
     * @return The result of the function application.
     */
    template<typename _Function>
    NODISCARD FORCEINLINE constexpr decltype(auto) Apply(_Function&& func) const& noexcept {
        return std::apply(std::forward<_Function>(func), tuple_);
    }

    /**
     * @brief Applies a function to the elements of the tuple (R-value reference).
     * @tparam _Function The type of the function to apply.
     * @param func The function to apply.
     * @return The result of the function application.
     */
    template<typename _Function>
    NODISCARD FORCEINLINE constexpr decltype(auto) Apply(_Function&& func) && noexcept {
        return std::apply(std::forward<_Function>(func), std::move(tuple_));
    }

    /**
     * @brief Swaps the contents of this tuple with another tuple.
     * @param _tuple The tuple to swap with.
     */
    FORCEINLINE constexpr Void Swap(TTuple& _tuple) noexcept {
        tuple_.swap(_tuple.tuple_);
    }

private:
    template<typename... _OtherArgsType>
    friend class TTuple;

private:
    /** @brief The underlying standard tuple storage. */
    STDTuple_ tuple_;
};

namespace tuple {

/**
 * @brief Creates a TTuple, deducing the target type from the types of arguments.
 * Performs type decay on the arguments (similar to std::make_tuple).
 * @tparam _ArgsType The types of the arguments.
 * @param _args The arguments to construct the tuple from.
 * @return A new TTuple containing the passed arguments.
 */
template<typename... _ArgsType>
NODISCARD FORCEINLINE constexpr TTuple<typename std::decay<_ArgsType>::type...> MakeTuple(
    _ArgsType&&... _args
) noexcept {
    return TTuple<typename std::decay<_ArgsType>::type...>(std::forward<_ArgsType>(_args)...);
}

/**
 * @brief Constructs a TTuple of references to the arguments.
 * @tparam _ArgsType The types of the arguments.
 * @param _args The arguments to forward into the tuple.
 * @return A TTuple containing forwarded references to the arguments.
 */
template<typename... _ArgsType>
NODISCARD FORCEINLINE constexpr TTuple<_ArgsType...> ForwardAsTuple(_ArgsType&&... _args) noexcept {
    return TTuple<_ArgsType...>(std::forward<_ArgsType>(_args)...);
}

/**
 * @brief Creates a TTuple of lvalue references to its arguments.
 * @tparam _ArgsType The types of the arguments.
 * @param _args The arguments to tie.
 * @return A TTuple containing lvalue references.
 */
template<typename... _ArgsType>
NODISCARD FORCEINLINE constexpr TTuple<_ArgsType&...> Tie(_ArgsType&... _args) noexcept {
    return TTuple<_ArgsType&...>(_args...);
}

/**
 * @brief Gets a reference to the element at the specified index from a tuple.
 * @tparam kIndex The index of the element to retrieve.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _tuple The tuple to retrieve from.
 * @return A reference to the element at the specified index.
 */
template<SizeType kIndex, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(TTuple<_ArgsType...>& _tuple) noexcept {
    return _tuple.Get<kIndex>();
}

/**
 * @brief Gets a const reference to the element at the specified index from a const tuple.
 * @tparam kIndex The index of the element to retrieve.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _tuple The tuple to retrieve from.
 * @return A const reference to the element at the specified index.
 */
template<SizeType kIndex, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(const TTuple<_ArgsType...>& _tuple) noexcept {
    return _tuple.Get<kIndex>();
}

/**
 * @brief Gets an rvalue reference to the element at the specified index from an rvalue tuple.
 * @tparam kIndex The index of the element to retrieve.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _tuple The tuple to retrieve from.
 * @return An rvalue reference to the element at the specified index.
 */
template<SizeType kIndex, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(TTuple<_ArgsType...>&& _tuple) noexcept {
    return std::move(_tuple).Get<kIndex>();
}

/**
 * @brief Gets an rvalue reference to the element at the specified index from a const rvalue tuple.
 * @tparam kIndex The index of the element to retrieve.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _tuple The tuple to retrieve from.
 * @return An rvalue reference to the element at the specified index.
 */
template<SizeType kIndex, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Get(const TTuple<_ArgsType...>&& _tuple) noexcept {
    return std::move(_tuple).Get<kIndex>();
}

/**
 * @brief Gets a reference to the element of the specified type from a tuple.
 * @tparam _ObjectType The type of the element to retrieve.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _tuple The tuple to retrieve from.
 * @return A reference to the element of the specified type.
 */
template<typename _ObjectType, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr _ObjectType& Get(TTuple<_ArgsType...>& _tuple) noexcept {
    return _tuple.Get<_ObjectType>();
}

/**
 * @brief Gets a const reference to the element of the specified type from a const tuple.
 * @tparam _ObjectType The type of the element to retrieve.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _tuple The tuple to retrieve from.
 * @return A const reference to the element of the specified type.
 */
template<typename _ObjectType, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr const _ObjectType& Get(const TTuple<_ArgsType...>& _tuple) noexcept {
    return _tuple.Get<_ObjectType>();
}

/**
 * @brief Gets an rvalue reference to the element of the specified type from an rvalue tuple.
 * @tparam _ObjectType The type of the element to retrieve.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _tuple The tuple to retrieve from.
 * @return An rvalue reference to the element of the specified type.
 */
template<typename _ObjectType, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr _ObjectType&& Get(TTuple<_ArgsType...>&& _tuple) noexcept {
    return std::move(_tuple).Get<_ObjectType>();
}

/**
 * @brief Gets an rvalue reference to the element of the specified type from a const rvalue tuple.
 * @tparam _ObjectType The type of the element to retrieve.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _tuple The tuple to retrieve from.
 * @return An rvalue reference to the element of the specified type.
 */
template<typename _ObjectType, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr const _ObjectType&& Get(const TTuple<_ArgsType...>&& _tuple) noexcept {
    return std::move(_tuple).Get<_ObjectType>();
}

/**
 * @brief Sets the value of the element at the specified index in the tuple.
 * @tparam kIndex The index of the element to set.
 * @tparam _ObjectType The type of the value to set.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _tuple Pointer to the tuple to modify.
 * @param _object The value to set.
 */
template<SizeType kIndex, typename _ObjectType, typename... _ArgsType>
FORCEINLINE constexpr Void Set(TTuple<_ArgsType...>* _tuple, _ObjectType&& _object) noexcept {
    _tuple->Set<kIndex>(std::forward<_ObjectType>(_object));
}

/**
 * @brief Sets the value of the element of the specified type in the tuple.
 * @tparam _ObjectType The type of the element to set.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _tuple Pointer to the tuple to modify.
 * @param _object The value to set.
 */
template<typename _ObjectType, typename... _ArgsType>
FORCEINLINE constexpr Void Set(TTuple<_ArgsType...>* _tuple, _ObjectType&& _object) noexcept {
    _tuple->Set<_ObjectType>(std::forward<_ObjectType>(_object));
}

/**
 * @brief Gets the size of the tuple.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _tuple The tuple to check.
 * @return The number of elements in the tuple.
 */
template<typename... _ArgsType>
NODISCARD FORCEINLINE constexpr const SizeType GetSize(const TTuple<_ArgsType...>& _tuple) noexcept {
    return _tuple.GetSize();
}

/**
 * @brief Applies a function to the elements of the tuple (L-value).
 * @tparam _Function The type of the function to apply.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _func The function to apply.
 * @param _tuple The tuple whose elements will be passed to the function.
 * @return The result of the function application.
 */
template<typename _Function, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Apply(_Function&& _func, TTuple<_ArgsType...>& _tuple) noexcept {
    return _tuple.Apply(std::forward<_Function>(_func));
}

/**
 * @brief Applies a function to the elements of the tuple (Const L-value).
 * @tparam _Function The type of the function to apply.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _func The function to apply.
 * @param _tuple The tuple whose elements will be passed to the function.
 * @return The result of the function application.
 */
template<typename _Function, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Apply(_Function&& _func, const TTuple<_ArgsType...>& _tuple) noexcept {
    return _tuple.Apply(std::forward<_Function>(_func));
}

/**
 * @brief Applies a function to the elements of the tuple (R-value).
 * @tparam _Function The type of the function to apply.
 * @tparam _ArgsType The types of the elements in the tuple.
 * @param _func The function to apply.
 * @param _tuple The tuple whose elements will be passed to the function.
 * @return The result of the function application.
 */
template<typename _Function, typename... _ArgsType>
NODISCARD FORCEINLINE constexpr decltype(auto) Apply(_Function&& _func, TTuple<_ArgsType...>&& _tuple) noexcept {
    return std::move(_tuple).Apply(std::forward<_Function>(_func));
}

} // tuple
} // zengine