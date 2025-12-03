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

#include "d_lib.h"

namespace zengine {
namespace tsrpg {

template<typename _NumberType>
struct Vector3D {
    _NumberType x_;
    _NumberType y_;
    _NumberType z_;

    NODISCARD FORCEINLINE Bool operator==(const Vector3D& _pos) const noexcept {
        return (x_ == _pos.x_) && (y_ == _pos.y_) && (z_ == _pos.z_);
    }

    NODISCARD FORCEINLINE Vector3D operator+(const Vector3D& _pos) const noexcept {
        return Vector3D(x_ + _pos.x_, y_ + _pos.y_, z_ + _pos.z_);
    }
    FORCEINLINE Vector3D& operator+=(const Vector3D& _pos) noexcept {
        x_ += _pos.x_;
        y_ += _pos.y_;
        z_ += _pos.z_;
        return *this;
    }
    NODISCARD FORCEINLINE Vector3D operator-(const Vector3D& _pos) const noexcept {
        return Vector3D(x_ - _pos.x_, y_ - _pos.y_, z_ - _pos.z_);
    }
    FORCEINLINE Vector3D& operator-=(const Vector3D& _pos) noexcept {
        x_ -= _pos.x_;
        y_ -= _pos.y_;
        z_ -= _pos.z_;
        return *this;
    }

    NODISCARD FORCEINLINE Vector3D operator*(_NumberType _num) const noexcept {
        return Vector3D(x_ * _num, y_ * _num, z_ * _num);
    }
    FORCEINLINE Vector3D& operator*=(_NumberType _num) noexcept {
        x_ *= _num;
        y_ *= _num;
        z_ *= _num;
        return *this;
    }

    NODISCARD FORCEINLINE Vector3D operator/(_NumberType _num) const noexcept {
        return Vector3D(x_ / _num, y_ / _num, z_ / _num);
    }
    FORCEINLINE Vector3D& operator/=(_NumberType _num) noexcept {
        x_ /= _num;
        y_ /= _num;
        z_ /= _num;
        return *this;
    }
};

template<typename _NumberType>
struct Vector2D {
    _NumberType x_;
    _NumberType y_;

    NODISCARD FORCEINLINE Bool operator==(const Vector2D& _pos) const noexcept {
        return (x_ == _pos.x_) && (y_ == _pos.y_);
    }

    NODISCARD FORCEINLINE Vector2D operator+(const Vector2D& _pos) const noexcept {
        return Vector2D(x_ + _pos.x_, y_ + _pos.y_);
    }
    FORCEINLINE Vector2D& operator+=(const Vector2D& _pos) noexcept {
        x_ += _pos.x_;
        y_ += _pos.y_;
        return *this;
    }
    NODISCARD FORCEINLINE Vector2D operator-(const Vector2D& _pos) const noexcept {
        return Vector2D(x_ - _pos.x_, y_ - _pos.y_);
    }
    FORCEINLINE Vector2D& operator-=(const Vector2D& _pos) noexcept {
        x_ -= _pos.x_;
        y_ -= _pos.y_;
        return *this;
    }

    NODISCARD FORCEINLINE Vector2D operator*(_NumberType _num) const noexcept {
        return Vector2D(x_ * _num, y_ * _num);
    }
    FORCEINLINE Vector2D& operator*=(_NumberType _num) noexcept {
        x_ *= _num;
        y_ *= _num;
        return *this;
    }

    NODISCARD FORCEINLINE Vector2D operator/(_NumberType _num) const noexcept {
        return Vector2D(x_ / _num, y_ / _num);
    }
    FORCEINLINE Vector2D& operator/=(_NumberType _num) noexcept {
        x_ /= _num;
        y_ /= _num;
        return *this;
    }
};

using DisplayVector3D = Vector3D<Float64>;
using LogicVector3D = Vector3D<Int32>;
using LogicVector2D = Vector2D<Int32>;

/*
    Tile size type.
*/
struct TileSizeType {
    Float64 radius_;
    Float64 height_;

    NODISCARD FORCEINLINE Bool operator==(const TileSizeType& _size) const noexcept {
        return (radius_ == _size.radius_) && (height_ == _size.height_);
    }
};

}//tsrpg
}//zengine