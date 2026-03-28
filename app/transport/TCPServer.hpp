#pragma once


#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>

#include "../models/SocketGuard.hpp"
#include "../models/Connectiom.hpp"
#include "../utils/errors.hpp"

class TCPServer
{
    SocketGuard server_fd;
    sockaddr_in addr;

    public:
        TCPServer() : server_fd{-1}, addr{} {}

        void init(const uint16_t port = 8080)
        {
            if(
                server_fd = SocketGuard(socket(AF_INET, SOCK_STREAM, 0));
                server_fd.get() == -1
            ) throw_errno("Init, socket");

            addr.sin_family      = AF_INET;
            addr.sin_port        = htons(port);
            addr.sin_addr.s_addr = INADDR_ANY;

            server_fd.set_options(
                std::make_tuple(SOL_SOCKET ,  SO_REUSEADDR,  1),
                std::make_tuple(IPPROTO_TCP,  TCP_NODELAY,   1),
                std::make_tuple(SOL_SOCKET ,  SO_KEEPALIVE,  1),
                std::make_tuple(IPPROTO_TCP,  TCP_KEEPIDLE,  120),
                std::make_tuple(IPPROTO_TCP,  TCP_KEEPINTVL, 20),
                std::make_tuple(IPPROTO_TCP,  TCP_KEEPCNT,   3)
            );

            if(bind(server_fd.get(), (sockaddr*)&addr, sizeof(addr))<0)
                throw_errno("Init, bind");

            //* Add epoll
        }

        void listen(const int max_half_open_cons)
        {
            //* Nonblock socket whith epoll

            if(::listen(server_fd.get(), max_half_open_cons)<0)
                throw_errno("Listen");

            //* Add epoll
        }

        Connection accept()
        {
            sockaddr_in con_addr{};

            socklen_t con_addr_len = sizeof(con_addr);

            int con_fd;

            if(
                con_fd = ::accept(server_fd.get(), (sockaddr*)&con_addr, &con_addr_len);
                con_fd == -1    
            ) throw_errno("Accept");

            return Connection{SocketGuard(con_fd), con_addr};

        }
};