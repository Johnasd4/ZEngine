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

#include "z_udp_packet/z_udp_packet_continue_sending_message.h"
#include "z_udp_packet/z_udp_packet_dicard_message_by_id.h"
#include "z_udp_packet/z_udp_packet_echo.h"
#include "z_udp_packet/z_udp_packet_message_package.h"
#include "z_udp_packet/z_udp_packet_message_first_slice.h"
#include "z_udp_packet/z_udp_packet_message_last_slice.h"
#include "z_udp_packet/z_udp_packet_message_slice.h"
#include "z_udp_packet/z_udp_packet_p2p_connect.h"
#include "z_udp_packet/z_udp_packet_p2p_keep_alive.h"
#include "z_udp_packet/z_udp_packet_p2p_shutdown.h"
#include "z_udp_packet/z_udp_packet_pause_sending_message.h"
#include "z_udp_packet/z_udp_packet_receive_response.h"
#include "z_udp_packet/z_udp_packet_resend_request.h"
#include "z_udp_packet/z_udp_packet_set_transfer_speed_limit.h"
#include "z_udp_packet/z_udp_packet_start_sending_message.h"
#include "z_udp_packet/z_udp_packet_stop_sending_message.h"