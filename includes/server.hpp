#pragma once

#include "./includes.hpp"
#include "./user.hpp"

class Server
{
    private:
        pollfd              _pfds[MAX_CONS + 1];
        sockaddr_in         _addr;
        std::string         _pass;
        std::vector<User>   _users;
        void                _init(int port, std::string pass);

    public:
        Server(void);
        Server(int port, std::string pass);
        Server(const Server &src);
        Server &operator=(const Server &src);
        ~Server();
        void    _run(void);
        void    _handle_connection(void);
        void    _handle_message(size_t icli);
        User    &_get_user_from_i(size_t icli);

    class   SocketInitError     : public std::exception
        {   public: virtual const char *what() const throw(); };
    class   SocketBindError     : public std::exception
        {   public: virtual const char *what() const throw(); };
    class   SocketListenError   : public std::exception
        {   public: virtual const char *what() const throw(); };
    class   UserNotFoundError   : public std::exception
        {   public: virtual const char *what() const throw(); };
};
