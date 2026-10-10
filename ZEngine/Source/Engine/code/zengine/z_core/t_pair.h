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

#include "z_object.h"

namespace zengine {

/**
 * @brief A wrapper class for std::pair that inherits from ZObject.
 *
 * This class serves as a container holding two heterogeneous objects. It delegates storage and operations to an
 * underlying std::pair while inheriting from ZObject to ensure compatibility with the ZEngine's object hierarchy and
 * memory management system. It supports various constructors and assignment operators for seamless interoperability
 * between TPair and std::pair.
 *
 * @tparam _ObjectType1 The type of the first element.
 * @tparam _ObjectType2 The type of the second element.
 */
template<typename _ObjectType1, typename _ObjectType2>
class TPair : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;
    /** @brief Alias for the underlying standard pair type. */
    using STDPair_ = std::pair<_ObjectType1, _ObjectType2>;

public:
    /**
     * @brief Default constructor.
     * Initializes the pair with default constructed elements.
     */
    FORCEINLINE constexpr TPair() noexcept : SuperType_() {}

    /**
     * @brief Copy constructor.
     * @param _pair The pair to copy from.
     */
    FORCEINLINE constexpr TPair(const TPair<_ObjectType1, _ObjectType2>& _pair) noexcept
        : SuperType_(_pair)
        , pair_(_pair.pair_)
    {}

    /**
     * @brief Move constructor.
     * @param _pair The pair to move from.
     */
    FORCEINLINE constexpr TPair(TPair<_ObjectType1, _ObjectType2>&& _pair) noexcept
        : SuperType_(std::move(_pair))
        , pair_(std::move(_pair.pair_))
    {}

    /**
     * @brief Copy constructor from a TPair with different template types.
     * @tparam _OtherObjectType1 Type of the first element in the source pair.
     * @tparam _OtherObjectType2 Type of the second element in the source pair.
     * @param _pair The pair to copy from.
     */
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    FORCEINLINE constexpr TPair(const TPair<_OtherObjectType1, _OtherObjectType2>& _pair) noexcept
        : SuperType_(_pair)
        , pair_(_pair.pair_) 
    {}

    /**
     * @brief Move constructor from a TPair with different template types.
     * @tparam _OtherObjectType1 Type of the first element in the source pair.
     * @tparam _OtherObjectType2 Type of the second element in the source pair.
     * @param _pair The pair to move from.
     */
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    FORCEINLINE constexpr TPair(TPair<_OtherObjectType1, _OtherObjectType2>&& _pair) noexcept
        : SuperType_(std::move(_pair))
        , pair_(std::move(_pair.pair_))
    {}

    /**
     * @brief Constructor from a standard pair with different template types (Copy).
     * @tparam _OtherObjectType1 Type of the first element in the source pair.
     * @tparam _OtherObjectType2 Type of the second element in the source pair.
     * @param _pair The standard pair to copy from.
     */
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    FORCEINLINE constexpr TPair(const std::pair<_OtherObjectType1, _OtherObjectType2>& _pair) noexcept
        : SuperType_()
        , pair_(_pair) 
    {}

    /**
     * @brief Constructor from a standard pair with different template types (Move).
     * @tparam _OtherObjectType1 Type of the first element in the source pair.
     * @tparam _OtherObjectType2 Type of the second element in the source pair.
     * @param _pair The standard pair to move from.
     */
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    FORCEINLINE constexpr TPair(std::pair<_OtherObjectType1, _OtherObjectType2>&& _pair) noexcept
        : SuperType_()
        , pair_(std::move(_pair))
    {}

    /**
     * @brief Constructor from a standard pair (Copy).
     * @param _pair The standard pair to copy from.
     */
    FORCEINLINE constexpr TPair(const std::pair<_ObjectType1, _ObjectType2>& _pair) noexcept
        : SuperType_()
        , pair_(_pair) 
    {}

    /**
     * @brief Constructor from a standard pair (Move).
     * @param _pair The standard pair to move from.
     */
    FORCEINLINE constexpr TPair(std::pair<_ObjectType1, _ObjectType2>&& _pair) noexcept
        : SuperType_()
        , pair_(std::move(_pair))
    {}

