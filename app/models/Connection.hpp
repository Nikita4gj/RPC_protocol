#pragma  once


#include <sys/socket.h>

#include "SocketGuard.hpp"

namespace rpc::models
{
    struct Connection
    {
        SocketGuard fd;
        sockaddr_in addr;
        
        Connection(SocketGuard fd, sockaddr_in addr) noexcept
        : fd{std::move(fd)}, addr{addr} {}
    };
}