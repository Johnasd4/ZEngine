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

#include "t_list.h"
#include "z_object.h"

namespace zengine {

/*
    Pool caintainer. When all objects used, will auto apply new objects.
    Applying object and releasing object is independent, can be used in diffent threads.
*/
template<typename _ObjectType>
class TPool : public ZObject {
public:
    static constexpr IndexType kDefaultPoolSize = 10;
    static constexpr IndexType kMemoryPtrListSize = 10;
    static constexpr Float32 kAutoExtendMultFactor = 0.2f;

    using MemoryPtrList_ = TList<Void*>;

    TPool() noexcept : SuperType_(), mem_ptr_list_(kMemoryPtrListSize), total_num_(0) {
        ApplyNewObjectsP(kDefaultPoolSize);
    }
    TPool(IndexType _size) noexcept : SuperType_(), mem_ptr_list_(kMemoryPtrListSize), total_num_(0) {
        ApplyNewObjectsP(_size);
    }
    TPool(IndexType _size, const _ObjectType& _val) noexcept
        : SuperType_(), mem_ptr_list_(kMemoryPtrListSize), total_num_(0) 
    {
        ApplyNewObjectsP(_size, &_val);
    }

    ~TPool() noexcept {
        for (auto mem_ptr = mem_ptr_list_.Begin(); mem_ptr != mem_ptr_list_.End(); ++mem_ptr) {
            memory_pool::ReleaseMemory(*mem_ptr);
        }
    }

    NODISCARD _ObjectType* Apply() noexcept {
        if (head_node_ptr_ == end_node_ptr_) {
            ApplyNewObjectsP();
        }
        _ObjectType* object_ptr = reinterpret_cast<_ObjectType*>(head_node_ptr_);
        head_node_ptr_ = head_node_ptr_->next_node_;
        return object_ptr;
    }
    NODISCARD Void Release(_ObjectType* _object_ptr) noexcept {
        end_node_ptr_->next_node_ = reinterpret_cast<Node_*>(_object_ptr);
        end_node_ptr_ = reinterpret_cast<Node_*>(_object_ptr);
    }

    /*
        Extends the pool to a size >= given size. If _size = 0 then auto calculates the extending size.
    */
    Void ExtendPool(IndexType _size = 0) noexcept {
        ApplyNewObjectsP(_size);
    }
    /*
        Extends the pool to a size >= given size. If _size = 0 then auto calculates the extending size.
    */
    Void ExtendPool(IndexType _size, const _ObjectType& _val) noexcept {
        ApplyNewObjectsP(_size, &_val);
    }

protected:
    using SuperType_ = ZObject;

private:
    struct Node_ {
        _ObjectType object_;
        Node_* next_node_;
    };

    TPool(const TPool&) = delete;
    TPool(TPool&&) = delete;
    TPool& operator=(const TPool&) = delete;
    TPool& operator=(TPool&&) = delete;

    /*
        Apply new ojects, add to the head node.
    */
    Void ApplyNewObjectsP(Int32 _num = 0, const _ObjectType* _val_ptr = nullptr) noexcept {
        Int32 extend_num = _num == 0 ? Int32(total_num_ * kAutoExtendMultFactor) : (total_num_ - _num);
        if (extend_num < kDefaultPoolSize) {
            extend_num = kDefaultPoolSize;
        }
        MemoryType mem_size = sizeof(Node_) * extend_num;
        Void* mem_ptr = memory_pool::ApplyMemory(mem_size, &mem_size);
        extend_num = mem_size / sizeof(Node_);
        if (_val_ptr == nullptr) {
            for (IndexType index = 0; index < extend_num; ++index) {
                Node_* node_ptr = reinterpret_cast<Node_*>(mem_ptr) + index;
                new(reinterpret_cast<Void*>(node_ptr)) _ObjectType();
                node_ptr->next_node_ = node_ptr + 1;
            }
        }
        else {
            for (IndexType index = 0; index < extend_num; ++index) {
                Node_* node_ptr = reinterpret_cast<Node_*>(mem_ptr) + index;
                new(reinterpret_cast<Void*>(node_ptr)) _ObjectType(*_val_ptr);
                node_ptr->next_node_ = node_ptr + 1;
            }
        }
        if (total_num_ == 0) {
            end_node_ptr_ = reinterpret_cast<Node_*>(mem_ptr) + extend_num - 1;
        }
        else {
            (reinterpret_cast<Node_*>(mem_ptr) + extend_num - 1)->next_node_ = head_node_ptr_;
        }
        head_node_ptr_ = reinterpret_cast<Node_*>(mem_ptr);
        total_num_ = total_num_ + extend_num;
    }

    MemoryPtrList_ mem_ptr_list_;
    Node_* head_node_ptr_;
    Node_* end_node_ptr_;
    IndexType total_num_;
};

}//zengine