    /**
     * @brief Constructor from two individual elements (Copy).
     * @param _obj_1 The first object to copy.
     * @param _obj_2 The second object to copy.
     */
    FORCEINLINE constexpr TPair(const _ObjectType1& _obj_1, const _ObjectType2& _obj_2) noexcept
        : SuperType_()
        , pair_(_obj_1, _obj_2)
    {}

    /**
     * @brief Constructor from two individual elements (Move).
     * @param _obj_1 The first object to move.
     * @param _obj_2 The second object to move.
     */
    FORCEINLINE constexpr TPair(_ObjectType1&& _obj_1, _ObjectType2&& _obj_2) noexcept
        : SuperType_()
        , pair_(std::move(_obj_1), std::move(_obj_2))
    {}

    /**
     * @brief Constructs a pair from two const references with different types.
     * @tparam _OtherObjectType1 Type of the first object.
     * @tparam _OtherObjectType2 Type of the second object.
     * @param _obj_1 The first object to copy.
     * @param _obj_2 The second object to copy.
     */
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    FORCEINLINE constexpr TPair(const _OtherObjectType1& _obj_1, const _OtherObjectType2& _obj_2) noexcept
        : SuperType_()
        , pair_(_obj_1, _obj_2)
    {}

    /**
     * @brief Constructs a pair from two forwarding references with different types.
     * @tparam _OtherObjectType1 Type of the first object.
     * @tparam _OtherObjectType2 Type of the second object.
     * @param _obj_1 The first object to move or copy.
     * @param _obj_2 The second object to move or copy.
     */
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    FORCEINLINE constexpr TPair(_OtherObjectType1&& _obj_1, _OtherObjectType2&& _obj_2) noexcept
        : SuperType_()
        , pair_(std::move(_obj_1), std::move(_obj_2))
    {}

