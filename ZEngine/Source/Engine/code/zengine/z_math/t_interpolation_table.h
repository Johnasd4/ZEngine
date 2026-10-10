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

#include "../z_core/t_fixed_array.h"

namespace zengine {
namespace math {

/**
 * @brief A fixed-size interpolation table class for fast linear interpolation.
 *
 * This class implements a Look-Up Table (LUT) mechanism. It maps a continuous input value (position)
 * to a discrete index within a fixed array. The mapping is defined by a start offset and a step distance.
 * To optimize performance, the reciprocal of the step distance is stored to replace division with multiplication
 * during the sampling process.
 * 
 * The interpolation formula is: result = table[i] + (table[i+1] - table[i]) * factor.
 *
 * @tparam _NumberType The numeric type for data and calculations (must be a number type).
 * @tparam _TableSize The size of the fixed array (must be >= 2).
 */
template<typename _NumberType, SizeType _TableSize> 
class TInterpolationTable : public ZObject {
    STATIC_ASSERT(kIsNumber<_NumberType>, "TInterpolationTable: _NumberType must be an number type.");
    STATIC_ASSERT(_TableSize >= 2, "TInterpolationTable: Size must be at least 2 for interpolation.");
public:
    /** @brief Type alias for the base class. */
    using SuperType_ = ZObject;

public:
    /**
     * @brief Default constructor. Initializes offset to 0 and step distance to 1.
     */
    FORCEINLINE constexpr TInterpolationTable() noexcept 
        : SuperType_() 
        , table_()
        , index_offset_(static_cast<_NumberType>(0))
        , step_distance_reciprocal_(static_cast<_NumberType>(1))
    {}

    /**
     * @brief Copy constructor.
     * @param _table The source table to copy from.
     */
    FORCEINLINE constexpr TInterpolationTable(const TInterpolationTable& _table) noexcept
        : SuperType_(_table)
        , table_(_table.table_)
        , index_offset_(_table.index_offset_)
        , step_distance_reciprocal_(_table.step_distance_reciprocal_)
    {}

    /**
     * @brief Move constructor.
     * @param _table The source table to move from.
     */
    FORCEINLINE constexpr TInterpolationTable(TInterpolationTable&& _table) noexcept
        : SuperType_(std::move(_table))
        , table_(std::move(_table.table_))
        , index_offset_(_table.index_offset_)
        , step_distance_reciprocal_(_table.step_distance_reciprocal_)
    {}

    /**
     * @brief Constructor with initialization parameters.
     * @param _index_offset The starting value of the input domain.
     * @param _step_distance The distance between two consecutive indices in the input domain.
     */
    FORCEINLINE constexpr TInterpolationTable(_NumberType _index_offset, _NumberType _step_distance) noexcept
        : SuperType_()
        , table_()
        , index_offset_(_index_offset)
        , step_distance_reciprocal_(static_cast<_NumberType>(1) / _step_distance)
    {}

    /**
     * @brief Gets the current starting offset.
     * @return The index offset.
     */
    NODISCARD FORCEINLINE constexpr _NumberType GetIndexOffset() const noexcept {
        return index_offset_;
    }

    /**
     * @brief Gets the current step distance.
     * @return The step distance (calculated from the reciprocal).
     */
    NODISCARD FORCEINLINE constexpr _NumberType GetStepDistance() const noexcept {
        return static_cast<_NumberType>(1) / step_distance_reciprocal_;
    }

    /**
     * @brief Gets the reciprocal of the step distance.
     * @return The reciprocal value.
     */
    NODISCARD FORCEINLINE constexpr _NumberType GetStepDistanceReciprocal() const noexcept {
        return step_distance_reciprocal_;
    }

    /**
     * @brief Gets the size of the table.
     * @return The fixed size of the table.
     */
    NODISCARD FORCEINLINE constexpr SizeType GetSize() const noexcept {
        return _TableSize;
    }

    /**
     * @brief Sets the starting offset of the input domain.
     * @param _offset The new offset value.
     */
    FORCEINLINE constexpr Void SetIndexOffset(_NumberType _offset) noexcept {
        index_offset_ = _offset;
    }

    /**
     * @brief Sets the step distance and recalculates its reciprocal.
     * @param _distance The new step distance (must not be zero).
     */
    FORCEINLINE constexpr Void SetStepDistance(_NumberType _distance) noexcept {
        ASSERT(_distance != static_cast<_NumberType>(0), "TInterpolationTable: _distance cannot be zero.");
        step_distance_reciprocal_ = static_cast<_NumberType>(1) / _distance;
    }

    /**
     * @brief Accesses the table element at the specified index without bounds checking.
     * @param _index The index of the element to access.
     * @return A reference to the element at the specified index.
     */
    NODISCARD FORCEINLINE constexpr _NumberType& operator[](SizeType _index) noexcept {
        return table_[_index];
    }

    /**
     * @brief Accesses the table element at the specified index without bounds checking (const version).
     * @param _index The index of the element to access.
     * @return A const reference to the element at the specified index.
     */
    NODISCARD FORCEINLINE constexpr const _NumberType& operator[](SizeType _index) const noexcept {
        return table_[_index];
    }

    /**
     * @brief Accesses the table element at the specified index with bounds checking.
     * @param _index The index of the element to access.
     * @return A reference to the element at the specified index.
     */
    NODISCARD FORCEINLINE constexpr _NumberType& At(SizeType _index) { 
        ASSERT(_index < _TableSize, "TInterpolationTable: _index out of bounds!");
        return table_.At(_index); 
    }

    /**
     * @brief Accesses the table element at the specified index with bounds checking (const version).
     * @param _index The index of the element to access.
     * @return A const reference to the element at the specified index.
     */
    NODISCARD FORCEINLINE constexpr const _NumberType& At(SizeType _index) const { 
        ASSERT(_index < _TableSize, "TInterpolationTable: _index out of bounds!");
        return table_.At(_index); 
    }

    /**
     * @brief Performs linear interpolation at the specified position.
     * 
     * Calculates the table index based on the offset and step distance, clamps the position to the valid range,
     * and interpolates between the two nearest table values.
     * 
     * @param _pos The input position to sample.
     * @return The interpolated value.
     */
    NODISCARD FORCEINLINE constexpr _NumberType operator()(_NumberType _pos) const noexcept {
        _NumberType table_pos = (_pos - index_offset_) * step_distance_reciprocal_;

        // Clamp table_pos to [0, _TableSize - 1]
        if (table_pos <= 0) {
            return table_[0];
        }
        else if (table_pos >= static_cast<_NumberType>(_TableSize - 1)) {
            return table_[_TableSize - 1];
        }

        SizeType array_index = static_cast<SizeType>(table_pos);
        _NumberType factor = table_pos - static_cast<_NumberType>(array_index);

        return table_[array_index] + (table_[array_index + 1] - table_[array_index]) * factor;
    }

    /**
     * @brief Alias for operator() to sample the table.
     * @param _pos The input position to sample.
     * @return The interpolated value.
     */
    NODISCARD FORCEINLINE constexpr _NumberType ComputeSample(_NumberType _pos) const noexcept {
        return operator()(_pos);
    }

private:
    /** @brief The fixed-size array storing the table data. */
    TFixedArray<_NumberType, _TableSize> table_;
    /** @brief The starting offset of the input domain. */
    _NumberType index_offset_;
    /** @brief The reciprocal of the step distance (1 / step) for fast calculation. */
    _NumberType step_distance_reciprocal_;
};

} // math
} // zengine