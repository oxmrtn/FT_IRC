#pragma once

#include <poll.h>
#include <netinet/in.h>
#include <string>
#include <vector>
#include <exception>
#include "const.hpp"
#include "auth.hpp"
#include "user.hpp"
#include "message.hpp"
#include "channel.hpp"
#include "exception.hpp"
#include "tools.hpp"

class User;
class Message;
class Channel;
class ChannelException;

class   Server
{
    private:
        Server(void);
        pollfd                  _pfds[CON_USER_LIMIT + 2];
        sockaddr_in             _addr;
        std::string             _pass;
        std::vector<User>       _users;
        std::vector<Channel>    _channels;
        // METHODS
        void                    _init(int port, std::string pass);
        void                    _process_polled(size_t user_i);
        bool                    _handle_sigquit(void);
        void                    _handle_connection(void);
        void                    _handle_message(User &user, Message &parsed);
        void                    _handle_pass(User &user, Auth auth, std::string command, std::vector<std::string> params);
        void                    _handle_names(User &user, Auth auth, std::string command, std::vector<std::string> params, std::string trailing);
        void                    _handle_ping(User &user, std::string trailing);
        bool                    _is_supported_command(std::string command, std::vector<std::string> params);
        void                    _send_response(User &user, std::string command, std::string error_code, std::string trailing);
        void                    _send_response(User & user,  std::string error_code, std::vector<std::string> response_params, std::string trailing);
        void                    _send_error(User &user, const std::string &command, const ChannelException &e);
        bool                    _is_nickname_available(std::string nickname);
        bool                    _is_name_valid(User &user, std::string name, bool is_nick);
        bool                    _is_char_accepted(char c, bool is_nick);
        void                    _disconnect_user(User &user);
        // GETTERS
        User                    &_get_user_from_i(size_t user_i);
        User                    _get_user_from_username(std::string username) const;
        size_t                  _get_i_from_user(User &user) const;

    public:
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
};
