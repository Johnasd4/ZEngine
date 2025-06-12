/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "internal/z_drive.h"

#include "t_vector.h"
#include "z_object.h"

namespace zengine {

/*
    Vector caintainer.
*/
template<typename _ObjectType>
class TMatrix : public ZObject {
public:
    using DataVector_ = TVector<_ObjectType>;
    using Iterator_ = DataVector_::Iterator_;
    using ConstIterator_ = DataVector_::ConstIterator_;
    using ReverseIterator_ = DataVector_::ReverseIterator_;
    using ConstReverseIterator_ = DataVector_::ConstReverseIterator_;

    TMatrix() noexcept : SuperType_(), data_vec_(), row_(0LL), column_(0LL) {}
    TMatrix(const TMatrix& _matrix) noexcept 
        : SuperType_(), data_vec_(_matrix.data_vec_), row_(_matrix.row_), column_(_matrix.column_) {}
    TMatrix(TMatrix&& _matrix) noexcept
        : SuperType_()
        , data_vec_(std::forward<DataVector_>(_matrix.data_vec_))
        , row_(_matrix.row_)
        , column_(_matrix.column_)
    {
        _matrix.row_ = 0LL;
        _matrix.column_ = 0LL;
    }

    TMatrix(IndexType _row, IndexType _column) noexcept
        : SuperType_(), data_vec_(_row* _column), row_(_row), column_(_column)
    {
        data_vec_.Resize(_row * _column);
    }
    TMatrix(IndexType _row, IndexType _column, const _ObjectType& _val) noexcept
        : SuperType_(), data_vec_(_row* _column, _val), row_(_row), column_(_column) {}

    ~TMatrix() noexcept {}

    TMatrix& operator=(const TMatrix& _matrix) noexcept {
        SuperType_::operator=(_matrix);
        data_vec_ = _matrix.data_vec_;
        row_ = _matrix.row_;
        column_ = _matrix.column_;
        return *this;
    }
    TMatrix& operator=(TMatrix&& _matrix) noexcept {
        SuperType_::operator=(std::forward<TMatrix>(_matrix));
        row_ = _matrix.row_;
        column_ = _matrix.column_;
        data_vec_ = std::forward<DataVector_>(_matrix.data_vec_);
        _matrix.row_ = 0;
        _matrix.column_ = 0;
        return *this;
    }

    Void Assign(IndexType _row, IndexType _column, const _ObjectType& _val) noexcept {
        data_vec_.Assign(_row * _column, _val);
        row_ = _row;
        column_ = _column;
    }

    NODISCARD FORCEINLINE Bool operator==(const TMatrix& _matrix) noexcept { return data_vec_ == _matrix.data_vec_; }
    NODISCARD FORCEINLINE Bool operator!=(const TMatrix& _matrix) noexcept { return data_vec_ != _matrix.data_vec_; }

    NODISCARD FORCEINLINE _ObjectType& operator()(IndexType _row, IndexType _column) noexcept { 
        return data_vec_[_row * column_ + _column];
    }
    NODISCARD FORCEINLINE const _ObjectType& operator()(IndexType _row, IndexType _column) const noexcept {
        return data_vec_[_row * column_ + _column];
    }

    NODISCARD FORCEINLINE _ObjectType& At(IndexType _row, IndexType _column) noexcept {
        return data_vec_.At(_row * column_ + _column);
    }
    NODISCARD FORCEINLINE const _ObjectType& At(IndexType _row, IndexType _column) const noexcept {
        return data_vec_.At(_row * column_ + _column);
    }

    NODISCARD FORCEINLINE _ObjectType* DataPtr() noexcept { return data_vec_.DataPtr(); }
    NODISCARD FORCEINLINE const _ObjectType* DataPtr() const noexcept { return data_vec_.DataPtr(); }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { return data_vec_.Size(); }
    NODISCARD FORCEINLINE IndexType Row() const noexcept { return row_; }
    NODISCARD FORCEINLINE IndexType Column() const noexcept { return column_; }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return data_vec_.Capacity(); }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return data_vec_.Empty(); }

    NODISCARD FORCEINLINE Iterator_ Begin() noexcept { return data_vec_.Begin(); }
    NODISCARD FORCEINLINE ConstIterator_ Begin() const noexcept { return data_vec_.Begin(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstBegin() const noexcept { return data_vec_.ConstBegin(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseBegin() noexcept { return data_vec_.ReverseBegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseBegin() const noexcept { return data_vec_.ReverseBegin(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseBegin() const noexcept { return data_vec_.ConstReverseBegin(); }
    NODISCARD FORCEINLINE Iterator_ End() noexcept { return data_vec_.End(); }
    NODISCARD FORCEINLINE ConstIterator_ End() const noexcept { return data_vec_.End(); }
    NODISCARD FORCEINLINE ConstIterator_ ConstEnd() const noexcept { return data_vec_.ConstEnd(); }
    NODISCARD FORCEINLINE ReverseIterator_ ReverseEnd() noexcept { return data_vec_.ReverseEnd(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ReverseEnd() const noexcept { return data_vec_.ReverseEnd(); }
    NODISCARD FORCEINLINE ConstReverseIterator_ ConstReverseEnd() const noexcept { return data_vec_.ConstReverseEnd(); }

    Void Clear() noexcept { 
        data_vec_.Clear(); 
        row_ = 0;
        column_ = 0;
    }

    template <typename... _ArgsType>
    FORCEINLINE Void Emplace(IndexType _row, IndexType _column, _ArgsType&&... _args) noexcept {
        data_vec_.Emplace(_row * column_ + _column, std::forward<_ArgsType>(_args)...);
    }

    Void Resize(IndexType _row, IndexType _column) noexcept { 
        data_vec_.Clear();
        data_vec_.Resize(_row * _column);
        row_ = _row;
        column_ = _column;
    }
    Void Resize(IndexType _row, IndexType _column, const _ObjectType& _val) noexcept {
        data_vec_.Clear();
        data_vec_.Resize(_row * _column, _val);
        row_ = _row;
        column_ = _column;
    }

    Void Swap(TMatrix& _matrix) noexcept { 
        data_vec_.Swap(_matrix.data_vec_);
        ::zengine::Swap(&row_, &_matrix.row_);
        ::zengine::Swap(&column_, &_matrix.column_);
    }

protected:
    using SuperType_ = ZObject;

private:
    DataVector_ data_vec_;
    IndexType row_;
    IndexType column_;
};

}//zengine