//#include "I:\ZEngine\ZEngine\Library\Boost_1_88_0\include\boost\asio.hpp"
//#include <iostream>
//#include <string>
//
//using boost::asio::ip::udp;
//
//int main(int argc, char* argv[]) {
//    try {
//
//        // 创建IO服务对象
//        boost::asio::io_context io_context;
//
//        // 创建UDP Socket:cite[3]
//        udp::socket socket(io_context);
//        socket.open(udp::v4()); // 打开Socket，使用IPv4协议:cite[4]
//
//        boost::asio::ip::address address = boost::asio::ip::make_address("127.0.0.1");
//
//        // 设置服务器端点 (使用命令行传入的IP和固定端口8001):cite[3]
//        udp::endpoint server_endpoint(address, 8001);
//
//        // 准备要发送的消息
//        std::string message = "Hello, UDP Server!";
//
//        // 同步发送数据到服务器:cite[3]
//        socket.send_to(boost::asio::buffer(message), server_endpoint);
//        std::cout << "Sent message to server: " << message << std::endl;
//
//        // 准备接收服务器的回复:cite[3]
//        char recv_buf[1024];
//        udp::endpoint sender_endpoint;
//
//        // 同步接收数据:cite[3]
//        size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
//
//        // 显示回复
//        std::string reply(recv_buf, len);
//        std::cout << "Received from server: " << reply << std::endl;
//
//    }
//    catch (std::exception& e) {
//        std::cerr << "Exception: " << e.what() << std::endl;
//    }
//
//    return 0;
//}