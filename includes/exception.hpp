#pragma once

#include <string>
#include <exception>

class ChannelException : public std::exception {
protected:
    std::string _target;
    std::string _trailing;
public:
    ChannelException(const std::string& target, const std::string& trailing)
        : _target(target), _trailing(trailing) {}

    explicit ChannelException(const std::string& trailing)
        : _target(""), _trailing(trailing) {}

    virtual ~ChannelException() throw() {}

    virtual const char* what() const throw() { return _trailing.c_str(); }

    const std::string& getTarget() const { return _target; }
    const std::string& getTrailing() const { return _trailing; }
};

class ErrNoSuchNick       : public ChannelException { public: ErrNoSuchNick(const std::string& nick) : ChannelException(nick, "No such nick/channel") {} };
class ErrNoSuchChannel    : public ChannelException { public: ErrNoSuchChannel(const std::string& chan) : ChannelException(chan, "No such channel") {} };
class ErrTooManyChannels  : public ChannelException { public: ErrTooManyChannels(const std::string& chan) : ChannelException(chan, "You have joined too many channels") {} };

class ErrUnknownMode      : public ChannelException { public: ErrUnknownMode(const std::string& mode) : ChannelException(mode, "is unknown mode char to me") {} };
class ErrChannelIsFull    : public ChannelException { public: ErrChannelIsFull(const std::string& chan) : ChannelException(chan, "Cannot join channel (+l)") {} };
class ErrInviteOnlyChan   : public ChannelException { public: ErrInviteOnlyChan(const std::string& chan) : ChannelException(chan, "Cannot join channel (+i)") {} };
class ErrBannedFromChan   : public ChannelException { public: ErrBannedFromChan(const std::string& chan) : ChannelException(chan, "Cannot join channel (+b)") {} };
class ErrBadChannelKey    : public ChannelException { public: ErrBadChannelKey(const std::string& chan) : ChannelException(chan, "Cannot join channel (+k)") {} };
class ErrBadChanMask      : public ChannelException { public: ErrBadChanMask(const std::string& chan) : ChannelException(chan, "Bad Channel Mask") {} };
class ErrNoChanModes      : public ChannelException { public: ErrNoChanModes(const std::string& chan) : ChannelException(chan, "Channel doesn't support modes") {} };

class ErrUserNotInChannel : public ChannelException { public: ErrUserNotInChannel(const std::string& nick, const std::string& chan) : ChannelException(nick + " " + chan, "They aren't on that channel") {} };
class ErrNotOnChannel     : public ChannelException { public: ErrNotOnChannel(const std::string& chan) : ChannelException(chan, "You're not on that channel") {} };
class ErrUserOnChannel    : public ChannelException { public: ErrUserOnChannel(const std::string& nick, const std::string& chan) : ChannelException(nick + " " + chan, "is already on channel") {} };
class ErrChanOpPrivsNeeded: public ChannelException { public: ErrChanOpPrivsNeeded(const std::string& chan) : ChannelException(chan, "You're not channel operator") {} };

class ErrNeedMoreParams   : public ChannelException { public: ErrNeedMoreParams(const std::string& cmd) : ChannelException(cmd, "Not enough parameters") {} };
class ErrInvalidModeParams: public ChannelException { public: ErrInvalidModeParams(const std::string& chan, const std::string& mode, const std::string& param) : ChannelException(chan + " " + mode + " " + param, "Invalid mode parameter") {} };


class ErrUserNotFound     : public ChannelException { public: ErrUserNotFound(const std::string& user) : ChannelException(user, "User not found") {} };
class ErrUsersDontMatch   : public ChannelException { public: ErrUsersDontMatch() : ChannelException("Cannot change mode for other users") {} };

