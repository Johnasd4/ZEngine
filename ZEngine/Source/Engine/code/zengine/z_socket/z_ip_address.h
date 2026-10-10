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

#include "../z_core/t_optional.h"
#include "../z_core/z_object.h"
#include "../z_core/z_string_view.h"

#include "internal/l_asio.h"

namespace zengine {
namespace error_code {

enum ZAddressErrorCodeEnum : ReturnType::ValueType {
    kZAddressErrorCode_LinkError = kErrorCodeBase_ZAddress,
    kZAddressErrorCode_SystemOrLibraryError,
    kZAddressErrorCode_NullptrParam,
    kZAddressErrorCode_ParamOutOfRange,
    kZAddressErrorCode_IPNotValid
};

} // zengine
} // error_code

namespace zengine {
namespace socket {

/**
 * @brief A wrapper class for IPv4 addresses.
 *
 * This class encapsulates `asio::ip::address_v4` to provide a unified interface within the ZEngine ecosystem.
 * It provides efficient memory management by disabling heap allocation via `operator new`, forcing usage on the stack
 * or as a member variable. It supports conversion from strings, raw integers, and ASIO types.
 * String generation is optimized using stack buffers to avoid unnecessary heap allocations.
 */
class SOCKET_DLLAPI ZIP4Address {
    NO_NEW_DELETE
public:
    /** @brief Alias for the underlying raw address type (32-bit integer). */
    using AddressType = UInt32;

public:
    /**
     * @brief Creates a ZIP4Address from a string representation.
     * @param _ip_str The string view containing the IPv4 address (e.g., "192.168.1.1").
     * @return TOptional containing the address object on success, or an error code if the string is invalid.
     */
    NODISCARD static TOptional<ZIP4Address> MakeAddress(ZStringView _ip_str) noexcept;

    /** @brief Default constructor. */
    FORCEINLINE ZIP4Address() noexcept = default;

    /**
     * @brief Copy constructor.
     * @param other The source ZIP4Address to copy.
     */
    FORCEINLINE ZIP4Address(const ZIP4Address&) noexcept = default;

    /**
     * @brief Constructs an address from a raw 32-bit integer.
     * @param _ip The 32-bit integer representing the IP address.
     */
    FORCEINLINE ZIP4Address(AddressType _ip) noexcept : ip_(_ip) {}

    /**
     * @brief Constructs an address from an ASIO address object.
     * @param _ip The ASIO IPv4 address object.
     */
    FORCEINLINE ZIP4Address(asio::ip::address_v4 _ip) noexcept : ip_(_ip) {}

    /** @brief Destructor. */
    FORCEINLINE ~ZIP4Address() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param other The source ZIP4Address to assign.
     * @return A reference to this object.
     */
    FORCEINLINE ZIP4Address& operator=(const ZIP4Address&) noexcept = default;

    /**
     * @brief Assigns a raw 32-bit integer to the address.
     * @param _ip The 32-bit integer representing the IP address.
     * @return A reference to this object.
     */
    FORCEINLINE ZIP4Address& operator=(AddressType _ip) noexcept {
        ip_ = asio::ip::address_v4(_ip); 
        return *this;
    }

    /**
     * @brief Assigns an ASIO address object to this address.
     * @param _ip The ASIO IPv4 address object.
     * @return A reference to this object.
     */
    FORCEINLINE ZIP4Address& operator=(asio::ip::address_v4 _ip) noexcept { 
        ip_ = _ip; 
        return *this;
    }

    /**
     * @brief Sets the IP address from another ZIP4Address.
     * @param _ip The source ZIP4Address.
     * @return Void.
     */
    FORCEINLINE Void SetIP(const ZIP4Address& _ip) noexcept { ip_ = _ip.ip_; }

    /**
     * @brief Sets the IP address by moving from another ZIP4Address.
     * @param _ip The source ZIP4Address to move from.
     * @return Void.
     */
    FORCEINLINE Void SetIP(ZIP4Address&& _ip) noexcept { ip_ = std::move(_ip.ip_); }

    /**
     * @brief Sets the IP address using a raw 32-bit integer.
     * @param _ip The 32-bit integer representing the IP address.
     * @return Void.
     */
    FORCEINLINE Void SetIP(AddressType _ip) noexcept { ip_ = asio::ip::address_v4(_ip); }

    /**
     * @brief Sets the IP address using an ASIO address object.
     * @param _ip The ASIO IPv4 address object.
     * @return Void.
     */
    FORCEINLINE Void SetIP(asio::ip::address_v4 _ip) noexcept { ip_ = asio::ip::address_v4(_ip); }

