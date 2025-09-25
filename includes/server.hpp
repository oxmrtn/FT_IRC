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
        // ALIASES
        typedef std::string (User::*Getter)() const;
        typedef void (User::*Setter)(std::string);
        // METHODS
        void                _init(int port, std::string pass);
        void                _set_user_field(User &user, std::string &content, const std::string &field_name, Getter getter, Setter setter);
        void                _process_polled(size_t user_i);
        void                _handle_connection(void);
        void                _handle_message(User &user, const std::string &msg);
        void                _handle_auth(User &user, const std::string &msg);
        void                _handle_setup(User &user, const std::string &msg);
        void                _config_msgs(User &user, bool set_nl);
        bool                _is_username_available(std::string username);
        void                _disconnect_user(User &user);
        // GETTERS
        User                &_get_user_from_i(size_t user_i);
        size_t              _get_i_from_user(User &user);

    public:
        Server(void);
        Server(int port, std::string pass);
        Server(const Server &src);
        Server &operator=(const Server &src);
        ~Server();
        // METHODS
        void    _run(void);

    class   SocketInitError     : public std::exception
        {   public: virtual const char *what() const throw(); };
    class   SocketBindError     : public std::exception
        {   public: virtual const char *what() const throw(); };
    class   SocketListenError   : public std::exception
        {   public: virtual const char *what() const throw(); };
    class   UserNotFoundError   : public std::exception
        {   public: virtual const char *what() const throw(); };
};
