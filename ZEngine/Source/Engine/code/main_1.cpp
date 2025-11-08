//#include "I:\ZEngine\ZEngine\Library\Boost_1_88_0\include\boost\asio.hpp"
//#include <iostream>
//#include <string>
//
//using boost::asio::ip::udp;
//
//int main(int argc, char* argv[]) {
//    try {
//
//        // IO
//        boost::asio::io_context io_context;
//
//        // UDP Socket:cite[3]
//        udp::socket socket(io_context);
//        socket.open(udp::v4()); // SocketʹIPv4Э:cite[4]
//
//        boost::asio::ip::address address = boost::asio::ip::make_address("127.0.0.1");
//
//        // ÷˵ (ʹдIP͹̶˿8001):cite[3]
//        udp::endpoint server_endpoint(address, 8001);
//
//        // ׼Ҫ͵Ϣ
//        std::string message = "Hello, UDP Server!";
//
//        // ͬݵ:cite[3]
//        socket.send_to(boost::asio::buffer(message), server_endpoint);
//        std::cout << "Sent message to server: " << message << std::endl;
//
//        // ׼շĻظ:cite[3]
//        char recv_buf[1024];
//        udp::endpoint sender_endpoint;
//
//        // ͬ:cite[3]
//        size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
//
//        // ʾظ
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