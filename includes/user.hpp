#pragma once

#include "./includes.hpp"

enum    Authentication   {
    NOT_AUTH,
    IS_AUTH
};

class Channel;
#include <poll.h>
#include <netinet/in.h>
#include <string>

class User
{
    private:
        pollfd                  *_pfd;
        sockaddr_in             _addr;
        char                    _msg_buf[MSG_BUF_SIZ];
        std::string             _username;
        std::string             _nickname;
        std::vector<Channel>    _channel;
        Authentication  _auth;
        pollfd      *_pfd;
        sockaddr_in _addr;
        std::string _msg;
        std::string _username;
        std::string _nickname;
        bool        _auth;
        size_t      _retry;

    public:
        User();
        User(const User &src);
        User &operator=(const User &src);
        bool operator==(const User &other) const;
        ~User();
        const std::string& getUsername() const;
        const std::string& getNickname() const;
        ~User();
        // GETTERS
        sockaddr_in &_get_addr(void);
        pollfd      *_get_pfd(void) const;
        std::string _get_msg(void) const;
        bool        _get_auth(void) const;
        size_t      _get_retry(void) const;
        std::string _get_username(void) const;
        std::string _get_nickname(void) const;
        // SETTERS
        void        _set_pfd(pollfd *pfd);
        void        _set_msg(std::string msg, bool merge);
        void        _set_auth(bool auth);
        void        _set_username(std::string username);
        void        _set_nickname(std::string nickname);
        void        _decr_retry(void);
};
