#pragma once

#include "./includes.hpp"
#include "./channel.hpp"


class User
{
    private:
        std::string                 _nickName;
        std::string                 _userName;
        int                         socket_fd;
        std::vector<std::string>    _channel; 

    public:
        User();
        User(std::string nick, std::string user, int socket);
        User(const User & other);
        User & operator=(const User & other);
        bool operator==(const User & other);
        ~User();
        int joinChannel(Channel chanel);
};
