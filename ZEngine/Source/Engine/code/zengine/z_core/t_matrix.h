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

#include "f_memory_pool.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief A container class representing a 2D matrix.
 *
 * This class manages a contiguous block of memory to store elements in a row-major layout.
 * It provides dynamic sizing and memory management using the ZEngine memory pool system.
 * The container handles the construction and destruction of elements, optimizing for simple
 * types (POD) when _IfCallConstructorAndDestructor is false.
 *
 * @tparam _ObjectType The type of the elements stored in the matrix.
 * @tparam _IsGlobal Specifies whether to use the global memory pool (true) or a local/thread-specific one (false).
 * @tparam _IfCallConstructorAndDestructor Flag to determine if constructors and destructors should be explicitly called.
 */
template<
    typename _ObjectType, 
    Bool _IsGlobal = DEFAULT_USE_GLOBAL_MEMORY_POOL,
    Bool _IfCallConstructorAndDestructor = !kIsTriviallyCopyable<_ObjectType>
>
class TMatrix : public ZObject {
public:
    /** @brief Alias for the base class ZObject. */
    using SuperType_ = ZObject;

public:
    /**
     * @brief Default constructor.
     *
     * Initializes an empty matrix with zero rows, zero columns, and no memory allocation.
     */
    TMatrix() noexcept 
        : SuperType_()
        , data_ptr_(nullptr)
        , row_(0ULL)
        , column_(0ULL) 
    {}

    /**
     * @brief Copy constructor.
     *
     * Creates a deep copy of the specified matrix. Allocates new memory and copies all elements
     * from the source matrix.
     *
     * @param _matrix The source matrix to copy from.
     */
    TMatrix(const TMatrix& _matrix) noexcept 
        : SuperType_()
        , data_ptr_(reinterpret_cast<_ObjectType*>(memory_pool::ApplyMemory<_IsGlobal>(
            _matrix.row_* _matrix.column_ * sizeof(_ObjectType)
        )))
        , row_(_matrix.row_)
        , column_(_matrix.column_)
    {
        if constexpr (_IfCallConstructorAndDestructor) {
            SizeType obj_num = row_ * column_;
            for (SizeType index = 0; index < obj_num; ++index) {
                new (data_ptr_ + index) _ObjectType(_matrix.data_ptr_[index]);
            }
        }
        else {
            Copy(data_ptr_, _matrix.data_ptr_, row_ * column_ * sizeof(_ObjectType));
        }
    }

    /**
     * @brief Move constructor.
     *
     * Transfers ownership of the memory and data from the source matrix to this matrix.
     * The source matrix is reset to an empty state.
     *
     * @param _matrix The source matrix to move from.
     */
    TMatrix(TMatrix&& _matrix) noexcept
        : SuperType_()
        , data_ptr_(_matrix.data_ptr_)
        , row_(_matrix.row_)
        , column_(_matrix.column_)
    {
        _matrix.row_ = 0ULL;
        _matrix.column_ = 0ULL;
        _matrix.data_ptr_ = nullptr;
    }

    /**
     * @brief Constructor with dimensions.
     *
     * Allocates memory for the specified number of rows and columns and default constructs the elements.
     *
     * @param _row The number of rows.
     * @param _column The number of columns.
     */
    TMatrix(SizeType _row, SizeType _column) noexcept
        : SuperType_()
        , data_ptr_(reinterpret_cast<_ObjectType*>(memory_pool::ApplyMemory<_IsGlobal>(
            _row * _column * sizeof(_ObjectType)
        )))
        , row_(_row)
        , column_(_column)
    {
        if constexpr (_IfCallConstructorAndDestructor) {
            SizeType obj_num = row_ * column_;
            for (SizeType index = 0; index < obj_num; ++index) {
                new (data_ptr_ + index) _ObjectType();
            }
        }
    }

    /**
     * @brief Constructor with dimensions and initial value.
     *
     * Allocates memory for the specified number of rows and columns and initializes all elements with the given value.
     *
     * @param _row The number of rows.
     * @param _column The number of columns.
     * @param _val The initial value to assign to all elements.
     */
    TMatrix(SizeType _row, SizeType _column, const _ObjectType& _val) noexcept
        : SuperType_()
        , data_ptr_(reinterpret_cast<_ObjectType*>(memory_pool::ApplyMemory<_IsGlobal>(
            _row * _column * sizeof(_ObjectType)
        )))
        , row_(_row)
        , column_(_column)
    {
        SizeType obj_num = row_ * column_;
        for (SizeType index = 0; index < obj_num; ++index) {
            if constexpr (_IfCallConstructorAndDestructor) {
                new (data_ptr_ + index) _ObjectType(_val);
            }
            else {
                data_ptr_[index] = _val;
            }
        }
    }

    /**
     * @brief Destructor.
     *
     * Destroys all elements (if required) and releases the allocated memory back to the memory pool.
     */
    ~TMatrix() noexcept {
        // destruct old data
        if constexpr (_IfCallConstructorAndDestructor) {
            SizeType obj_num = row_ * column_;
            for (SizeType index = 0; index < obj_num; ++index) {
                data_ptr_[index].~_ObjectType();
            }
        }
        memory_pool::ReleaseMemory<_IsGlobal>(data_ptr_);
    }

