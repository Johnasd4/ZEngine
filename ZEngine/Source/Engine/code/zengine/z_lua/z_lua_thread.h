/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "drive.h"

#include "../z_core/z_string_view.h"

#include "internal/l_sol.h"
#include "z_lua_object.h"
#include "z_lua_state_view.h"

namespace zengine {
namespace lua {

/**
 * @brief A wrapper class for Lua threads (coroutines).
 * @details This class wraps the underlying `sol::thread` to provide object-oriented management of Lua coroutines.
 *          A Lua thread represents an independent execution stack that shares the global environment (Registry, _G)
 *          with the main state.
 *          Implementation Principle: This class holds a reference to the Lua thread object within the Lua registry.
 *          It distinguishes between the "Parent State" (the VM owner) and the "Thread State" (the specific execution
 *          stack). The lifecycle of the C++ object manages the reference count, while the Lua GC manages the actual
 *          coroutine memory.
 */
class LUA_DLLAPI ZLuaThread : public ZObject {
public:
    /** @brief Alias for the base class. */
    using SuperType_ = ZObject;

    /**
     * @brief Enumeration defining the execution status of a Lua thread.
     */
    enum class StatusEnum_ : Int32 {
        /** @brief The thread is in a normal state (running or ready). */
        kOK = sol::thread_status::ok,
        /** @brief The thread is currently yielded (suspended). */
        kYielded = sol::thread_status::yielded,
        /** @brief A runtime error occurred during execution. */
        kError_Runtime = sol::thread_status::runtime,
        /** @brief A memory allocation error occurred. */
        kError_Memory = sol::thread_status::memory,
        /** @brief An error occurred during garbage collection. */
        kError_GC = sol::thread_status::gc,
        /** @brief An error occurred in the message handler. */
        kError_Handler = sol::thread_status::handler,
        /** @brief The thread is dead or invalid. */
        kDead = sol::thread_status::dead,
    };

public:
    /**
     * @brief Creates a new ZLuaThread instance within the specified Lua state.
     * @param _lua_state The raw Lua state pointer where the thread will be created.
     * @return A new ZLuaThread instance. Returns an empty instance if the input state is null.
     */
    NODISCARD FORCEINLINE static ZLuaThread Create(RawLuaState* _lua_state) {
        if (!_lua_state) {
            return ZLuaThread();
        }
        return ZLuaThread(sol::thread::create(_lua_state));
    }

    /**
     * @brief Default constructor.
     * @details Creates an empty/invalid ZLuaThread.
     */
    FORCEINLINE ZLuaThread() noexcept = default;

    /**
     * @brief Copy constructor.
     * @param other The source object to copy from.
     */
    FORCEINLINE ZLuaThread(const ZLuaThread&) noexcept = default;

    /**
     * @brief Move constructor.
     * @param other The source object to move from.
     */
    FORCEINLINE ZLuaThread(ZLuaThread&&) noexcept = default;

    /**
     * @brief Explicit constructor from a raw sol::thread reference.
     * @param _thread The sol::thread object to wrap.
     */
    FORCEINLINE ZLuaThread(const sol::thread& _thread) noexcept
        : SuperType_()
        , thread_(_thread)
    {}

    /**
     * @brief Explicit constructor from a raw sol::thread rvalue.
     * @param _thread The sol::thread object to move into this wrapper.
     */
    FORCEINLINE ZLuaThread(sol::thread&& _thread) noexcept
        : SuperType_()
        , thread_(std::move(_thread))
    {}

    /**
     * @brief Explicit constructor from a generic ZLuaObject (copy).
     * @param _obj The generic Lua object, which must hold a thread.
     */
    FORCEINLINE explicit ZLuaThread(const ZLuaObject& _obj) noexcept 
        : SuperType_() 
        , thread_()
    {
        ASSERT(_obj.IsValid() && _obj.obj_.is<sol::thread>(), "ZLuaObject is not a thread!");
        thread_ = _obj.obj_;
    }

