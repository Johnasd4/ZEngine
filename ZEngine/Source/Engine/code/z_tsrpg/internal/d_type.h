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

/*
    World vector, used for world scenes.
*/
struct WorldVector3D {
    Float32 x_;
    Float32 y_;
    Float32 z_;

    NODISCARD FORCEINLINE Bool operator==(const WorldVector3D& _pos) const noexcept {
        return (x_ == _pos.x_) && (y_ == _pos.y_) && (z_ == _pos.z_);
    }

    FORCEINLINE WorldVector3D operator+(const WorldVector3D& _pos) const noexcept {
        return WorldVector3D(x_ + _pos.x_, y_ + _pos.y_, z_ + _pos.z_);
    }
    FORCEINLINE WorldVector3D& operator+=(const WorldVector3D& _pos) noexcept {
        x_ += _pos.x_;
        y_ += _pos.y_;
        z_ += _pos.z_;
        return *this;
    }
    FORCEINLINE WorldVector3D operator-(const WorldVector3D& _pos) const noexcept {
        return WorldVector3D(x_ - _pos.x_, y_ - _pos.y_, z_ - _pos.z_);
    }
    FORCEINLINE WorldVector3D& operator-=(const WorldVector3D& _pos) noexcept {
        x_ -= _pos.x_;
        y_ -= _pos.y_;
        z_ -= _pos.z_;
        return *this;
    }

    FORCEINLINE WorldVector3D operator*(Float32 _num) const noexcept {
        return WorldVector3D(x_ * _num, y_ * _num, z_ * _num);
    }
    FORCEINLINE WorldVector3D& operator*=(Float32 _num) noexcept {
        x_ *= _num;
        y_ *= _num;
        z_ *= _num;
        return *this;
    }

};

/*
    Logic vector, used for game logics.
*/
struct LogicVector2D {
    Int32 x_;
    Int32 y_;

    FORCEINLINE Bool operator==(const LogicVector2D& _pos) const noexcept {
        return (x_ == _pos.x_) && (y_ == _pos.y_);
    }

    FORCEINLINE LogicVector2D operator+(const LogicVector2D& _pos) const noexcept {
        return LogicVector2D(x_ + _pos.x_, y_ + _pos.y_);
    }
    FORCEINLINE LogicVector2D& operator+=(const LogicVector2D& _pos) noexcept {
        x_ += _pos.x_;
        y_ += _pos.y_;
        return *this;
    }
    FORCEINLINE LogicVector2D operator-(const LogicVector2D& _pos) const noexcept {
        return LogicVector2D(x_ - _pos.x_, y_ - _pos.y_);
    }
    FORCEINLINE LogicVector2D& operator-=(const LogicVector2D& _pos) noexcept {
        x_ -= _pos.x_;
        y_ -= _pos.y_;
        return *this;
    }
};

/*
    Logic vector, used for game logics.
*/
struct LogicVector3D {
    Int32 x_;
    Int32 y_;
    Int32 z_;

    FORCEINLINE Bool operator==(const LogicVector3D& _pos) const noexcept {
        return (x_ == _pos.x_) && (y_ == _pos.y_) && (z_ == _pos.z_);
    }

    FORCEINLINE LogicVector3D operator+(const LogicVector3D& _pos) const noexcept {
        return LogicVector3D(x_ + _pos.x_, y_ + _pos.y_, z_ + _pos.z_);
    }
    FORCEINLINE LogicVector3D& operator+=(const LogicVector3D& _pos) noexcept {
        x_ += _pos.x_;
        y_ += _pos.y_;
        z_ += _pos.z_;
        return *this;
    }
    FORCEINLINE LogicVector3D operator-(const LogicVector3D& _pos) const noexcept {
        return LogicVector3D(x_ - _pos.x_, y_ - _pos.y_, z_ - _pos.z_);
    }
    FORCEINLINE LogicVector3D& operator-=(const LogicVector3D& _pos) noexcept {
        x_ -= _pos.x_;
        y_ -= _pos.y_;
        z_ -= _pos.z_;
        return *this;
    }
};

/*
    Tile size type.
*/
struct TileSizeType {
    Float32 radius_;
    Float32 height_;

    NODISCARD FORCEINLINE Bool operator==(const TileSizeType& _size) const noexcept {
        return (radius_ == _size.radius_) || (height_ == _size.height_);
    }
};

class ZBoard;
class ZBoardObject;
class ZHexLogicBoard;
class ZHexLogicTile;
class ZLogicBoard;
class ZLogicBoardObject;
class ZLogicTile;
class ZSceneEvent;
class ZTile;
class ZWorldBoard;
class ZWorldBoardObject;
class ZWorldTile;

}//tsrpg
}//zengine