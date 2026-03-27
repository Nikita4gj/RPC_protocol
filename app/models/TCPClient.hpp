#pragma once


#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>

#include<string_view>
#include<string>

#include "SocketGuard.hpp"
#include "../utils/errors.hpp"

class TCPClient
{
    SocketGuard client_fd;

    public:
        TCPClient() : client_fd{-1} {}

        void connect(std::string_view server_ip = "127.0.0.1", const uint16_t port = 8080)
        {
            addrinfo hints{}, *res;

            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            
            if(getaddrinfo(server_ip.data(), std::to_string(port).c_str(), &hints, &res)!=0)
                throw_gaierror("Connect, getaddrinfo");

            bool connected = false;

            for(auto it = res; it != nullptr && !connected; it = it->ai_next)
            {
                SocketGuard temp_fd {socket(it->ai_family, it->ai_socktype, 0)};

                if(::connect(temp_fd.get(), it->ai_addr, it->ai_addrlen)==0)
                {
                    connected = true;
                    client_fd = std::move(temp_fd);
                }
            }

            freeaddrinfo(res);

            if(!connected)
                throw_errno("Failed to connect to the server");

            client_fd.set_options(
                std::make_tuple(IPPROTO_TCP,  TCP_NODELAY,   1),
                std::make_tuple(SOL_SOCKET ,  SO_KEEPALIVE,  1),
                std::make_tuple(IPPROTO_TCP,  TCP_KEEPIDLE,  120),
                std::make_tuple(IPPROTO_TCP,  TCP_KEEPINTVL, 20),
                std::make_tuple(IPPROTO_TCP,  TCP_KEEPCNT,   3)
            );
        }
};

