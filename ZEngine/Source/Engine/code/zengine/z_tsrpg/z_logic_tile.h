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

#include "drive.h"

#include "../z_core/t_array.h"
#include "../z_core/z_string.h"

#include "z_tile.h"

namespace zengine {
namespace error_code {
enum ZLogicTileErrorCodeEnum : ReturnType {
    kZLogicTileErrorCode_LinkError = kErrorCodeBase_ZLogicTile,
    kZLogicTileErrorCode_SystemError,
    kZLogicTileErrorCode_NullptrParam,
    kZLogicTileErrorCode_ParamOutOfRange,
    kZLogicTileErrorCode_TextureNotExist,
    kZLogicTileErrorCode_TexturePosZOutOfRange
};
}//error_code
}//zengine

namespace zengine {
namespace tsrpg {

/*
    Tile texture, contains the name of the texture and the name of the relevant material.
*/
class TSRPG_DLLAPI ZLogicTileTexture : public ZObject {
public:
    static Void RegisterLogicTileTexture(ZLogicTileTexture&& _texture) noexcept;
    NODISCARD static const ZLogicTileTexture* GetLogicTileTextureByName(const WChar* _texture_name) noexcept;

    ZLogicTileTexture() noexcept;
    ZLogicTileTexture(ZLogicTileTexture&& _texture) noexcept;
    ZLogicTileTexture(const WChar* _texture_name, const WChar* _material_name) noexcept;

    virtual ~ZLogicTileTexture() noexcept;

    ZLogicTileTexture& operator=(ZLogicTileTexture&& _texture) noexcept;

    //TODO: all kinds of function ptr
    ZWString texture_name_;
    ZWString material_name_;

private:
    Void MoveP(ZLogicTileTexture&& _texture) noexcept;
};

}//tsrpg
}//zengine

namespace zengine {
namespace tsrpg {

/*
    The logic tile base class, used for logic calculation.
*/
class TSRPG_DLLAPI ZLogicTile : public ZTile {
public:

    /*
        Bottom node first, top node last.
        pos_z_: The top of the texture node.
        num_: The num of the same texture.
    */
    struct TextureNode_ {    
        const ZLogicTileTexture* texture_ptr_;
        Int32 pos_z_;
        Int32 num_;
    };

    static inline constexpr Int32 kDistanceNotConnect = -1;

    ZLogicTile() noexcept;

    virtual ~ZLogicTile() noexcept;

    NODISCARD FORCEINLINE Int32 X() const noexcept { return pos_.x_; }
    NODISCARD FORCEINLINE Int32 Y() const noexcept { return pos_.y_; }
    NODISCARD FORCEINLINE Int32 Z() const noexcept { return pos_.z_; }
    NODISCARD FORCEINLINE LogicVector3D Pos() const noexcept { return pos_; }
    NODISCARD ReturnType SetPos(const LogicVector3D& _pos) noexcept;

    /*
        Returns the top texture of the tile.
    */
    NODISCARD const ZLogicTileTexture* GetTopTexturePtr() const noexcept;

    /*
        Returns the texture of the tile by pos z.
    */
    NODISCARD const ZLogicTileTexture* GetTexturePtrByPosZ(Int32 _pos_z) const noexcept;

    /*
        Returns the texture array by the given pos z and length.
    */
    NODISCARD const ReturnType CalculateTexturePtrVectorByPosZAndLength(
        TArray<const ZLogicTileTexture*>* _texture_ptr_array_ptr,
        Int32 _pos_z, 
        Int32 _length
    ) const noexcept;

    /*
        Gets the tile's board index.
    */
    NODISCARD virtual LogicVector2D Index() const noexcept;

    /*
        Get the tile type.
    */
    NODISCARD virtual RPGObjectType Type() const noexcept;

    /*
        Destroy the tile.
    */
    virtual Void Destroy() noexcept;

    /*
        Initialize the tile.
    */
    NODISCARD virtual ReturnType Initialize(
        ZBoard* _owner_board_ptr, 
        const LogicVector3D& _pos, 
        const ZLogicTileTexture* _texture_ptr,
        Int32 _texture_length
    ) noexcept;

    /*
        Returns a copy of the current tile. Must be overrided.
    */
    NODISCARD virtual ZTile* CreateCopy() const noexcept = 0;

    /*
        Returns the logic distance with height, ignores all obstacles.
    */
    NODISCARD virtual Int32 CalculateDistanceWithHeight(const ZLogicTile& _tile) const noexcept = 0;

    /*
        Returns the logic distance without height, ignores all obstacles.
    */
    NODISCARD virtual Int32 CalculateDistanceWithoutHeight(const ZLogicTile& _tile) const noexcept = 0;

    /*
        Called after pos changed.
    */
    NODISCARD virtual ReturnType OnPosChanged(const LogicVector3D& _old_pos, const LogicVector3D& _new_pos) noexcept;

protected:
    using SuperType_ = ZTile;

private:
    ZLogicTile(const ZLogicTile&) = delete;
    ZLogicTile(ZLogicTile&&) = delete;
    ZLogicTile& operator=(const ZLogicTile&) = delete;
    ZLogicTile& operator=(ZLogicTile&&) = delete;

public:
    LogicVector3D pos_;
    TArray<TextureNode_> texture_node_array_;

    ZDisplayTile* display_tile_head_ptr_;
    //upper tile
    ZLogicTile* upper_logic_tile_ptr_;
    //lower tile
    ZLogicTile* lower_logic_tile_ptr_;
};

}//tsrpg
}//zengine