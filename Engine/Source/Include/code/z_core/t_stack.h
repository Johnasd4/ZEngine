/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#ifndef Z_CORE_T_STACK_H_
#define Z_CORE_T_STACK_H_

#include "internal/z_drive.h"


#include "t_allocator.h"
#include "t_vector.h"
#include "z_object.h"

namespace zengine {

/*
    Stack caintainer.
*/
template<typename ObjectType, typename ContainerType = TVector<ObjectType>>
class TStack : public ZObject {
public:
    using InitializerList = std::initializer_list<ObjectType>;

    FORCEINLINE TStack() noexcept : SuperType(), stack_() {}
    FORCEINLINE TStack(const TStack& stack) noexcept : SuperType(), stack_(stack.stack_) {}
    FORCEINLINE TStack(TStack&& stack) noexcept : SuperType(), stack_(std::move(stack.stack_)) {}

    FORCEINLINE TStack(SizeType size) noexcept : SuperType(), stack_(size) {}
    FORCEINLINE TStack(SizeType size, const ObjectType& value) noexcept : SuperType(), stack_(size, value) {}
    template <typename InputIterator>
    FORCEINLINE TStack(InputIterator first, InputIterator last) noexcept : SuperType(), stack_(first, last) {}
    FORCEINLINE TStack(InitializerList init_list) noexcept : SuperType(), stack_(init_list) {}
 
    FORCEINLINE ~TStack() noexcept {}

    FORCEINLINE TStack& operator=(const TStack& stack) noexcept { 
        stack_.operator=(stack.stack_);
        return *this;
    }
    FORCEINLINE TStack& operator=(TStack&& stack) noexcept { 
        stack_.operator=(std::move(stack.stack_));
        return *this;
    }
    FORCEINLINE TStack& operator=(InitializerList init_list) noexcept {
        stack_.operator=(init_list);
        return *this;
    }

    FORCEINLINE Void Assign(SizeType size, const ObjectType& value) noexcept {
        return stack_.Assign(size, value);
    }
    template <class InputIterator>
    FORCEINLINE Void Assign(InputIterator first, InputIterator last) noexcept {
        return stack_.Assign(first, last);
    }
    FORCEINLINE Void Assign(InitializerList init_list) noexcept {
        return stack_.Assign(init_list);
    }

    NODISCARD FORCEINLINE Bool operator==(const TStack& stack) noexcept { return stack_ == stack; }
    NODISCARD FORCEINLINE Bool operator!=(const TStack& stack) noexcept { return stack_ != stack; }

    NODISCARD FORCEINLINE ObjectType& Top() noexcept { return stack_.Back(); }
    NODISCARD FORCEINLINE const ObjectType& Top() const noexcept { return stack_.Back(); }

    NODISCARD FORCEINLINE IndexType Size() const noexcept { return stack_.Size(); }
    NODISCARD FORCEINLINE IndexType Capacity() const noexcept { return stack_.Capacity(); }
    NODISCARD FORCEINLINE Bool Empty() const noexcept { return stack_.Empty(); }

    FORCEINLINE Void Pop() noexcept { stack_.PopBack(); }

    FORCEINLINE Void Push(const ObjectType& value) noexcept { stack_.PushBack(value); }
    FORCEINLINE Void Push(ObjectType&& value) noexcept { stack_.PushBack(std::forward<ObjectType>(value)); }

    FORCEINLINE Void Clear() noexcept { stack_.Clear(); }

    FORCEINLINE Void Resize(SizeType size) noexcept { stack_.Resize(size); }
    FORCEINLINE Void Resize(SizeType size, const ObjectType& value) noexcept { stack_.Resize(size, value); }

    FORCEINLINE Void Swap(TStack& stack) noexcept { stack_.Swap(stack); }

protected:
    using SuperType = ZObject;

private:
    ContainerType stack_;
};

}//zengine

#endif // !Z_CORE_T_STACK_H_