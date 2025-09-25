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
        void                _user_infos_setup(User &user, std::string arg, const std::string field_name, Getter getter, Setter setter);
        void                _process_polled(size_t user_i);
        void                _handle_connection(void);
        void                _handle_message(User &user, std::string cmd, std::string arg);
        void                _handle_auth(User &user, std::string cmd, std::string arg);
        void                _handle_setup(User &user, std::string cmd, std::string arg);
        void                _config_msgs(User &user, bool is_prompt);
        bool                _is_username_available(std::string username);
        void                _disconnect_user(User &user);
        void                _help_cmd(User &user);
        void                _logout_cmd(User &user);
        void                _whoami_cmd(User &user);
        void                _update_nickname_cmd(User &user, std::string arg);
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
