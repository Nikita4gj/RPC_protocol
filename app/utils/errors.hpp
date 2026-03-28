#pragma once

#include <stdexcept>
#include <string>
#include <cstring>
#include <cerrno>
#include <netdb.h>

[[noreturn]] inline void throw_errno(const char* msg)
{
    throw std::runtime_error(
        std::string(msg) + ": " + strerror(errno)
    );
}

[[noreturn]] inline void throw_gaierror(const char* msg, int gai_code)
{
    throw std::runtime_error(
        std::string(msg) + ": " + gai_strerror(gai_code)
    );
}