    /** @brief Default destructor. */
    FORCEINLINE constexpr ~TPair() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @tparam _OtherObjectType1 Type of the first element in the source pair.
     * @tparam _OtherObjectType2 Type of the second element in the source pair.
     * @param _pair The pair to copy from.
     * @return A reference to this pair.
     */
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    FORCEINLINE constexpr TPair& operator=(const TPair<_OtherObjectType1, _OtherObjectType2>& _pair) noexcept {
        SuperType_::operator=(_pair);
        pair_ = _pair.pair_;
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @tparam _OtherObjectType1 Type of the first element in the source pair.
     * @tparam _OtherObjectType2 Type of the second element in the source pair.
     * @param _pair The pair to move from.
     * @return A reference to this pair.
     */
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    FORCEINLINE constexpr TPair& operator=(TPair<_OtherObjectType1, _OtherObjectType2>&& _pair) noexcept {
        SuperType_::operator=(std::move(_pair));
        pair_ = std::move(_pair.pair_);
        return *this;
    }

    /**
     * @brief Copy assignment operator from a standard pair.
     * @tparam _OtherObjectType1 Type of the first element in the source pair.
     * @tparam _OtherObjectType2 Type of the second element in the source pair.
     * @param _pair The standard pair to copy from.
     * @return A reference to this pair.
     */
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    FORCEINLINE constexpr TPair& operator=(const std::pair<_ObjectType1, _ObjectType2>& _pair) noexcept {
        pair_ = _pair;
        return *this;
    }

    /**
     * @brief Move assignment operator from a standard pair.
     * @tparam _OtherObjectType1 Type of the first element in the source pair.
     * @tparam _OtherObjectType2 Type of the second element in the source pair.
     * @param _pair The standard pair to move from.
     * @return A reference to this pair.
     */
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    FORCEINLINE constexpr TPair& operator=(std::pair<_ObjectType1, _ObjectType2>&& _pair) noexcept {
        pair_ = std::move(_pair);
        return *this;
    }

    /**
     * @brief Three-way comparison operator (spaceship operator).
     * @tparam _OtherObjectType1 Type of the first element in the other pair.
     * @tparam _OtherObjectType2 Type of the second element in the other pair.
     * @param _pair The other pair to compare with.
     * @return The result of the comparison between the underlying pairs.
     */
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    FORCEINLINE auto operator<=>(const TPair<_OtherObjectType1, _OtherObjectType2>& _pair) const {
        return pair_ <=> _pair.pair_;
    }

    /**
     * @brief Conversion operator to the underlying standard pair.
     * @return A copy of the underlying std::pair.
     */
    NODISCARD FORCEINLINE operator STDPair_() noexcept {
        return pair_;
    }

    /**
     * @brief Conversion operator to the underlying standard pair (const).
     * @return A copy of the underlying std::pair.
     */
    NODISCARD FORCEINLINE operator const STDPair_() const noexcept {
        return pair_;
    }

    /**
     * @brief Accesses the first element of the pair.
     * @return A reference to the first element.
     */
    NODISCARD FORCEINLINE _ObjectType1& First() noexcept { return pair_.first; }

    /**
     * @brief Accesses the first element of the pair (const).
     * @return A const reference to the first element.
     */
    NODISCARD FORCEINLINE const _ObjectType1& First() const noexcept { return pair_.first; }

    /**
     * @brief Accesses the second element of the pair.
     * @return A reference to the second element.
     */
    NODISCARD FORCEINLINE _ObjectType2& Second() noexcept { return pair_.second; }

    /**
     * @brief Accesses the second element of the pair (const).
     * @return A const reference to the second element.
     */
    NODISCARD FORCEINLINE const _ObjectType2& Second() const noexcept { return pair_.second; }

    /**
     * @brief Accesses the underlying standard pair.
     * @return A reference to the underlying std::pair.
     */
    NODISCARD FORCEINLINE STDPair_& STDPair() noexcept { return pair_; }

    /**
     * @brief Accesses the underlying standard pair (const).
     * @return A const reference to the underlying std::pair.
     */
    NODISCARD FORCEINLINE const STDPair_& STDPair() const noexcept { return pair_; }

    /**
     * @brief Gets the element at the specified index from the pair.
     * @tparam _Index The index of the element to retrieve (0 or 1).
     * @param _pair The pair to retrieve the element from.
     * @return A reference to the element at the specified index.
     */
    template <std::size_t _Index>
    friend constexpr decltype(auto) get(TPair& _pair) noexcept {
        return std::get<_Index>(_pair.pair_);
    }

    /**
     * @brief Gets the element at the specified index from the pair (const).
     * @tparam _Index The index of the element to retrieve (0 or 1).
     * @param _pair The pair to retrieve the element from.
     * @return A const reference to the element at the specified index.
     */
    template <std::size_t _Index>
    friend constexpr decltype(auto) get(const TPair& _pair) noexcept {
        return std::get<_Index>(_pair.pair_);
    }

    /**
     * @brief Gets the element at the specified index from the pair (rvalue).
     * @tparam _Index The index of the element to retrieve (0 or 1).
     * @param _pair The pair to retrieve the element from.
     * @return A rvalue reference to the element at the specified index.
     */
    template <std::size_t _Index>
    friend constexpr decltype(auto) get(TPair&& _pair) noexcept {
        return std::get<_Index>(std::move(_pair.pair_));
    }

    /**
     * @brief Gets the element at the specified index from the pair (const rvalue).
     * @tparam _Index The index of the element to retrieve (0 or 1).
     * @param _pair The pair to retrieve the element from.
     * @return A const rvalue reference to the element at the specified index.
     */
    template <std::size_t _Index>
    friend constexpr decltype(auto) get(const TPair&& _pair) noexcept {
        return std::get<_Index>(std::move(_pair.pair_));
    }

private:
    /**
     * @brief Friend declaration to allow different template instantiations of TPair to access private members.
     * @tparam _OtherObjectType1 Type of the first element of the other pair.
     * @tparam _OtherObjectType2 Type of the second element of the other pair.
     */
    template<typename _OtherObjectType1, typename _OtherObjectType2>
    friend class TPair;

private:
    /** @brief The underlying standard pair storage. */
    STDPair_ pair_;
};

} // zengine
namespace std {

template<typename _ObjectType1, typename _ObjectType2>
struct tuple_size<zengine::TPair<_ObjectType1, _ObjectType2>>
    : integral_constant<std::size_t, 2> 
{};

template<std::size_t _Index, typename _ObjectType1, typename _ObjectType2>
struct tuple_element<_Index, zengine::TPair<_ObjectType1, _ObjectType2>>
    : tuple_element<_Index, std::pair<_ObjectType1, _ObjectType2>> 
{};

} // std