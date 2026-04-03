#pragma  once


#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>

#include "../models/SocketGuard.hpp"
#include "../models/Connection.hpp"
#include "../utils/errors.hpp"


namespace rpc::net
{
    using rpc::models::SocketGuard;

    class TCPServer
    {
        SocketGuard _server_fd;
        sockaddr_in _addr;
    
        public:
            TCPServer() noexcept : _server_fd{-1}, _addr{} {}

            TCPServer(const TCPServer&) = delete;
            TCPServer& operator=(const TCPServer&) = delete;

            TCPServer(TCPServer&&) noexcept = default;
            TCPServer& operator=(TCPServer&&) noexcept = default;

            ~TCPServer() = default;

            void init(const uint16_t port = 8080)
            {
                if(
                    _server_fd = SocketGuard(socket(AF_INET, SOCK_STREAM, 0));
                    _server_fd.get() == -1
                ) rpc::utils::errors::throw_errno("Init, socket");
    
                _addr.sin_family      = AF_INET;
                _addr.sin_port        = htons(port);
                _addr.sin_addr.s_addr = INADDR_ANY;
    
                _server_fd.set_options(
                    std::make_tuple(SOL_SOCKET ,  SO_REUSEADDR,  1),
                    std::make_tuple(IPPROTO_TCP,  TCP_NODELAY,   1),
                    std::make_tuple(SOL_SOCKET ,  SO_KEEPALIVE,  1),
                    std::make_tuple(IPPROTO_TCP,  TCP_KEEPIDLE,  120),
                    std::make_tuple(IPPROTO_TCP,  TCP_KEEPINTVL, 20),
                    std::make_tuple(IPPROTO_TCP,  TCP_KEEPCNT,   3)
                );
    
                if(bind(_server_fd.get(), (sockaddr*)&_addr, sizeof(_addr))<0)
                    rpc::utils::errors::throw_errno("Init, bind");
    
                //* Add epoll
            }
    
            void listen(const int max_half_open_cons) const
            {
                //* Nonblock socket whith epoll
    
                if(::listen(_server_fd.get(), max_half_open_cons)<0)
                    rpc::utils::errors::throw_errno("Listen");
    
                //* Add epoll
            }
    
            rpc::models::Connection accept()
            {
                sockaddr_in con_addr{};
    
                socklen_t con_addr_len = sizeof(con_addr);
    
                int con_fd;
    
                if(
                    con_fd = ::accept(_server_fd.get(), (sockaddr*)&con_addr, &con_addr_len);
                    con_fd == -1    
                ) rpc::utils::errors::throw_errno("Accept");
    
                return rpc::models::Connection{SocketGuard(con_fd), con_addr};
            }
    };
}