    /**
     * @brief Copy assignment operator.
     *
     * Destroys the current content, reallocates memory if necessary, and copies elements from the source matrix.
     *
     * @param _matrix The source matrix to copy from.
     * @return TMatrix& A reference to this matrix.
     */
    TMatrix& operator=(const TMatrix& _matrix) noexcept {
        if (this == &_matrix) {
            return *this;
        }
        SuperType_::operator=(_matrix);

        // destruct old data
        if constexpr (_IfCallConstructorAndDestructor) {
            SizeType old_num = row_ * column_;
            for (SizeType index = 0; index < old_num; ++index) {
                data_ptr_[index].~_ObjectType();
            }
        }

        SizeType src_matrix_size = _matrix.row_ * _matrix.column_ * sizeof(_ObjectType);
        data_ptr_ = reinterpret_cast<_ObjectType*>(memory_pool::ReapplyNoCopyMemory<_IsGlobal>(
            data_ptr_, src_matrix_size
        ));
        row_ = _matrix.row_;
        column_ = _matrix.column_;

        // construct new data
        if constexpr (_IfCallConstructorAndDestructor) {
            SizeType obj_num = row_ * column_;

            for (SizeType index = 0; index < obj_num; ++index) {
                new (data_ptr_ + index) _ObjectType(_matrix.data_ptr_[index]);
            }
        }
        else {
            Copy(data_ptr_, _matrix.data_ptr_, src_matrix_size);
        }
        return *this;
    }

    /**
     * @brief Move assignment operator.
     *
     * Destroys the current content and takes ownership of the source matrix's memory and data.
     *
     * @param _matrix The source matrix to move from.
     * @return TMatrix& A reference to this matrix.
     */
    TMatrix& operator=(TMatrix&& _matrix) noexcept {
        if (this == &_matrix) {
            return *this;
        }
        SuperType_::operator=(std::move(_matrix));

        // Destruct old data
        if constexpr (_IfCallConstructorAndDestructor) {
            SizeType old_num = row_ * column_;
            for (SizeType index = 0; index < old_num; ++index) {
                data_ptr_[index].~_ObjectType();
            }
        }

        memory_pool::ReleaseMemory<_IsGlobal>(data_ptr_);

        data_ptr_ = _matrix.data_ptr_;
        row_ = _matrix.row_;
        column_ = _matrix.column_;
        _matrix.data_ptr_ = nullptr;
        _matrix.row_ = 0ULL;
        _matrix.column_ = 0ULL;

        return *this;
    }

    /**
     * @brief Gets the raw data pointer of the matrix.
     *
     * @return _ObjectType* Pointer to the beginning of the memory block.
     */
    NODISCARD FORCEINLINE _ObjectType* GetDataPtr() noexcept { return data_ptr_; }

    /**
     * @brief Gets the const raw data pointer of the matrix.
     *
     * @return const _ObjectType* Const pointer to the beginning of the memory block.
     */
    NODISCARD FORCEINLINE const _ObjectType* GetDataPtr() const noexcept { return data_ptr_; }

    /**
     * @brief Equality comparison operator.
     *
     * Checks if the data pointer is identical to the other matrix's data pointer.
     * Note: This checks for pointer identity, not content equality.
     *
     * @param _matrix The matrix to compare with.
     * @return Bool True if both matrices point to the same memory address, False otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TMatrix& _matrix) noexcept { return data_ptr_ == _matrix.data_ptr_; }

    /**
     * @brief Inequality comparison operator.
     *
     * Checks if the data pointer is different from the other matrix's data pointer.
     *
     * @param _matrix The matrix to compare with.
     * @return Bool True if the matrices point to different memory addresses, False otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TMatrix& _matrix) noexcept { return data_ptr_ != _matrix.data_ptr_; }

    /**
     * @brief Function call operator to access elements.
     *
     * @param _row The row index of the element.
     * @param _column The column index of the element.
     * @return _ObjectType& A reference to the element at the specified position.
     */
    NODISCARD FORCEINLINE _ObjectType& operator()(SizeType _row, SizeType _column) noexcept { 
        ASSERT(_row < row_, "TMatrix: _row out of bounds!");
        ASSERT(_column < column_, "TMatrix: _column out of bounds!");
        return data_ptr_[_row * column_ + _column];
    }

    /**
     * @brief Const function call operator to access elements.
     *
     * @param _row The row index of the element.
     * @param _column The column index of the element.
     * @return const _ObjectType& A const reference to the element at the specified position.
     */
    NODISCARD FORCEINLINE const _ObjectType& operator()(SizeType _row, SizeType _column) const noexcept {
        ASSERT(_row < row_, "TMatrix: _row out of bounds!");
        ASSERT(_column < column_, "TMatrix: _column out of bounds!");
        return data_ptr_[_row * column_ + _column];
    }

