#include <iostream>
#include <thread>
#include <mutex>

#include "../app/transport/TCPServer.hpp"
#include "../app/transport/TCPClient.hpp"

using rpc::net::TCPClient, rpc::net::TCPServer;
using namespace std::chrono_literals;

int main()
{
    int counter_succes = 0;

    //* Test server and client whitout any exeption
    try
    {
        TCPServer server;
        server.init(8080);
        server.listen(5);

        std::mutex mx;

        std::exception_ptr eptr = nullptr;

        std::thread client_thread(
            [&eptr, &mx, &server]()
            {
                try
                {
                    TCPClient client;
                    client.connect();

                    std::this_thread::sleep_for(1s);

                    std::lock_guard lock(mx);
                    server.accept();
                    std::cout << "Everything okay, whith client one\n";
                }
                catch(const std::exception& e)
                {
                    eptr = std::current_exception();
                }
            }
        );
        
        TCPClient client;
        client.connect();

        std::this_thread::sleep_for(1s);

        {
            std::lock_guard lock(mx);
            server.accept();
            std::cout << "Everything okay, whith client two\n";
        }

        client_thread.join();

        if(eptr!=nullptr)
            std::rethrow_exception(eptr);

        counter_succes++;
        std::cout << "Everything fine in first test\n" << std::endl;
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << "\n";
        std::cerr << "Something went wrong in first test\n" << std::endl;
    }

    //* Test server with exception
    try
    {
        TCPServer server_with_excep;
        server_with_excep.init(10);   

        std::cerr << "Something went wrong in second test\n" << std::endl;
    }
    catch(const std::exception& e)
    {
        counter_succes++;
        std::cout << e.what() << "\n";
        std::cout << "Everything fine in second test\n" << std::endl;
    }

    //* Test client with exception when the server listens on another port
    try
    {
        TCPServer server;
        server.init(8000);
        server.listen(5);
        
        TCPClient client_with_excep;
        client_with_excep.connect();

        std::cerr << "Something went wrong in third test\n" << std::endl;
    }
    catch(const std::exception& e)
    {
        counter_succes++;
        std::cout << e.what() << "\n";
        std::cout << "Everything fine in tird test\n" << std::endl;
    }

    //* Test client with exception when the server isn't listening
    try
    {
        TCPServer server;
        server.init(8080);
        
        TCPClient client_with_excep;
        client_with_excep.connect();

        std::cerr << "Something went wrong in fourth test\n" << std::endl;
    }
    catch(const std::exception& e)
    {
        counter_succes++;
        std::cout << e.what() << "\n";
        std::cout << "Everything fine in fourth test\n" << std::endl;
    }

    if(counter_succes == 4)
        std::cout << "All tests passed!\n" << std::endl;
    else
        std::cerr << counter_succes << " out of 4 tests passed(((\n" << std::endl;
}