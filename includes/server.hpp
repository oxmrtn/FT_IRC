#pragma once

#include "./includes.hpp"

class Server
{
    private:
        int                 _socket;
        sockaddr_in         _addr;
        std::vector<int>    _clients;
        void                _init(int port);

    public:
        Server(void);
        Server(int port);
        Server(const Server &src);
        Server &operator=(const Server &src);
        ~Server();

    class   SocketInitError : public std::exception
        {   public: virtual const char *what() const throw(); };
    class   SocketBindError : public std::exception
        {   public: virtual const char *what() const throw(); };
    class   SocketListenError : public std::exception
        {   public: virtual const char *what() const throw(); };
};

