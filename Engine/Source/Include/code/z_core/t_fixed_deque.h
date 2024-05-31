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
#ifndef Z_CORE_T_FIEXED_QUEUE_H_
#define Z_CORE_T_FIEXED_QUEUE_H_

#include "internal/drive.h"

#include <deque>

#include "z_object.h"

namespace zengine {

namespace internal {

template<typename FixedDequeType, typename Function, typename... ArgsType>
concept kIsFixedDequeInitFunction = requires(FixedDequeType * deque, Function function, ArgsType&&... args) {
    function(deque, std::forward<ArgsType>(args)...);
};

}//internal

/*
    Fixed deque caintainer, front points at the first object, back points at the last object, when the deque is 
    empty, the front points at 0 and back points at -1.
*/
template<typename ObjectType, IndexType kCapacity>
class TFixedDeque : public ZObject {
public:
    using STDArray = std::array<ObjectType, kCapacity>;
    using InitializerList = std::initializer_list<ObjectType>;

    FORCEINLINE constexpr TFixedDeque() noexcept : SuperType(), deque_(), front_index_(0), back_index_(-1) {}
    FORCEINLINE constexpr TFixedDeque(const TFixedDeque& deque) noexcept : 
        SuperType(), deque_(deque.deque_), front_index_(deque.front_index_), back_index_(deque.back_index_) {}
    FORCEINLINE constexpr TFixedDeque(TFixedDeque&& deque) noexcept : 
        SuperType(), deque_(std::move(deque.deque_)), front_index_(deque.front_index_), back_index_(deque.back_index_) {}
    FORCEINLINE TFixedDeque(InitializerList init_list) noexcept : 
        SuperType(), deque_(init_list), front_index_(0), back_index_(kCapacity - 1) {}

    /*
        Constexpr deque, the work is done at compile time.
        The Constructor's fisrt parameter is the initial funtion of the deque. The
        first parameter of the initial funtion must be TFixedDeque*.
        Constructor Template Parameters:
        - InitFunction: The function type that initial the deque.
        - ArgsType...: The parameters type.
        Constructor Parameters:
        - init_function: The function to initial the deque.
        - ArgsType...: The parameters of the function except for the fisrt.
        Example:
        constexpr auto init_function = [](TFixedDeque<Int32, 10>* deque_ptr) {
            for (IndexType index = 0; index < deque_ptr->Size(); ++index) {
                (*deque_ptr)[index] = 1;
            }
        };
        constexpr TFixedDeque<Int32, 10> test(init_function);
    */
    template<typename InitFunction, typename... ArgsType>
    requires internal::kIsFixedDequeInitFunction<TFixedDeque<ObjectType, kCapacity>, InitFunction, ArgsType...>
    FORCEINLINE constexpr TFixedDeque(InitFunction&& init_function, ArgsType&&... args) noexcept : SuperType() {
        init_function(this, std::forward<ArgsType>(args)...);
    }

    FORCEINLINE constexpr ~TFixedDeque() noexcept {}

    NODISCARD FORCEINLINE constexpr Bool operator==(const TFixedDeque& deque) noexcept { return deque_ == deque.deque_; }
    NODISCARD FORCEINLINE constexpr Bool operator!=(const TFixedDeque& deque) noexcept { return deque_ != deque.deque_; }
     
    NODISCARD FORCEINLINE constexpr ObjectType& operator[](IndexType index) noexcept { 
        return deque_[(front_index_ + index) % kCapacity];
    }
    NODISCARD FORCEINLINE constexpr const ObjectType& operator[](IndexType index) const noexcept { 
        return deque_[(front_index_ + index) % kCapacity];
    }

    NODISCARD FORCEINLINE constexpr ObjectType& At(IndexType index) noexcept { 
        return deque_.at((front_index_ + index) % kCapacity);
    }
    NODISCARD FORCEINLINE constexpr const ObjectType& At(IndexType index) const noexcept { 
        return deque_.at((front_index_ + index) % kCapacity); 
    }
    NODISCARD FORCEINLINE constexpr ObjectType& Front() noexcept { return deque_[front_index_]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& Front() const noexcept { return deque_[front_index_]; }
    NODISCARD FORCEINLINE constexpr ObjectType& Back() noexcept { return deque_[back_index_]; }
    NODISCARD FORCEINLINE constexpr const ObjectType& Back() const noexcept { return deque_[back_index_]; }
    NODISCARD FORCEINLINE constexpr ObjectType* DataPtr() noexcept { return deque_.data(); }
    NODISCARD FORCEINLINE constexpr const ObjectType* DataPtr() const noexcept { return deque_.data(); }

    NODISCARD FORCEINLINE static constexpr IndexType Capacity() noexcept { return kCapacity; }
    NODISCARD FORCEINLINE constexpr IndexType Size() noexcept { return kCapacity; }  //TODO

    NODISCARD FORCEINLINE constexpr IndexType PopFront() noexcept { return kCapacity; }  //TODO
    NODISCARD FORCEINLINE constexpr IndexType PopBack() noexcept { return kCapacity; }  //TODO
    NODISCARD FORCEINLINE constexpr IndexType PushFront() noexcept { return kCapacity; }  //TODO
    NODISCARD FORCEINLINE constexpr IndexType PushBack() noexcept { return kCapacity; }  //TODO

    NODISCARD FORCEINLINE constexpr IndexType SetSize() noexcept { return kCapacity; }  //TODO
    NODISCARD FORCEINLINE constexpr IndexType Clear() noexcept { return kCapacity; }  //TODO
    FORCEINLINE Void Fill(const ObjectType& value) noexcept { 
        deque_.fill(value); 
        front_index_ = 0;
        back_index_ = kCapacity - 1;

    }
    FORCEINLINE Void Swap(TFixedDeque& deque) noexcept { deque_.swap(deque); }

protected:
    using SuperType = ZObject;

private:
    IndexType front_index_;
    IndexType back_index_;
    STDArray deque_;
};

}//zengine

#endif // !Z_CORE_T_FIEXED_QUEUE_H_