#pragma once

#include "./includes.hpp"
#include "./tools.hpp"

class User;

class Channel
{
    private:
        std::string         _name;
        std::string         _topic;
        bool                _otopic;
        std::string         _pwd;
        bool                _pwdNeeded;
        std::vector<User>   _uList;
        std::vector<User>   _oList;
        bool                _iOnly;
        std::vector<User>   _invList;

    public:
        Channel();
        Channel(std::string name);
        Channel(const Channel & other);
        Channel & operator=(const Channel & other);
        ~Channel();
        bool kick(User & user);
        bool invite(User & user);
        bool mode(std::string mode, User & user);
        bool setTopic(std::string topic, User & user);

};