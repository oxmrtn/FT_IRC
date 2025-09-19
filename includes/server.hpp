#pragma once

#include "./includes.hpp"

class Server
{
    private:
        std::vector<int> client_fd;
    public:
        Server();
        Server(const Server & Server);
        Server & operator=(const Server & other);
        ~Server();
};

