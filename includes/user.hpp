#pragma once

#include "./includes.hpp"

enum    Authentication   {
    NOT_AUTH,
    IS_AUTH
};

class Channel;

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

    public:
        User();
        User(const User &src);
        User &operator=(const User &src);
        ~User();
};
