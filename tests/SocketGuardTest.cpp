#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <iostream>

#include "../app/models/SocketGuard.hpp"

int main()
{
    SocketGuard fd;

    fd = SocketGuard(socket(AF_INET, SOCK_STREAM, 0));

    try{
        fd.set_options(
            std::make_tuple(SOL_SOCKET ,   SO_REUSEADDR, 1),
            std::make_tuple(SOL_SOCKET ,   SO_KEEPALIVE, 1),
            std::make_tuple(IPPROTO_TCP,   TCP_NODELAY,  1)
        );
    }
    catch(std::exception& e){
        std::cerr << e.what() << std::endl;
    }

    int val;
    socklen_t len = sizeof(val);

    getsockopt(fd.get(), SOL_SOCKET, SO_REUSEADDR, &val, &len);

    std::cout << "SO_REUSEADDR: " << val << std::endl;

    getsockopt(fd.get(), SOL_SOCKET, SO_KEEPALIVE, &val, &len);

    std::cout << "SO_KEEPALIVE: " << val << std::endl;

    getsockopt(fd.get(), IPPROTO_TCP, TCP_NODELAY, &val, &len);

    std::cout << "TCP_NODELAY: " << val << std::endl;
}