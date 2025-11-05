#pragma once

#include "includes.hpp"
#include "tools.hpp"
#include "message.hpp"
#include "user.hpp"
#include "exception.hpp"


class Channel
{
    private:
        std::string             _name;
        std::string             _topic;
        bool                    _otopic;
        std::string             _topicSetter;
        time_t                  _topicTime;
        std::string             _pwd;
        bool                    _pwdNeeded;
        std::vector<User*>      _uList;
        std::vector<User*>      _oList;
        bool                    _iOnly;
        std::vector<User*>      _invList;
        int                     _uLimit;

    public:
        Channel();
        Channel(std::string name);
        Channel(const Channel & other);
        Channel & operator=(const Channel & other);
        ~Channel();
        bool kick(User * user, User * op, std::string reason);
        bool invite(User * to_invite, User * user);
        bool join(User * user, const std::string & parameters);
        bool mode(char mode, User & user, char sign, std::string parameters);
        bool setTopic(std::string topic, User & user);
        bool addOpp(User  * user, bool create);
        const std::string & _getName() const ;
        const std::string & _getTopic() const ;
        const std::string & _getTopicSetter() const ;
        int                 _getCount();
        bool _is_user_in_chan(const User &user);
        std::string  _getTopicTime() const ;
        void _send_message_to_channel(User & sender, std::string content, bool global);
};
