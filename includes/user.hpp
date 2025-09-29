#pragma once

#include "includes.hpp"
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
        bool        _auth;

    public:
        User(void);
        User(const User &src);
        User &operator=(const User &src);
        ~User();
        // GETTERS
        sockaddr_in &_get_addr(void);
        pollfd      *_get_pfd(void) const;
        std::string _get_msg(void) const;
        bool        _get_auth(void) const;
        std::string _get_username(void) const;
        std::string _get_nickname(void) const;
        // SETTERS
        void        _set_pfd(pollfd *pfd);
        void        _set_msg(std::string msg, bool merge);
        void        _set_auth(bool auth);
        void        _set_username(std::string username);
        void        _set_nickname(std::string nickname);
};
