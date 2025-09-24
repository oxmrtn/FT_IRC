#pragma once

#include <poll.h>
#include <netinet/in.h>
#include <string>
#include <vector>
#include <exception>
#include "consts.hpp"
#include "user.hpp"

class Server
{
    private:
        pollfd              _pfds[MAX_CONS + 1];
        sockaddr_in         _addr;
        std::string         _pass;
        std::vector<User>   _users;

        typedef std::string (User::*Getter)() const;
        typedef void (User::*Setter)(std::string);
        void                _init(int port, std::string pass);
        void                _set_user_field(User &user, std::string &content, const std::string &field_name, Getter getter, Setter setter);

    public:
        Server(void);
        Server(int port, std::string pass);
        Server(const Server &src);
        Server &operator=(const Server &src);
        ~Server();
    
        void    _run(void);
        void    _handle_connection(void);
        void    _handle_message(size_t icli);
        void    _auth_process(User &user, const std::string &msg);
        void    _config_process(User &user, const std::string &msg);

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
