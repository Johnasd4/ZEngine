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
#include "z_ip_address.h"

namespace zengine {
namespace socket {

/**
 * @brief UDP endpoint class.
 * 
 * Implementation Principle: This class encapsulates the underlying asio::ip::udp::endpoint 
 * object from the Asio library. It is designed to provide a unified interface for handling 
 * both IPv4 and IPv6 network endpoints within the engine's UDP socket system, thereby abstracting 
 * away the direct dependencies and details of the Asio networking implementation.
 */
class SOCKET_DLLAPI ZUDPEndpoint {
    NO_NEW_DELETE
public:
    /**
     * @brief Default constructor.
     */
    FORCEINLINE ZUDPEndpoint() noexcept = default;

    /**
     * @brief Copy constructor.
     * @param unnamed The ZUDPEndpoint object to copy from.
     */
    FORCEINLINE ZUDPEndpoint(const ZUDPEndpoint&) noexcept = default;

    /**
     * @brief Move constructor.
     * @param unnamed The ZUDPEndpoint object to move from.
     */
    FORCEINLINE ZUDPEndpoint(ZUDPEndpoint&&) noexcept = default;

    /**
     * @brief Construct a UDP endpoint from an IPv4 address and a port.
     * @param _ip The IPv4 address object.
     * @param _port The port number.
     */
    FORCEINLINE ZUDPEndpoint(ZIP4Address _ip, UInt16 _port) noexcept 
        : endpoint_(_ip.ip_, _port)
    {}

    /**
     * @brief Construct a UDP endpoint from an IPv6 address and a port.
     * @param _ip The IPv6 address object.
     * @param _port The port number.
     */
    FORCEINLINE ZUDPEndpoint(ZIP6Address _ip, UInt16 _port) noexcept 
        : endpoint_(_ip.ip_, _port)
    {}

    /**
     * @brief Construct a UDP endpoint from an ASIO endpoint object.
     * @param _endpoint The ASIO UDP endpoint object to copy from.
     */
    FORCEINLINE ZUDPEndpoint(const asio::ip::udp::endpoint& _endpoint) noexcept
        : endpoint_(_endpoint)
    {}

    /**
     * @brief Construct a UDP endpoint from an ASIO endpoint object.
     * @param _endpoint The ASIO UDP endpoint object to move from.
     */
    FORCEINLINE ZUDPEndpoint(asio::ip::udp::endpoint&& _endpoint) noexcept
        : endpoint_(std::move(_endpoint))
    {}

    /**
     * @brief Destructor.
     */
    FORCEINLINE ~ZUDPEndpoint() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param unnamed The ZUDPEndpoint object to copy from.
     * @return A reference to the assigned ZUDPEndpoint object.
     */
    FORCEINLINE ZUDPEndpoint& operator=(const ZUDPEndpoint&) noexcept = default;

    /**
     * @brief Move assignment operator.
     * @param unnamed The ZUDPEndpoint object to move from.
     * @return A reference to the assigned ZUDPEndpoint object.
     */
    FORCEINLINE ZUDPEndpoint& operator=(ZUDPEndpoint&&) noexcept = default;

    /**
     * @brief Get the IP type of the endpoint.
     * @return The IP type enum indicating whether it is an IPv4 or IPv6 address.
     */
    NODISCARD FORCEINLINE IPTypeEnum GetIPType() const noexcept {
        return endpoint_.protocol() == asio::ip::udp::v6() ? IPTypeEnum::IP6 : IPTypeEnum::IP4;
    }

    /**
     * @brief Get the IPv4 address from the endpoint.
     * @return The IPv4 address associated with this endpoint.
     */
    NODISCARD FORCEINLINE ZIP4Address GetIP4Address() const noexcept {
        ASSERT(endpoint_.protocol() == asio::ip::udp::v4(), "ZUDPEndpoint: Invalid IP type!");
        return ZIP4Address(endpoint_.address().to_v4());
    }

    /**
     * @brief Get the IPv6 address from the endpoint.
     * @return The IPv6 address associated with this endpoint.
     */
    NODISCARD FORCEINLINE ZIP6Address GetIP6Address() const noexcept {
        ASSERT(endpoint_.protocol() == asio::ip::udp::v6(), "ZUDPEndpoint: Invalid IP type!");
        return ZIP6Address(endpoint_.address().to_v6());
    }

    /**
     * @brief Get the port number of the endpoint.
     * @return The port number associated with this endpoint.
     */
    NODISCARD FORCEINLINE UInt16 GetPort() const noexcept {
        return endpoint_.port();
    }

    /**
     * @brief Set the endpoint using an IPv4 address and a port.
     * @param _ip The IPv4 address object.
     * @param _port The port number.
     */
    FORCEINLINE Void SetEndpoint(ZIP4Address _ip, UInt16 _port) noexcept {
        endpoint_.address(_ip.ip_);
        endpoint_.port(_port);
    }

    /**
     * @brief Set the endpoint using an IPv6 address and a port.
     * @param _ip The IPv6 address object.
     * @param _port The port number.
     */
    FORCEINLINE Void SetEndpoint(ZIP6Address _ip, UInt16 _port) noexcept {
        endpoint_.address(_ip.ip_);
        endpoint_.port(_port);
    }

    /**
     * @brief Set the endpoint using an ASIO endpoint object.
     * @param _endpoint The ASIO UDP endpoint object to move from.
     */
    FORCEINLINE Void SetEndpoint(asio::ip::udp::endpoint&& _endpoint) noexcept {
        endpoint_ = std::move(_endpoint);
    }

    /**
     * @brief Get the IP address as a string from the endpoint.
     * @return A ZString containing the IP address in string format.
     */
    NODISCARD ZString GenerateIPString() const noexcept;

private:
    /** @brief Friend declaration to allow ZUDPSocket access to private members. */
    friend class ZUDPSocket;
    /** @brief Friend declaration to allow ZIOContext access to private members. */
    friend class ZIOContext;

private:
    /** @brief The underlying asio udp endpoint instance used to store IP address and port data. */
    asio::ip::udp::endpoint endpoint_;
};

} // socket
} // zengine