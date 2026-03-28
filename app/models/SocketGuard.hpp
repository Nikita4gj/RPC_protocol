#pragma once


#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <tuple>

#include "../utils/errors.hpp"

class SocketGuard
{
    private:
        int fd;
    public:
        explicit SocketGuard(): fd{-1} {}

        explicit SocketGuard(int socket_fd) : fd{socket_fd}{}

        SocketGuard(SocketGuard&) = delete;
        SocketGuard operator=(SocketGuard&) = delete;

        SocketGuard(SocketGuard&& other) : fd{-1}
        {
            fd = other.fd;
            other.fd = -1;
        }

        SocketGuard& operator=(SocketGuard&& other)
        {
            if(fd!=-1)
                ::close(fd);

            fd = other.fd;
            other.fd = -1;

            return *this;
        }

        void set_nonblock()
        {
            int flags = fcntl(fd, F_GETFL);
            fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        }

        using OPTION = std::tuple<int, int, int>;

        template<class... OPTIONS>
        void set_options(OPTIONS... tuples)
        {
            static_assert(
                (std::is_constructible_v<OPTION, std::decay_t<OPTIONS>> && ...),
                "The type must be constructed in std::tuple<int, int, int>"
            );

            auto set_option = [this](OPTION option) mutable
            {
                auto [level, optname, opt] = option;
                if(setsockopt(fd, level, optname, &opt, sizeof(opt))<0)
                    throw_errno("SocketGuard, set_options");
            };

            (set_option(tuples), ...);
            
        }

        const int& get(){return fd;}

        int release(){int f = fd; fd = -1; return f;}

        void close()
        {
            if(fd != -1)
            {
                ::close(fd);
                fd = -1;
            }
        }

        ~SocketGuard()
        {
            if(fd !=-1) ::close(fd);
        }
};

