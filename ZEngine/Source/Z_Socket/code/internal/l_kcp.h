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

#include "kcp/ikcp.h"

namespace zengine {
namespace socket {

class KcpSession : public ZObject {
public:
    using SuperType_ = ZObject;

public:
    KcpSession(UInt32 _conv)
        : SuperType_()
        , kcp_context_ptr_(nullptr)
    {}

    NODISCARD ReturnType Open()
        // 1. 创建 KCP 实例
        // 参数 1: conv (会话ID，双端必须一致)
        // 参数 2: user (用户自定义指针，传 this 指针方便在回调中使用类成员)
        kcp_ = ikcp_create(conv, this);

        // 2. 绑定底层发送回调函数
        ikcp_setoutput(kcp_, &KcpSession::udp_output_callback);

        // 3. 【核心调优】配置为极速模式
        // nodelay: 1 (开启非阻塞/无延迟模式)
        // interval: 10 (内部时钟刷新间隔 10ms，默认 100ms 太慢)
        // resend: 2 (快速重传指标，跨越 2 个 ACK 立即重传，不等到超时)
        // nc: 1 (关闭流控/拥塞控制，在丢包时不减速)
        ikcp_nodelay(kcp_, 1, 10, 2, 1);

        // 4. 【核心调优】设置发送/接收窗口大小
        // 默认是 32，太小容易堵塞；根据网络吞吐需求增大，比如 128 或 256
        ikcp_wndsize(kcp_, 128, 128);

        // 5. 设置 MTU（最大传输单元）
        // 默认 1400，通常保持 1400 即可（避开以太网 1500 限制，防止 IP 层分片）
        ikcp_setmtu(kcp_, 1400);

        // 6. （可选）设置流模式还是报文模式
        // 默认是 0（报文模式，保留消息边界，像 UDP）
        // 如果设置为 1（流模式，像 TCP，会把多次 send 的小包合并，节省包头）
        kcp_->stream = 0;
    }

    ~KcpSession() {
        if (kcp_context_ptr_) {
            ikcp_release(kcp_context_ptr_);
            kcp_context_ptr_ = nullptr;
        }
    }
private:
    // 静态回调函数
    static int udp_output_callback(const char* buf, int len, ikcpcb* kcp, void* user) {
        auto self = static_cast<KcpSession*>(user);
        return self->real_send(buf, len);
    }

    int real_send(const char* buf, int len) {
        // 在这里调用底层 socket 发送 UDP 数据 (比如 boost::asio 或 sendto)
        return 0;
    }

private:
    ikcpcb* kcp_context_ptr_;
};

} // socket
} // zengine