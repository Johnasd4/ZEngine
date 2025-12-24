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
#define TSRPG_DLLFILE
#include "drive/d_pch.h"

#include "z_logic_tile.h"

#include "z_display_tile.h"
#include "z_logic_board.h"

namespace zengine {
namespace tsrpg {
    
NODISCARD static THashMap<ZWString::STDString_, ZLogicTileTexture>& TextureMapP() noexcept{
    static THashMap<ZWString::STDString_, ZLogicTileTexture> map;
    return map;
}

Void ZLogicTileTexture::RegisterLogicTileTexture(ZLogicTileTexture&& _texture) noexcept {
    static THashMap<ZWString::STDString_, ZLogicTileTexture>& texture_map = TextureMapP();
    texture_map.InsertOrAssign(_texture.texture_name_.GetDataPtr(), std::move(_texture));
}

NODISCARD const ZLogicTileTexture* ZLogicTileTexture::GetLogicTileTextureByName(const WChar* _texture_name) noexcept {
    static THashMap<ZWString::STDString_, ZLogicTileTexture>& texture_map = TextureMapP();
    auto iterator = texture_map.Find(_texture_name);
    if (iterator == texture_map.End()) {
        Z_LOG_ERROR(
            error_code::kZLogicTileErrorCode_TextureNotExist, 0,
            "Texture not exist! Name: %ls", _texture_name
        );
        return nullptr;
    }
    return &(iterator->second);
}

ZLogicTileTexture::ZLogicTileTexture() noexcept
    : texture_name_()
    , material_name_()
{}

ZLogicTileTexture::ZLogicTileTexture(ZLogicTileTexture&& _texture) noexcept
{
    MoveP(std::forward<ZLogicTileTexture>(_texture));
}

ZLogicTileTexture::ZLogicTileTexture(const WChar* _texture_name, const WChar* _material_name) noexcept
    : texture_name_(_texture_name)
    , material_name_(_material_name)
{}

ZLogicTileTexture::~ZLogicTileTexture() noexcept {}

ZLogicTileTexture& ZLogicTileTexture::operator=(ZLogicTileTexture&& _texture) noexcept
{
    if (this == &_texture) {
        return *this;
    }
    MoveP(std::forward<ZLogicTileTexture>(_texture));
    return *this;
}

Void ZLogicTileTexture::MoveP(ZLogicTileTexture&& _texture) noexcept {
    texture_name_ = std::move(_texture.texture_name_);
    material_name_ = std::move(_texture.material_name_);
}

}//tsrpg
}//zengine

namespace zengine {
namespace tsrpg {

ZLogicTile::ZLogicTile() noexcept
    : SuperType_()
    , pos_()
    , texture_node_array_()
    , display_tile_head_ptr_(nullptr)
    , upper_logic_tile_ptr_(nullptr)
    , lower_logic_tile_ptr_(nullptr)
{}

ZLogicTile::~ZLogicTile() noexcept {}

NODISCARD ReturnType ZLogicTile::SetPos(const LogicVector3D& _pos) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    if (pos_ != _pos) {
        LogicVector3D old_pos = pos_;
        pos_ = _pos;
        if (Initialized()) {
            link_code = OnPosChanged(old_pos, _pos);
            if (link_code != kOK) {
                ret_val = error_code::kZLogicTileErrorCode_LinkError;
                Z_LOG_ERROR(ret_val, link_code, "ZLogicTile::OnPosChanged() link error!");
                return ret_val;
            }
        }
    }
    return ret_val;
}

NODISCARD const ZLogicTileTexture* ZLogicTile::GetTopTexturePtr() const noexcept {
    if (texture_node_array_.GetSize() == 0) {
        return static_cast<ZLogicBoard*>(owner_board_ptr_)->base_layer_texture_ptr_;
    }
    return texture_node_array_[texture_node_array_.GetSize() - 1].texture_ptr_;
}

NODISCARD const ZLogicTileTexture* ZLogicTile::GetTexturePtrByPosZ(Int32 _pos_z) const noexcept {
    SizeType node_index = texture_node_array_.GetSize() - 1;
    if (_pos_z > Z()) {
        Z_LOG_ERROR(
            error_code::kZLogicTileErrorCode_TexturePosZOutOfRange, 0,
            "Texture pos z out of range! Tile(x, y, z) = (%d, %d, %d), pos_z = %d", X(), Y(), Z(), _pos_z
        );
        return nullptr;
    }
    else if (_pos_z <= texture_node_array_[0].pos_z_ - texture_node_array_[node_index].num_) {
        const ZLogicTileTexture* base_layer_texture_ptr = 
            static_cast<ZLogicBoard*>(owner_board_ptr_)->base_layer_texture_ptr_;
        if (base_layer_texture_ptr == nullptr) {
            Z_LOG_ERROR(
                error_code::kZLogicTileErrorCode_TexturePosZOutOfRange, 0,
                "Texture pos z out of range! Tile(x, y, z) = (%d, %d, %d), pos_z = %d", X(), Y(), Z(), _pos_z
            );
            return nullptr;
        }
        return base_layer_texture_ptr;
    }
    while (_pos_z <= texture_node_array_[node_index].pos_z_ - texture_node_array_[node_index].num_) {
        --node_index;
    }
    return texture_node_array_[node_index].texture_ptr_;
}

NODISCARD const ReturnType ZLogicTile::CalculateTexturePtrVectorByPosZAndLength(
    TArray<const ZLogicTileTexture*>* _texture_ptr_array_ptr,
    Int32 _pos_z,
    Int32 _length
) const noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    Z_CHECK(
        _texture_ptr_array_ptr == nullptr, error_code::kZLogicTileErrorCode_NullptrParam, 
        "_texture_ptr_array_ptr is nullptr!"
    )
        Int32 node_index = static_cast<Int32>(texture_node_array_.GetSize()) - 1;
    Int32 start_pos_z = math::Min(_pos_z, Z());
    const ZLogicTileTexture* base_layer_texture_ptr = 
        static_cast<ZLogicBoard*>(owner_board_ptr_)->base_layer_texture_ptr_;
    Int32 end_pos_z;
    if (base_layer_texture_ptr != nullptr) {
        end_pos_z = _pos_z - _length;
    }
    else {
        end_pos_z = math::Max(
            _pos_z - _length,
            texture_node_array_[0].pos_z_ - texture_node_array_[node_index].num_
        );
    }
    Int32 length = start_pos_z - end_pos_z;
    _texture_ptr_array_ptr->Clear();
    _texture_ptr_array_ptr->Reserve(length);
    //find start pos
    while (node_index >= 0) {
        if (start_pos_z <= texture_node_array_[node_index].pos_z_ - texture_node_array_[node_index].num_) {
            break;
        }
        --node_index;
    }