    /**
     * @brief Generates a string representation of the IPv4 address.
     * @return A ZString containing the address in dot-decimal notation (e.g., "127.0.0.1").
     */
    NODISCARD ZString GenerateIPString() const noexcept;

private:
    friend class ZUDPEndpoint;
    friend class ZTCPEndpoint;

private:
    /** @brief The underlying ASIO IPv4 address storage. */
    asio::ip::address_v4 ip_;
};

/**
 * @brief A wrapper class for IPv6 addresses.
 *
 * This class encapsulates `asio::ip::address_v6`. It handles 128-bit addresses using `TFixedArray<Byte, 16>`
 * for raw data interaction, ensuring strict type safety. Like its IPv4 counterpart, it disables heap allocation
 * via `operator new` to enforce stack usage. The string generation is optimized to use stack memory, avoiding
 * heap allocations during the formatting process.
 */
class SOCKET_DLLAPI ZIP6Address {
    NO_NEW_DELETE
public:
    /** @brief Alias for the underlying raw address type (16-byte fixed array). */
    using AddressType = TFixedArray<Byte, 16>;

public:
    /**
     * @brief Creates a ZIP6Address from a string representation.
     * @param _ip_str The string view containing the IPv6 address (e.g., "2001:db8::1").
     * @return TOptional containing the address object on success, or an error code if the string is invalid.
     */
    NODISCARD static TOptional<ZIP6Address> MakeAddress(ZStringView _ip_str) noexcept;

    /** @brief Default constructor. */
    FORCEINLINE ZIP6Address() noexcept = default;

    /**
     * @brief Copy constructor.
     * @param other The source ZIP6Address to copy.
     */
    FORCEINLINE ZIP6Address(const ZIP6Address&) noexcept = default;

    /**
     * @brief Constructs an address from a fixed 16-byte array.
     * @param _ip The fixed array containing the 128-bit IPv6 address.
     */
    FORCEINLINE ZIP6Address(AddressType _ip) noexcept : ip_(_ip.GetSTDFixedArray()) {}

    /**
     * @brief Constructs an address from an ASIO address object.
     * @param _ip The ASIO IPv6 address object.
     */
    FORCEINLINE ZIP6Address(asio::ip::address_v6 _ip) noexcept : ip_(_ip) {}

    /** @brief Destructor. */
    FORCEINLINE ~ZIP6Address() noexcept = default;

    /**
     * @brief Copy assignment operator.
     * @param other The source ZIP6Address to assign.
     * @return A reference to this object.
     */
    FORCEINLINE ZIP6Address& operator=(const ZIP6Address&) noexcept = default;

    /**
     * @brief Assigns a fixed 16-byte array to the address.
     * @param _ip The fixed array containing the 128-bit IPv6 address.
     * @return A reference to this object.
     */
    FORCEINLINE ZIP6Address& operator=(AddressType _ip) noexcept {
        ip_ = asio::ip::address_v6(_ip.GetSTDFixedArray());
        return *this;
    }

    /**
     * @brief Assigns an ASIO address object to this address.
     * @param _ip The ASIO IPv6 address object.
     * @return A reference to this object.
     */
    FORCEINLINE ZIP6Address& operator=(asio::ip::address_v6 _ip) noexcept {
        ip_ = _ip;
        return *this;
    }

    /**
     * @brief Sets the IP address from another ZIP6Address.
     * @param _ip The source ZIP6Address.
     * @return Void.
     */
    FORCEINLINE Void SetIP(const ZIP6Address& _ip) noexcept { ip_ = _ip.ip_; }

    /**
     * @brief Sets the IP address by moving from another ZIP6Address.
     * @param _ip The source ZIP6Address to move from.
     * @return Void.
     */
    FORCEINLINE Void SetIP(ZIP6Address&& _ip) noexcept { ip_ = std::move(_ip.ip_); }

    /**
     * @brief Sets the IP address using a fixed 16-byte array.
     * @param _ip The fixed array containing the 128-bit IPv6 address.
     * @return Void.
     */
    FORCEINLINE Void SetIP(AddressType _ip) noexcept { ip_ = asio::ip::address_v6(_ip.GetSTDFixedArray()); }

    /**
     * @brief Sets the IP address using an ASIO address object.
     * @param _ip The ASIO IPv6 address object.
     * @return Void.
     */
    FORCEINLINE Void SetIP(asio::ip::address_v6 _ip) noexcept { ip_ = asio::ip::address_v6(_ip); }

    /**
     * @brief Generates a string representation of the IPv6 address.
     * @return A ZString containing the hexadecimal notation (e.g., "2001:0db8:...").
     */
    NODISCARD ZString GenerateIPString() const noexcept;

private:
    friend class ZUDPEndpoint;
    friend class ZTCPEndpoint;

private:
    /** @brief The underlying ASIO IPv6 address storage. */
    asio::ip::address_v6 ip_;
};

} // socket
} // zengine