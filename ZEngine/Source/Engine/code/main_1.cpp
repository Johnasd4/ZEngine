//#include "I:\ZEngine\ZEngine\Library\Boost_1_88_0\include\boost\asio.hpp"
//#include <iostream>
//#include <string>
//
//using boost::asio::ip::udp;
//
//class UDPServer {
//public:
//    UDPServer(boost::asio::io_context& io_context, unsigned short port)
//        : socket_(io_context, udp::endpoint(udp::v4(), port)) {
//        start_receive();
//    }
//
//private:
//    void start_receive() {
//        socket_.async_receive_from(
//            boost::asio::buffer(recv_buffer_), remote_endpoint_,
//            [this](boost::system::error_code ec, std::size_t bytes_recvd) {
//                handle_receive(ec, bytes_recvd);
//            });
//    }
//
//    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred) {
//        if (!error) {
//            std::string message = std::string(recv_buffer_.data(), bytes_transferred);
//            std::cout << "Received from " << remote_endpoint_.address().to_string()
//                << ":" << remote_endpoint_.port() << " - " << message << std::endl;
//
//            // 回显消息给客户端
//            response = "Echo: " + message;
//            socket_.async_send_to(
//                boost::asio::buffer(response), remote_endpoint_,
//                [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/) {
//                    start_receive();  // 继续接收下一条消息
//                });
//        }
//        else {
//            std::cerr << "Receive error: " << error.message() << std::endl;
//            start_receive();  // 即使出错也继续接收
//        }
//    }
//    std::string response;
//    udp::socket socket_;
//    udp::endpoint remote_endpoint_;
//    std::array<char, 1024> recv_buffer_;
//};
//
//int main() {
//    try {
//        boost::asio::io_context io_context;
//        UDPServer server(io_context, 12345);
//
//        std::cout << "UDP Server started on port 12345..." << std::endl;
//        io_context.run();
//    }
//    catch (std::exception& e) {
//        std::cerr << "Exception: " << e.what() << std::endl;
//    }
//
//    return 0;
//}