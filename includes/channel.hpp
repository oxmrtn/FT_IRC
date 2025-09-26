#pragma once

#include "./includes.hpp"
#include "./tools.hpp"
#include "./message.hpp"
#include "./consts.hpp"
#include "./user.hpp"


class Channel
{
    private:
        std::string             _name;
        std::string             _topic;
        bool                    _otopic;
        std::string             _pwd;
        bool                    _pwdNeeded;
        std::vector<User*>       _uList;
        std::vector<User*>       _oList;
        bool                    _iOnly;
        std::vector<User*>       _invList;
        std::deque<Message>    _messList;
        int                    _uLimit;

    public:
        Channel();
        Channel(std::string name);
        Channel(const Channel & other);
        Channel & operator=(const Channel & other);
        ~Channel();
        bool kick(User * user);
        bool invite(User * user);
        bool join(User * user, const std::string & parameters);
        bool mode(char mode, User & user, char sign, std::string parameters);
        bool setTopic(std::string topic, User & user);
        void displayLastMessage();
        void displayAllMessage();
        void addMessage(const Message & message);
};