    /**
     * @brief Access element at specified position.
     *
     * @param _row The row index of the element.
     * @param _column The column index of the element.
     * @return _ObjectType& A reference to the element at the specified position.
     */
    NODISCARD FORCEINLINE _ObjectType& At(SizeType _row, SizeType _column) {
        if (_row < row_ || _column < column_) {
            throw std::out_of_range("TMatrix: _row or _column out of logical bounds");
        }
        return data_ptr_[_row * column_ + _column];
    }

    /**
     * @brief Const access element at specified position.
     *
     * @param _row The row index of the element.
     * @param _column The column index of the element.
     * @return const _ObjectType& A const reference to the element at the specified position.
     */
    NODISCARD FORCEINLINE const _ObjectType& At(SizeType _row, SizeType _column) const {
        if (_row < row_ || _column < column_) {
            throw std::out_of_range("TMatrix: _row or _column out of logical bounds");
        }
        return data_ptr_[_row * column_ + _column];
    }

    /**
     * @brief Gets the number of rows in the matrix.
     *
     * @return SizeType The row count.
     */
    NODISCARD FORCEINLINE SizeType Row() const noexcept { return row_; }

    /**
     * @brief Gets the number of columns in the matrix.
     *
     * @return SizeType The column count.
     */
    NODISCARD FORCEINLINE SizeType Column() const noexcept { return column_; }

    /**
     * @brief Gets the total number of elements in the matrix.
     *
     * @return SizeType The product of rows and columns.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return row_ * column_; }

    /**
     * @brief Gets the capacity of the matrix.
     *
     * @return SizeType The current capacity (same as size for this implementation).
     */
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return row_ * column_; }

    /**
     * @brief Constructs an element in-place at the specified position.
     *
     * Uses perfect forwarding to construct the object directly at the memory location.
     *
     * @tparam _ArgsType Parameter types for the object's constructor.
     * @param _row The row index where the object will be constructed.
     * @param _column The column index where the object will be constructed.
     * @param _args The arguments to pass to the object's constructor.
     * @return Void.
     */
    template <typename... _ArgsType>
    FORCEINLINE Void Emplace(SizeType _row, SizeType _column, _ArgsType&&... _args) noexcept {
        data_ptr_[_row * column_ + _column] = _ObjectType(std::forward<_ArgsType>(_args)...);
    }

    /**
     * @brief Resets the matrix to a new size with default-constructed elements.
     *
     * Destroys existing data, reallocates memory for the new dimensions, and default-constructs new elements.
     *
     * @param _row The new number of rows.
     * @param _column The new number of columns.
     * @return Void.
     */
    Void Reset(SizeType _row, SizeType _column) noexcept {
        // Destruct old data
        if constexpr (_IfCallConstructorAndDestructor) {
            SizeType old_num = row_ * column_;
            for (SizeType index = 0; index < old_num; ++index) {
                data_ptr_[index].~_ObjectType();
            }
        }

        data_ptr_ = reinterpret_cast<_ObjectType*>(memory_pool::ReapplyNoCopyMemory<_IsGlobal>(
            data_ptr_, _row * _column * sizeof(_ObjectType)
        ));
        row_ = _row;
        column_ = _column;

        // Construct new data (Default Construct)
        if constexpr (_IfCallConstructorAndDestructor) {
            SizeType obj_num = row_ * column_;
            for (SizeType index = 0; index < obj_num; ++index) {
                new (data_ptr_ + index) _ObjectType();
            }
        }
    }

    /**
     * @brief Resets the matrix to a new size and initializes elements with a specific value.
     *
     * Destroys existing data, reallocates memory, and initializes all new elements as copies of the provided value.
     *
     * @param _row The new number of rows.
     * @param _column The new number of columns.
     * @param _val The value to initialize the new elements with.
     * @return Void.
     */
    Void Reset(SizeType _row, SizeType _column, const _ObjectType& _val) noexcept {
        // Destruct old data
        if constexpr (_IfCallConstructorAndDestructor) {
            SizeType old_num = row_ * column_;
            for (SizeType index = 0; index < old_num; ++index) {
                data_ptr_[index].~_ObjectType();
            }
        }

        data_ptr_ = reinterpret_cast<_ObjectType*>(memory_pool::ReapplyNoCopyMemory<_IsGlobal>(
            data_ptr_, _row * _column * sizeof(_ObjectType)
        ));
        row_ = _row;
        column_ = _column;

        // Construct new data (Copy Construct from val)
        SizeType obj_num = row_ * column_;
        for (SizeType index = 0; index < obj_num; ++index) {
            if constexpr (_IfCallConstructorAndDestructor) {
                new (data_ptr_ + index) _ObjectType(_val);
            }
            else {
                data_ptr_[index] = _val;
            }
        }
    }

    /**
     * @brief Swaps the content of this matrix with another matrix.
     *
     * @param _matrix The other matrix to swap with.
     * @return Void.
     */
    FORCEINLINE Void Swap(TMatrix& _matrix) noexcept {
        ::zengine::Swap(this, &_matrix);
    }

private:
    /** @brief Pointer to the contiguous memory block storing the matrix elements. */
    _ObjectType* data_ptr_;
    /** @brief The number of rows in the matrix. */
    SizeType row_;
    /** @brief The number of columns in the matrix. */
    SizeType column_;
};

} // zengine