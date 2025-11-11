#pragma once

#include "auth.hpp"
#include <poll.h>
#include <netinet/in.h>
#include <string>


class   User
{
    private:
        pollfd      *_pfd;
        sockaddr_in _addr;
        std::string _msg;
        std::string _username;
        std::string _nickname;
        std::string _hostname;
        Auth    _auth;

    public:
        User(void);
        User(const User &src);
        User &operator=(const User &src);
        bool operator!=(const User &src) const;
        bool operator==(const User &src) const;
        ~User();
        // GETTERS
        sockaddr_in &_get_addr(void);
        pollfd      *_get_pfd(void) const;
        std::string _get_msg(void) const;
        Auth        _get_auth(void) const;
        std::string _get_username(void) const;
        std::string _get_nickname(void) const;
        std::string _get_hostname(void) const;
        std::string _get_prefix( void ) const;
        // SETTERS
        void        _set_pfd(pollfd *pfd);
        void        _set_msg(std::string msg, bool merge);
        void        _set_auth(Auth auth);
        void        _set_username(std::string username);
        void        _set_nickname(std::string nickname);
        void        _set_hostname(std::string hostname);
};