    //calculate the texture array
    //node left
    if (node_index >= 0) {
        Int32 current_node_num_left =
            texture_node_array_[node_index].num_ - (texture_node_array_[node_index].pos_z_ - start_pos_z);
        while (node_index >= 0 && length > 0) {
            _texture_ptr_array_ptr->PushBack(texture_node_array_[node_index].texture_ptr_);
            --current_node_num_left;
            if (current_node_num_left == 0) {
                --node_index;
                current_node_num_left = texture_node_array_[node_index].num_;
            }
            --length;
        }
    }
    while (length > 0) {
        _texture_ptr_array_ptr->PushBack(base_layer_texture_ptr);
        --length;
    }
    return ret_val;
}

NODISCARD LogicVector2D ZLogicTile::Index() const noexcept { return LogicVector2D(pos_.x_, pos_.y_); }

NODISCARD UInt64 ZLogicTile::Type() const noexcept { return kTileType_LogicTile; }

Void ZLogicTile::Destroy() noexcept {
    SuperType_::Destroy();
    while (display_tile_head_ptr_ != nullptr) {
        display_tile_head_ptr_->Destroy();
    }
}

NODISCARD ReturnType ZLogicTile::Initialize(
    ZBoard* _owner_board_ptr,
    const LogicVector3D& _pos,
    const ZLogicTileTexture* _texture_ptr,
    Int32 _texture_length
) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;

    Z_CHECK(
        _owner_board_ptr == nullptr, error_code::kZLogicTileErrorCode_NullptrParam,
        "_TileMeshPtr is nullptr!"
    );

    link_code = SuperType_::InitializeP(_owner_board_ptr);
    if (link_code != kOK) {
        ret_val = error_code::kZLogicTileErrorCode_LinkError;
        Z_LOG_ERROR(ret_val, link_code, "ZLogicTile::Initialize() link error!");
        return ret_val;
    }

    pos_ = _pos;

    if (_texture_ptr != nullptr) {
        texture_node_array_.PushBack(ZLogicTile::TextureNode_(
            _texture_ptr,
            _pos.z_,
            _texture_length
        ));
    }

    return ret_val;
}

NODISCARD ReturnType ZLogicTile::OnPosChanged(const LogicVector3D& _old_pos, const LogicVector3D& _new_pos) noexcept {
    ReturnType ret_val = kOK;
    ReturnType link_code = kOK;
    //change texture pos
    if (_old_pos.z_ != _new_pos.z_) {
        Int32 offset_z = _new_pos.z_ - _old_pos.z_;
        for (auto node = texture_node_array_.Begin(); node != texture_node_array_.End(); ++node) {
            node->pos_z_ += offset_z;
        }
    }
    ZDisplayTile* display_tile_ptr = display_tile_head_ptr_;
    while (display_tile_ptr != nullptr) {
        link_code = display_tile_ptr->SetPos(_new_pos);
        if (link_code != kOK) {
            ret_val = error_code::kZLogicTileErrorCode_LinkError;
            Z_LOG_ERROR(
                ret_val, link_code, 
                "ZDisplayTile::SetPos() link error! old_pos(x, y, z) = (%d, %d, %d), new_pos(x, y, z) = (%d, %d, %d)",
                _old_pos.x_, _old_pos.y_, _old_pos.z_, _new_pos.x_, _new_pos.y_, _new_pos.z_
            );
        }
    }
    return ret_val;
}

}//tsrpg
}//zengine