    /**
     * @brief Explicit constructor from a generic ZLuaObject (move).
     * @param _obj The generic Lua object, which must hold a thread.
     */
    FORCEINLINE explicit ZLuaThread(ZLuaObject&& _obj) noexcept 
        : SuperType_() 
        , thread_()
    {
        ASSERT(_obj.IsValid() && _obj.obj_.is<sol::thread>(), "ZLuaObject is not a thread!");
        thread_ = std::move(_obj.obj_);
    }

    /**
     * @brief Destructor.
     */
    FORCEINLINE ~ZLuaThread() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param other The source object to copy from.
     * @return Reference to this object.
     */
    FORCEINLINE ZLuaThread& operator=(const ZLuaThread&) noexcept = default;

    /**
     * @brief Move assignment operator.
     * @param other The source object to move from.
     * @return Reference to this object.
     */
    FORCEINLINE ZLuaThread& operator=(ZLuaThread&&) noexcept = default;

    /**
     * @brief Implicit conversion operator to ZLuaStateView.
     * @return A ZLuaStateView representing this Lua thread.
     */
    FORCEINLINE operator ZLuaStateView() noexcept {
        return ZLuaStateView(thread_.lua_state());
    }

    /**
     * @brief Checks if the object is valid (not null/nil).
     * @return True if the object refers to a valid Lua thread, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsValid() const noexcept {
        return thread_.valid();
    }

    /**
     * @brief Checks if the thread is currently yielded (suspended).
     * @return True if the status is yielded, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsYielded() const noexcept {
        return thread_.status() == sol::thread_status::yielded;
    }
    
    /**
     * @brief Checks if the thread is in a runnable state.
     * @details A runnable thread is either OK (running/ready) or Yielded (can be resumed).
     * @return True if the thread is runnable, false otherwise.
     */
    NODISCARD FORCEINLINE Bool IsRunnable() const noexcept {
        auto s = thread_.status();
        return s == sol::thread_status::ok || s == sol::thread_status::yielded;
    }

    /**
     * @brief Checks if this object wraps the main Lua thread.
     * @return True if this is the main thread, false if it is a coroutine.
     */
    NODISCARD FORCEINLINE Bool IsMainThread() const noexcept {
        return thread_.is_main_thread();
    }

    /**
     * @brief Gets the raw Lua state pointer of the parent VM.
     * @details This returns the main state that owns the global registry, not the specific stack of this thread.
     * @return The lua_State* pointer of the parent state.
     */
    NODISCARD FORCEINLINE RawLuaState* GetParentLuaStateRaw() const noexcept {
        return thread_.lua_state();
    }

    /**
     * @brief Gets the raw Lua state pointer of this specific thread.
     * @details This returns the specific stack associated with this coroutine. Use this for operations involving
     *          this thread's local stack (e.g., resuming).
     * @return The lua_State* pointer of the thread/coroutine.
     */
    NODISCARD FORCEINLINE RawLuaState* GetThreadLuaStateRaw() const noexcept {
        return thread_.thread_state();
    }

    /**
     * @brief Gets the current status of the thread.
     * @return The StatusEnum_ representing the thread's state (OK, Yielded, Dead, etc.).
     */
    NODISCARD FORCEINLINE StatusEnum_ GetStatus() const noexcept {
        return static_cast<StatusEnum_>(thread_.status());
    }

    /**
     * @brief Converts the thread status and pointer to a string representation.
     * @return A string describing the thread.
     */
    NODISCARD ZString ToString() const noexcept;

private:
    /** @brief The internal sol::thread object that manages the Lua coroutine reference. */
    sol::thread thread_;
};

} // lua
} // zengine

// ZLuaThread to string formatter specialization for fmt library
template <>
struct fmt::formatter<zengine::lua::ZLuaThread> : fmt::formatter<std::string_view> {
    auto format(const zengine::lua::ZLuaThread& _thread, format_context& _context) const {
        return fmt::formatter<std::string_view>::format(_thread.ToString().STDString(), _context);
    }
};