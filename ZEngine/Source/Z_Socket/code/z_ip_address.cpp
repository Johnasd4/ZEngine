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
#define SOCKET_DLLFILE
#include "drive/d_pch.h"

#include "z_ip_address.h"

namespace zengine {
namespace socket {

NODISCARD TOptional<ZIP4Address> ZIP4Address::MakeAddress(ZStringView _ip_str) noexcept {
    std::error_code error_code;

    TOptional<ZIP4Address> address(asio::ip::make_address_v4(std::string_view(_ip_str), error_code));
    if (error_code) {
        address = ReturnType(error_code::kZAddressErrorCode_IPNotValid);
        Z_LOG_ERROR(
            error_code::kZAddressErrorCode_IPNotValid, 0,
            "IP not valid! _ip_str: {}",
            _ip_str
        );
    }

    return address;
}

NODISCARD ZString ZIP4Address::GenerateIPString() const noexcept {
    // asio::ip::address_v4::bytes_type is std::array<unsigned char, 4>
    const auto ip_bytes = ip_.to_bytes();

    // 15 for ip, 1 for null terminator
    constexpr SizeType kMaxIPStringLength = 16;

    // use stack buffer to avoid heap allocation, since IP string is at most 15 chars + null terminator
    Char buffer[kMaxIPStringLength];
    Char* char_ptr = buffer;

    // quick lambda to append a byte as decimal string to the buffer
    auto append_byte = [&char_ptr](Byte _number) {
        if (_number >= 100) {
            *char_ptr++ = '0' + (_number / 100);
            _number %= 100;
            *char_ptr++ = '0' + (_number / 10);
            _number %= 10;
        }
        else if (_number >= 10) {
            *char_ptr++ = '0' + (_number / 10);
            _number %= 10;
        }
        *char_ptr++ = '0' + _number;
    };

    // Append each byte and dots
    append_byte(ip_bytes[0]);
    *char_ptr++ = '.';
    append_byte(ip_bytes[1]);
    *char_ptr++ = '.';
    append_byte(ip_bytes[2]);
    *char_ptr++ = '.';
    append_byte(ip_bytes[3]);

    // Adds null terminator
    *char_ptr = '\0';

    // Calculate actual length
    const SizeType length = static_cast<SizeType>(char_ptr - buffer);

    // Directly construct ZString from buffer
    // Assume ZString has a (const char* str, size_t len) constructor, this is the most efficient
    // If not, ZString(buffer) is also fine, because we added '\0' at the end
    return ZString(buffer, length);
}

NODISCARD TOptional<ZIP6Address> ZIP6Address::MakeAddress(ZStringView _ip_str) noexcept {
    std::error_code error_code;

    TOptional<ZIP6Address> address(asio::ip::make_address_v6(std::string_view(_ip_str), error_code));
    if (error_code) {
        address = ReturnType(error_code::kZAddressErrorCode_IPNotValid);
        Z_LOG_ERROR(
            error_code::kZAddressErrorCode_IPNotValid, 0,
            "IP not valid! _ip_str: {}",
            _ip_str
        );
    }

    return address;
}

NODISCARD ZString ZIP6Address::GenerateIPString() const noexcept {
    const auto ip_bytes = ip_.to_bytes();

    // Standard IPv6 format (uncompressed): xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx
    // 32 hex digits + 7 colons + 1 null terminator = 40 chars.
    constexpr SizeType kMaxIPStringLength = 40ULL;

    Char buffer[kMaxIPStringLength];
    Char* char_ptr = buffer;

    auto append_byte = [&char_ptr](Byte _first_number, Byte _second_number) {
        // Use static to avoid re-initialization on stack (though compiler optimizes it)
        static const Char hex_digits[] = "0123456789abcdef";
        *char_ptr++ = hex_digits[_first_number >> 4];
        *char_ptr++ = hex_digits[_first_number & 0x0f];
        *char_ptr++ = hex_digits[_second_number >> 4];
        *char_ptr++ = hex_digits[_second_number & 0x0f];
    };

    // Append each byte and colons
    append_byte(ip_bytes[0], ip_bytes[1]);
    *char_ptr++ = ':';
    append_byte(ip_bytes[2], ip_bytes[3]);
    *char_ptr++ = ':';
    append_byte(ip_bytes[4], ip_bytes[5]);
    *char_ptr++ = ':';
    append_byte(ip_bytes[6], ip_bytes[7]);
    *char_ptr++ = ':';
    append_byte(ip_bytes[8], ip_bytes[9]);
    *char_ptr++ = ':';
    append_byte(ip_bytes[10], ip_bytes[11]);
    *char_ptr++ = ':';
    append_byte(ip_bytes[12], ip_bytes[13]);
    *char_ptr++ = ':';
    append_byte(ip_bytes[14], ip_bytes[15]);

    // Adds null terminator
    *char_ptr = '\0';

    const SizeType length = static_cast<SizeType>(char_ptr - buffer);
    return ZString(buffer, length);
}

} // socket
} // zengine