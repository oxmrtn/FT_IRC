#pragma once

#include "./includes.hpp"

enum    Authentication   {
    NOT_AUTH,
    IS_AUTH
};

class Client
{
    private:
        pollfd          *_pfd;
        sockaddr_in     _addr;
        char            _msg_buf[MSG_BUF_SIZ];
        std::string     _username;
        std::string     _nickname;
        Authentication  _auth;

    public:
        Client();
        Client(const Client &src);
        Client &operator=(const Client &src);
        ~Client();
};
