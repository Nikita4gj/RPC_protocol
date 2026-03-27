#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include<stdexcept>
#include<string_view>
#include<string>


#include "SocketGuard.hpp"

[[noreturn]] void throw_errno(const char* msg)
{
    throw std::runtime_error(
        std::string(msg) + ": " + strerror(errno)
    );
} 

void init_SO_KEEPALIVE(const int& fd)
{
    int opt = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt))<-1)
        throw_errno("Init, init_SO_KEEPALIVE, setsockopt: ");
    
    int idle = 30;
    int intvl = 5;
    int probes = 3;
    
}

class TCPClient
{
    SocketGuard server_fd;
    sockaddr_in addr;

    bool stop;

    public:
        TCPClient() : server_fd{-1}, stop{false}
        {
            memset(&addr, 0, sizeof(addr));
        }

        void init(const uint16_t port = 8080)
        {
            if(
                server_fd = SocketGuard(socket(AF_INET, SOCK_STREAM, 0));
                server_fd.get() == -1
            )
            throw_errno("Init, server_fd: ");
        
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = INADDR_ANY;

            int opt = 1;
            if(setsockopt(server_fd.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
                throw_errno("Init, setcokopt: ");

            

            if(bind(server_fd.get(), (sockaddr*)&addr, sizeof(addr))<0)
                throw_errno("Init, bind: ");

            
        }
};

