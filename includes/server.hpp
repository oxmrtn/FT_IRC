#pragma once

#include "./includes.hpp"

class Server
{
    private:
        pollfd      _fds[MAX_CONS];
        sockaddr_in _addrs[MAX_CONS];
        size_t      _fds_size;
        void    _init(int port);

    public:
        Server(void);
        Server(int port);
        Server(const Server &src);
        Server &operator=(const Server &src);
        ~Server();
        void    _run(void);
        void    _accept(void);

    class   SocketInitError     : public std::exception
        {   public: virtual const char *what() const throw(); };
    class   SocketBindError     : public std::exception
        {   public: virtual const char *what() const throw(); };
    class   SocketListenError   : public std::exception
        {   public: virtual const char *what() const throw(); };
};

