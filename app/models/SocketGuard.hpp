#pragma  once


#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

#include <tuple>

#include "../utils/errors.hpp"

namespace rpc::models
{
    class SocketGuard
    {
        int _socket_fd;
        
        using _OPTION = std::tuple<int, int, int>;

        void _set_option (_OPTION option) const
        {
            auto [level, optname, opt] = option;
            if(setsockopt(_socket_fd, level, optname, &opt, sizeof(opt))<0)
                rpc::utils::errors::throw_errno("SocketGuard, set_options");
        };

        public:
            SocketGuard() noexcept : _socket_fd{-1} {}

            explicit SocketGuard(int socket_fd) noexcept : _socket_fd{socket_fd}{}

            SocketGuard(SocketGuard&) = delete;
            SocketGuard operator=(SocketGuard&) = delete;

            SocketGuard(SocketGuard&& other) noexcept : _socket_fd{-1}
            {
                _socket_fd = other._socket_fd;
                other._socket_fd = -1;
            }

            SocketGuard& operator=(SocketGuard&& other) noexcept
            {
                if(_socket_fd!=-1)
                    ::close(_socket_fd);

                _socket_fd = other._socket_fd;
                other._socket_fd = -1;

                return *this;
            }

            ~SocketGuard()
            {
                close();
            }

            void set_nonblock() 
            {
                int flags = fcntl(_socket_fd, F_GETFL);

                if(flags == -1)
                    rpc::utils::errors::throw_errno("Set_nonblock");

                if(fcntl(_socket_fd, F_SETFL, flags | O_NONBLOCK) == -1)
                    rpc::utils::errors::throw_errno("Set_nonblock");
            }

            template<class... OPTIONS>
            void set_options(OPTIONS... tuples)
            {
                static_assert(
                    (std::is_constructible_v<_OPTION, std::decay_t<OPTIONS>> && ...),
                    "The type must be constructed in std::tuple<int, int, int>"
                );

                (_set_option(tuples), ...);    
            }

            const int& get() const noexcept { return _socket_fd; }

            int release() noexcept { int f = _socket_fd; _socket_fd = -1; return f; }

            void close() noexcept
            {
                if(_socket_fd != -1)
                {
                    ::close(_socket_fd); 
                    //* The error is intentionally ignored — the destructor does not throw an exception
                    _socket_fd = -1;
                }
            }
    };
}
