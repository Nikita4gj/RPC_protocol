#include <unistd.h>
#include <fcntl.h>

#pragma once


class SocketGuard
{
    private:
        int fd;
    public:
        explicit SocketGuard(): fd{-1} {}

        explicit SocketGuard(int socket_fd) : fd{socket_fd}
        {}

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

        void set_sock_nonblock()
        {
            int flags = fcntl(fd, F_GETFL);
            fcntl(fd, F_SETFL, flags | O_NONBLOCK);
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

