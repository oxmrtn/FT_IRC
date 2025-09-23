#pragma once

#include "./includes.hpp"

enum    Authentication  {
    NOT_AUTH,
    IS_AUTH
};

class User
{
    private:
        pollfd          *_pfd;
        sockaddr_in     _addr;
        char            _msg_buf[MSG_BUF_SIZ];
        std::string     _username;
        std::string     _nickname;
        Authentication  _auth;

    public:
        User();
        User(const User &src);
        User &operator=(const User &src);
        ~User();
        sockaddr_in &_get_addr(void);
        pollfd      *_get_pfd(void);
        char        *_get_msg_buf(void);
        void        _set_pfd(pollfd *pfd);
};
