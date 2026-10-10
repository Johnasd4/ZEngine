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

#include "internal/t_order_data_iterator.h"

namespace zengine {

/**
 * @brief Array view, initialized with an object pointer.
 * @details This is only a view, it does not apply for or own heap memory.
 *          It acts as a lightweight window into an existing memory buffer.
 * @tparam _ObjectType The type of the elements in the view.
 */
template<typename _ObjectType>
class TArrayView {
    NO_NEW_DELETE
public:
    /** @brief The constant forward iterator type. */
    using ConstIterator_ = internal::TOrderDataConstIterator<_ObjectType>;
    /** @brief The constant reverse iterator type. */
    using ConstReverseIterator_ = internal::TOrderDataConstReverseIterator<_ObjectType>;

    /** @brief Default constructor. Initializes an empty view. */
    FORCEINLINE TArrayView() noexcept
        : data_ptr_(nullptr)
        , size_(0ULL)
    {}

    /**
     * @brief Copy constructor.
     * @param _view The view to copy from.
     */
    FORCEINLINE TArrayView(const TArrayView& _view) noexcept
        : data_ptr_(_view.data_ptr_)
        , size_(_view.size_)
    {}

    /**
     * @brief Constructor from pointer and size.
     * @param _data_ptr Pointer to the data buffer.
     * @param _size Number of elements in the buffer.
     */
    FORCEINLINE TArrayView(_ObjectType* _data_ptr, SizeType _size) noexcept
        : data_ptr_(_data_ptr)
        , size_(_size)
    {}

    /** @brief Destructor. */
    FORCEINLINE ~TArrayView() noexcept = default;

    /**
     * @brief Assignment operator.
     * @param _view The view to assign from.
     * @return A reference to this view.
     */
    FORCEINLINE TArrayView& operator=(const TArrayView& _view) noexcept {
        data_ptr_ = _view.data_ptr_;
        size_ = _view.size_;
        return *this;
    }

    /**
     * @brief Reassigns the view to a new memory range.
     * @param _data_ptr Pointer to the new data buffer.
     * @param _size Number of elements in the new buffer.
     */
    FORCEINLINE Void Assign(_ObjectType* _data_ptr, SizeType _size) noexcept {
        data_ptr_ = _data_ptr;
        size_ = _size;
    }

    /**
     * @brief Check if the view is empty.
     * @return True if size is 0, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { return size_ == 0ULL; }

    /**
     * @brief Get the raw data pointer.
     * @return A const pointer to the start of the data.
     */
    NODISCARD FORCEINLINE const _ObjectType* GetDataPtr() const noexcept { return data_ptr_; }

    /**
     * @brief Get the size of the view.
     * @return The number of elements in the view.
     */
    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return size_; }

    /**
     * @brief Checks if two views point to the exact same memory range.
     * @param _view The view to compare with.
     * @return True if data pointer and size are identical, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator==(const TArrayView& _view) noexcept {
        return data_ptr_ == _view.data_ptr_ && size_ == _view.size_;
    }

    /**
     * @brief Checks if two views do not point to the exact same memory range.
     * @param _view The view to compare with.
     * @return True if data pointer or size are different, false otherwise.
     */
    NODISCARD FORCEINLINE Bool operator!=(const TArrayView& _view) noexcept {
        return data_ptr_ != _view.data_ptr_ || size_ != _view.size_;
    }

    /**
     * @brief Access element by index (no bounds check).
     * @param _index The index of the element to access.
     * @return A const reference to the element at the specified index.
     */
    NODISCARD FORCEINLINE const _ObjectType& operator[](SizeType _index) const noexcept { return data_ptr_[_index]; }

    /**
     * @brief Access element by index (same as operator[] for view).
     * @param _index The index of the element to access.
     * @return A const reference to the element at the specified index.
     */
    NODISCARD FORCEINLINE const _ObjectType& At(SizeType _index) const noexcept { return data_ptr_[_index]; }

    /**
     * @brief Access the first element.
     * @return A const reference to the first element.
     */
    NODISCARD FORCEINLINE const _ObjectType& Front() const noexcept { return data_ptr_[0ULL]; }

    /**
     * @brief Access the last element.
     * @return A const reference to the last element.
     */
    NODISCARD FORCEINLINE const _ObjectType& Back() const noexcept { return data_ptr_[size_ - 1ULL]; }

    /**
     * @brief Get iterator to the beginning.
     * @return An iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept {
        return ConstIterator_(data_ptr_);
    }

    /**
     * @brief Get const iterator to the beginning.
     * @return A const iterator to the first element.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept {
        return ConstIterator_(data_ptr_);
    }

    /**
     * @brief Get reverse iterator to the beginning (points to the last element).
     * @return A reverse iterator to the first element of the reversed view.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept {
        return ConstReverseIterator_(data_ptr_ + size_);
    }

    /**
     * @brief Get const reverse iterator to the beginning.
     * @return A const reverse iterator to the first element of the reversed view.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept {
        return ConstReverseIterator_(data_ptr_ + size_);
    }

    /**
     * @brief Get iterator to the end.
     * @return An iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept {
        return ConstIterator_(data_ptr_ + size_);
    }

    /**
     * @brief Get const iterator to the end.
     * @return A const iterator to the element following the last element.
     */
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept {
        return ConstIterator_(data_ptr_ + size_);
    }

    /**
     * @brief Get reverse iterator to the end (sentinel).
     * @return A reverse iterator to the element following the last element of the reversed view.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept {
        return ConstReverseIterator_(data_ptr_);
    }

    /**
     * @brief Get const reverse iterator to the end.
     * @return A const reverse iterator to the element following the last element of the reversed view.
     */
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept {
        return ConstReverseIterator_(data_ptr_);
    }

public:
    /** @brief Alias for the Begin() function, used for stl compatibility. */
    NODISCARD FORCEINLINE ConstIterator_ begin() const noexcept { return Begin(); }
    /** @brief Alias for the End() function, used for stl compatibility. */
    NODISCARD FORCEINLINE ConstIterator_ end() const noexcept { return End(); }

private:
    /** @brief Pointer to the start of the data. */
    const _ObjectType* data_ptr_;
    /** @brief Number of elements in the view. */
    SizeType size_;
};

} // zengine