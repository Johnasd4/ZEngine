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

#ifdef SOCKET_DLLFILE
#define IS_DICONNECT_ERROR(_error_code)\
    ( \
        (_error_code) == asio::error::eof || \
        (_error_code) == asio::error::broken_pipe || \
        (_error_code) == asio::error::operation_aborted || \
        (_error_code) == asio::error::not_connected || \
        (_error_code) == asio::error::timed_out || \
        (_error_code) == asio::error::network_down || \
        (_error_code) == asio::error::network_reset || \
        (_error_code) == asio::error::host_unreachable || \
        (_error_code) == asio::error::connection_aborted || \
        (_error_code) == asio::error::interrupted || \
        (_error_code) == asio::error::connection_reset \
    )

#define IS_RETRY_ERROR(_error_code)\
    ( \
        (_error_code) == asio::error::would_block || \
        (_error_code) == asio::error::try_again \
    )

#define IS_SHUTDOWN_ERROR(_error_code)\
    ( \
        (_error_code) == asio::error::shut_down || \
        (_error_code) == asio::error::eof || \
        (_error_code) == asio::error::connection_reset || \
        (_error_code) == std::errc::invalid_argument \
    )

#endif
