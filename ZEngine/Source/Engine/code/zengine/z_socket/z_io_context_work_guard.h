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

#include "../z_core/z_object.h"

#include "internal/l_asio.h"
#include "z_io_context.h"

namespace zengine {
namespace socket {

/**
 * @brief IO context work guard.
 *
 * This class prevents the associated `ZIOContext` (and its underlying `io_context`) from stopping its run loop
 * when there are no pending asynchronous operations. It encapsulates `asio::executor_work_guard` to maintain
 * work ownership. When this object is alive, the `io_context::run()` function will not return.
 *
 * Implementation Principle:
 * It holds an `asio::executor_work_guard` initialized with the executor of the provided `ZIOContext`.
 * This acts as an RAII wrapper: construction acquires work ownership, and destruction (or reset) releases it.
 */
class SOCKET_DLLAPI ZIOContextWorkGuard : public ZObject {
public:
    /** @brief Alias for the base class type. */
    using SuperType_ = ZObject;

public:
    /**
     * @brief Construct a new ZIOContextWorkGuard object.
     *
     * Initializes the internal work guard using the executor from the provided `ZIOContext`.
     *
     * @param _io_context Reference to the ZIOContext to be guarded.
     */
    FORCEINLINE ZIOContextWorkGuard(ZIOContext& _io_context) noexcept 
        : SuperType_()
        , work_guard_(_io_context.io_context_.get_executor())
    {}

    /**
     * @brief Move constructor.
     *
     * Transfers ownership of the work guard from another instance.
     *
     * @param _other The other ZIOContextWorkGuard instance to move from.
     */
    FORCEINLINE ZIOContextWorkGuard(ZIOContextWorkGuard&&) noexcept = default;

    /**
     * @brief Destructor.
     *
     * Releases the work guard. If this is the last guard, `io_context::run()` may return.
     */
    FORCEINLINE ~ZIOContextWorkGuard() noexcept = default;

    /**
     * @brief Move assignment operator.
     *
     * Transfers ownership of the work guard from another instance to this one.
     *
     * @param _other The other ZIOContextWorkGuard instance to move from.
     * @return ZIOContextWorkGuard& Reference to this instance.
     */
    FORCEINLINE ZIOContextWorkGuard& operator=(ZIOContextWorkGuard&&) noexcept = default;

    /**
     * @brief Reset the work guard.
     *
     * Explicitly releases the ownership of the work. After calling this, the `io_context` may stop running
     * if there are no other tasks or guards.
     *
     * @return Void
     */
    FORCEINLINE Void Reset() noexcept { work_guard_.reset(); }

private:
    /** @brief The internal ASIO executor work guard that keeps the IO context alive. */
    asio::executor_work_guard<asio::io_context::executor_type> work_guard_;
};

} // socket
} // zengine