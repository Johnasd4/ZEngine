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

#include "t_array.h"
#include "z_object.h"

namespace zengine {

/*
    Stack container.
*/
template<typename _ObjectType, typename _ContainerType = TArray<_ObjectType>>
class TStack : public ZObject<> {
public:
    using InitializerList_ = std::initializer_list<_ObjectType>;

    FORCEINLINE TStack() noexcept : SuperType_(), stack_() {}
    FORCEINLINE TStack(const TStack& _stack) noexcept : SuperType_(_stack), stack_(_stack.stack_) {}
    FORCEINLINE TStack(TStack&& _stack) noexcept 
        : SuperType_(std::forward<TStack>(_stack)), stack_(std::move(_stack.stack_)) {}

    FORCEINLINE TStack(SizeType _size) noexcept : SuperType_(), stack_(_size) {}
    FORCEINLINE TStack(SizeType _size, const _ObjectType& _val) noexcept : SuperType_(), stack_(_size, _val) {}
    template <typename _InputIterator>
    FORCEINLINE TStack(_InputIterator _first, _InputIterator _last) noexcept : SuperType_(), stack_(_first, _last) {}
    FORCEINLINE TStack(InitializerList_ _init_list) noexcept : SuperType_(), stack_(_init_list) {}
 
    FORCEINLINE ~TStack() noexcept {}

    FORCEINLINE TStack& operator=(const TStack& _stack) noexcept { 
        SuperType_::operator=(_stack);
        stack_ = _stack.stack_;
        return *this;
    }
    FORCEINLINE TStack& operator=(TStack&& _stack) noexcept { 
        SuperType_::operator=(std::forward<TStack>(_stack));
        stack_ = std::move(_stack.stack_);
        return *this;
    }
    FORCEINLINE TStack& operator=(InitializerList_ _init_list) noexcept {
        stack_ = _init_list;
        return *this;
    }

    FORCEINLINE Void Assign(SizeType _size, const _ObjectType& _val) noexcept {
        return stack_.Assign(_size, _val);
    }
    template <class _InputIterator>
    FORCEINLINE Void Assign(_InputIterator _first, _InputIterator _last) noexcept {
        return stack_.Assign(_first, _last);
    }
    FORCEINLINE Void Assign(InitializerList_ _init_list) noexcept {
        return stack_.Assign(_init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TStack& _stack) noexcept { return stack_ == _stack; }
    NODISCARD FORCEINLINE Bool operator!=(const TStack& _stack) noexcept { return stack_ != _stack; }

    NODISCARD FORCEINLINE _ObjectType& Top() noexcept { return stack_.Back(); }
    NODISCARD FORCEINLINE const _ObjectType& Top() const noexcept { return stack_.Back(); }

    NODISCARD FORCEINLINE SizeType GetSize() const noexcept { return stack_.GetSize(); }
    NODISCARD FORCEINLINE SizeType GetCapacity() const noexcept { return stack_.GetCapacity(); }
    NODISCARD FORCEINLINE Bool IsEmpty() const noexcept { return stack_.IsEmpty(); }

    FORCEINLINE Void PopFront() noexcept { stack_.PopBack(); }

    FORCEINLINE Void PushBack(const _ObjectType& _val) noexcept { stack_.PushBack(_val); }
    FORCEINLINE Void PushBack(_ObjectType&& _val) noexcept { stack_.PushBack(std::forward<_ObjectType>(_val)); }

    FORCEINLINE Void Clear() noexcept { stack_.Clear(); }

    FORCEINLINE Void Resize(SizeType _size) noexcept { stack_.Resize(_size); }
    FORCEINLINE Void Resize(SizeType _size, const _ObjectType& _val) noexcept { stack_.Resize(_size, _val); }

    FORCEINLINE Void Swap(TStack& _stack) noexcept { stack_.Swap(_stack); }

protected:
    using SuperType_ = ZObject;

private:
    _ContainerType stack_;
};

}//zengine