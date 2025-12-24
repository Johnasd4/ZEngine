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

#include <thread>

#include "t_tuple.h"
#include "z_object.h"

namespace zengine {

/**
 * @brief Thread class wrapper inheriting from ZObject.
 *
 * This class encapsulates a standard jthread and provides custom memory management for task arguments.
 * Basic type's reference will be removed. Basic type includes fundamental types, pointer types and enum types.
 */
class ZThread : public ZObject<> {
public:
    /** @brief Alias for the standard thread ID type. */
    using ThreadIDType_ = std::thread::id;

    /**
     * @brief Retrieves the ID of the currently executing thread.
     * @return ThreadIDType_ The unique identifier of the current thread.
     */
    NODISCARD FORCEINLINE static ThreadIDType_ GetCurrnetThreadID() noexcept { return std::this_thread::get_id(); }

    /**
     * @brief Default constructor. Creates a thread object that represents no thread of execution.
     */
    FORCEINLINE ZThread() noexcept 
        : SuperType_()
        , thread_()
    {}

    /**
     * @brief Move constructor. Transfers ownership of the thread from another ZThread object.
     * @param _thread The ZThread object to move from.
     */
    FORCEINLINE ZThread(ZThread&& _thread) noexcept
        : SuperType_()
        , thread_(std::move(_thread.thread_))
    {}

    /**
     * @brief Constructs a thread and executes the given function with arguments.
     *
     * This constructor allocates a task tuple using ZObject's memory pool (operator new), ensuring arguments
     * are stored and their lifetimes are managed correctly across the thread boundary.
     * The task memory is automatically freed (operator delete) after execution.
     *
     * @tparam _Function The type of the function to execute.
     * @tparam _ArgsType The types of the arguments.
     * @param _func The function to execute.
     * @param _args The arguments to pass to the function.
     */
    template <typename _Function, typename... _ArgsType>
    ZThread(_Function&& _func, _ArgsType&&... _args) noexcept
        : SuperType_() 
    {
        // Define the task data type (Function + Arguments Tuple)
        using ParamsTuple = TTuple<kRemoveBasicTypeReferenceType<_ArgsType>...>;
        using TaskType = TTuple<std::decay_t<_Function>, ParamsTuple>;

        // Allocate memory from the custom pool (ZObject::operator new)
        TaskType* task_ptr = new TaskType(
            std::forward<_Function>(_func),
            tuple::MakeTuple(NonBasicTypeForward(std::forward<_ArgsType>(_args))...)
        );

        try {
            // Start the thread
            thread_ = std::jthread([task_ptr]() {
                //Execute the task
                tuple::Apply(task_ptr->template Get<0>(), std::move(task_ptr->template Get<1>()));

                // Destructs the tuple (releasing resources held by args) 
                // and frees memory back to the pool (ZObject::operator delete).
                delete task_ptr;
                });
        }
        catch (...) {
            // If thread creation fails, we must manually free the allocated memory
            delete task_ptr;
        }
    }

    /**
     * @brief Destructor. Automatically joins the thread if it uses jthread semantics.
     */
    FORCEINLINE ~ZThread() noexcept {}

    /**
     * @brief Move assignment operator. Transfers ownership of the thread.
     * @param _thread The ZThread object to move from.
     * @return ZThread& Reference to this object.
     */
    FORCEINLINE ZThread& operator=(ZThread&& _thread) noexcept {
        thread_ = std::move(_thread.thread_);
        return *this;
    }

    /**
     * @brief Three-way comparison operator for thread IDs.
     * @param _thread The ZThread object to compare with.
     * @return std::strong_ordering The ordering relationship between thread IDs.
     */
    NODISCARD FORCEINLINE std::strong_ordering operator<=>(const ZThread& _thread) noexcept {
        return thread_.get_id() <=> _thread.thread_.get_id();
    }

    /**
     * @brief Retrieves the ID of the wrapped thread.
     * @return ThreadIDType_ The unique identifier of the thread.
     */
    NODISCARD FORCEINLINE ThreadIDType_ GetID() const noexcept { return thread_.get_id(); }

    /**
     * @brief Checks if the thread is joinable.
     * @return Bool True if the thread is joinable, False otherwise.
     */
    NODISCARD FORCEINLINE Bool IsJoinable() noexcept { return thread_.joinable(); }

    /**
     * @brief Waits for the thread to finish execution.
     */
    FORCEINLINE Void Join() noexcept { thread_.join(); }

    /**
     * @brief Detaches the thread, allowing it to execute independently.
     */
    FORCEINLINE Void Detach() noexcept { thread_.detach(); }

    /**
     * @brief Swaps the underlying thread handle with another ZThread object.
     * @param _thread The ZThread object to swap with.
     */
    FORCEINLINE Void Swap(ZThread& _thread) noexcept { thread_.swap(_thread.thread_); }

protected:
    /** @brief Alias for the base class. */
    using SuperType_ = ZObject;

private:
    /** @brief Deleted copy constructor. */
    ZThread(const ZThread&) = delete;
    /** @brief Deleted copy assignment operator. */
    ZThread& operator=(const ZThread&) = delete;

    /** @brief The underlying standard jthread object. */
    std::jthread thread_;
};

}//zengine