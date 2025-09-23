#pragma once

#include "./includes.hpp"

class Server
{
    private:
        pollfd      _fds[MAX_CONS];
        sockaddr_in _addrs[MAX_CONS];
        size_t      _ncli;
        std::string _pass;
        void    _init(int port, std::string pass);

    public:
        Server(void);
        Server(int port, std::string pass);
        Server(const Server &src);
        Server &operator=(const Server &src);
        ~Server();
        void    _run(void);
        void    _handle_connection(void);
        void    _handle_message(size_t icli);

    class   SocketInitError     : public std::exception
        {   public: virtual const char *what() const throw(); };
    class   SocketBindError     : public std::exception
        {   public: virtual const char *what() const throw(); };
    class   SocketListenError   : public std::exception
        {   public: virtual const char *what() const throw(); };
};

