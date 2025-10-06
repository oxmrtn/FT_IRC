#pragma once

#include "includes.hpp"
#include "tools.hpp"
#include "message.hpp"
#include "user.hpp"


class Channel
{
    private:
        std::string             _name;
        std::string             _topic;
        bool                    _otopic;
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
        bool kick(User * user, User * op);
        bool invite(User * user);
        bool join(User * user, const std::string & parameters);
        bool mode(char mode, User & user, char sign, std::string parameters);
        bool setTopic(std::string topic, User & user);
        bool addOpp(User  * user);
        const std::string & _getName() const ;
        const std::string & _getTopic() const ;
};

class ChannelException : public std::exception {
protected:
    std::string _msg;
public:
    ChannelException(const std::string& msg) : _msg(msg) {}
    virtual const char* what() const throw() { return _msg.c_str(); }
};


class ErrNoSuchNick       : public ChannelException { public: ErrNoSuchNick(const std::string& nick) : ChannelException(nick + " :No such nick/channel") {} };
class ErrNoSuchChannel    : public ChannelException { public: ErrNoSuchChannel(const std::string& chan) : ChannelException(chan + " :No such channel") {} };
class ErrTooManyChannels  : public ChannelException { public: ErrTooManyChannels(const std::string& chan) : ChannelException(chan + " :You have joined too many channels") {} };

class ErrUnknownMode      : public ChannelException { public: ErrUnknownMode(const std::string& c) : ChannelException(c + " :is unknown mode char to me") {} };
class ErrChannelIsFull    : public ChannelException { public: ErrChannelIsFull(const std::string& chan) : ChannelException(chan + " :Cannot join channel (+l)") {} };
class ErrInviteOnlyChan   : public ChannelException { public: ErrInviteOnlyChan(const std::string& chan) : ChannelException(chan + " :Cannot join channel (+i)") {} };
class ErrBannedFromChan   : public ChannelException { public: ErrBannedFromChan(const std::string& chan) : ChannelException(chan + " :Cannot join channel (+b)") {} };
class ErrBadChannelKey    : public ChannelException { public: ErrBadChannelKey(const std::string& chan) : ChannelException(chan + " :Cannot join channel (+k)") {} };
class ErrBadChanMask      : public ChannelException { public: ErrBadChanMask(const std::string& chan) : ChannelException(chan + " :Bad Channel Mask") {} };
class ErrNoChanModes      : public ChannelException { public: ErrNoChanModes(const std::string& chan) : ChannelException(chan + " :Channel doesn't support modes") {} };

class ErrUserNotInChannel : public ChannelException { public: ErrUserNotInChannel(const std::string& nick, const std::string& chan) : ChannelException(nick + " " + chan + " :They aren't on that channel") {} };
class ErrNotOnChannel     : public ChannelException { public: ErrNotOnChannel(const std::string& chan) : ChannelException(chan + " :You're not on that channel") {} };
class ErrUserOnChannel    : public ChannelException { public: ErrUserOnChannel(const std::string& nick, const std::string& chan) : ChannelException(nick + " " + chan + " :is already on channel") {} };
class ErrChanOpPrivsNeeded: public ChannelException { public: ErrChanOpPrivsNeeded(const std::string& chan) : ChannelException(chan + " :You're not channel operator") {} };

class ErrUsersDontMatch   : public ChannelException { public: ErrUsersDontMatch() : ChannelException(":Cannot change mode for other users") {} };