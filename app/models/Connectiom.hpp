#pragma once


#include <sys/socket.h>

#include "SocketGuard.hpp"

struct Connection
{
    SocketGuard fd;
    sockaddr_in addr;
};