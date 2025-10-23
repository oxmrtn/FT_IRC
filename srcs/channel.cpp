#include "../includes/channel.hpp"


// ==================== CONSTRUCTORS ====================

Channel::Channel()
{
    this->_name = "{ default_channel_name }";
    this->_topic = "";
    this->_otopic = false;
    this->_topicSetter = "none";
    this->_topicTime = time(NULL);
    this->_iOnly = false;
    this->_pwd = "";
    this->_pwdNeeded = false;
    this->_uLimit = MAX_USER_BY_CHANNEL;
}

Channel::Channel(const Channel& other)
    : _name(other._name),
      _topic(other._topic),
      _otopic(other._otopic),
      _pwd(other._pwd),
      _pwdNeeded(other._pwdNeeded),
      _uList(other._uList),
      _oList(other._oList),
      _iOnly(other._iOnly),
      _invList(other._invList),
      _uLimit(other._uLimit)
{}

Channel::Channel(std::string name)
{
    this->_name = name;
    this->_topic = "";
    this->_topicSetter = "none";
    this->_topicTime = time(NULL);
    this->_otopic = false;
    this->_iOnly = false;
    this->_pwd = "";
    this->_pwdNeeded = false;
    this->_uLimit = MAX_USER_BY_CHANNEL;
}

Channel::~Channel()
{}

// ==================== OPERATOR ====================

Channel & Channel::operator=(const Channel & other)
{
    if (&other != this)
    {
            _name = other._name;
            _topic = other._topic;
            _otopic = other._otopic;
            _topicSetter = other._topicSetter;
            _topicTime = other._topicTime;
            _pwd = other._pwd;
            _pwdNeeded = other._pwdNeeded;
            _uList = other._uList;
            _oList = other._oList;
            _iOnly = other._iOnly;
            _invList = other._invList;
            _uLimit = other._uLimit;
    }
    return (*this);
}


// ==================== METHODS ====================
bool Channel::kick(User* user, User *op, std::string reason)
{
    bool deleted = false;

    if (!UserInVector(*op, _uList))
        throw ErrNotOnChannel(_name);

    if (!UserInVector(*op, _oList))
        throw ErrChanOpPrivsNeeded(_name);
    if (!UserInVector(*user, _uList))
        throw ErrUserNotInChannel(user->_get_username(), _name);
    for (std::vector<User*>::iterator it = _uList.begin(); it != _uList.end(); )
    {
        if (*it == user)
        {
            it = _uList.erase(it);
            deleted = true;
        }
        else
            ++it;
    }
    for (std::vector<User*>::iterator it = _oList.begin(); it != _oList.end(); )
    {
        if (*it == user)
        {
            it = _oList.erase(it);
            deleted = true;
        }
        else
            ++it;
    }
    for (std::vector<User*>::iterator it = _invList.begin(); it != _invList.end(); )
    {
        if (*it == user)
        {
            it = _invList.erase(it);
            deleted = true;
        }
        else
            ++it;
    }
    if (deleted)
    {
        std::string to_send = op->_get_prefix() + " KICK " + _name + user->_get_nickname() + ":" + reason; 
        _send_message_to_channel(*op, to_send, false);
        _send_raw_string(*user, to_send);
    }
    return deleted;
}

bool Channel::join(User* user, const std::string& key)
{
    if (UserInVector(*user, _uList))
        return false;

    if (_uLimit > 0 && _uList.size() >= static_cast<size_t>(_uLimit))
        throw ErrChannelIsFull(_name);

    if (_iOnly && !UserInVector(*user, _invList))
        throw ErrInviteOnlyChan(_name);

    if (_pwdNeeded && _pwd != key)
        throw ErrBadChannelKey(_name);

    _uList.push_back(user);

    for (std::vector<User*>::iterator it = _invList.begin(); it != _invList.end(); ++it)
    {
        if (*it == user)
        {
            _invList.erase(it);
            break;
        }
    }
    std::string message_to_channel = user->_get_prefix() + " JOIN :" + _name;
    this->_send_message_to_channel(*user, message_to_channel, false);
    std::string message_to_user_topic = ":";
    message_to_user_topic += SERVER_NAME;
    std::string timestamp = message_to_user_topic;
    std::string user_list = message_to_user_topic;
    std::string last_message = message_to_user_topic;
    message_to_user_topic += " 332 " + user->_get_username() + " " + _name + " :" + _topic;
    timestamp += " 333 " + user->_get_nickname() + " " + _name + " " + this->_getTopicSetter() + " " + this->_getTopicTime();
    _send_raw_string(*user, message_to_user_topic);
    _send_raw_string(*user, timestamp);
    user_list += " 353 " + user->_get_nickname() + " = " + _name + " :";
    std::string names;
    for (size_t i = 0; i < _oList.size(); ++i)
        names += "@" + _oList[i]->_get_nickname() + " ";
    for (size_t i = 0; i < _uList.size(); ++i)
        if (!UserInVector(*_uList[i], _oList))
            names += _uList[i]->_get_nickname() + " ";
    size_t IRC_MAX_LEN = 512;   
    while ((user_list.size() + names.size()) > IRC_MAX_LEN)
    {
        std::string temp = user_list;
        size_t pos = 0;

        while (pos < names.size())
        {
            if (temp.size() + 1 >= IRC_MAX_LEN - 2)
                break;
            if (names[pos] == ' ' && temp.size() + 1 >= IRC_MAX_LEN - 10)
                break;
            temp += names[pos++];
        }

        names.erase(0, pos);
        _send_raw_string(*user, temp);
    }
    if (names.size() != 0 )
        _send_raw_string(*user, (user_list + names));
    last_message += " 306 " + user->_get_nickname() + _name + ":End of /NAMES list.";
    _send_raw_string(*user, last_message);
    return true;
}


bool Channel::invite(User* to_invite, User *user)
{
    if (UserInVector(*to_invite, _uList))
        throw ErrUserOnChannel(to_invite->_get_username(), _name);

    if (!UserInVector(*to_invite, _invList))
    {
        _invList.push_back(to_invite);
        std::string to_send_host = ":";
        to_send_host += SERVER_NAME;
        to_send_host += " 341 " + user->_get_nickname() + " " + to_invite->_get_nickname() + " " + _name;
        std::string to_send_guest = user->_get_prefix() + " INVITE " + to_invite->_get_nickname() + " :" + _name;
         _send_raw_string(*to_invite, to_send_guest);
         _send_raw_string(*user, to_send_host);
    }
    return (true);
}

bool Channel::addOpp(User *user, bool create)
{
    if (!UserInVector(*user, _uList) && !create)
        throw ErrNotOnChannel(_name);
    if (!UserInVector(*user, _oList))
        _oList.push_back(user);
    if (create)
        _uList.push_back(user);
    return true;
}


bool Channel::setTopic(std::string topic, User & user)
{
    if (!UserInVector(user, _uList))
        throw ErrUserNotInChannel(user._get_nickname(), _name);
    if (_otopic && !UserInVector(user, _oList))
        throw ErrChanOpPrivsNeeded(_name);
    _topic = topic;
    _topicSetter = user._get_nickname();
    _topicTime = time(NULL);
    std::string to_send = user._get_prefix() + " TOPIC " + _name + ":" + topic;
    _send_message_to_channel(user, to_send, true);
    return (true); 
}
 
bool Channel::mode(char mode, User & user, char sign, std::string parameters)
{
    // TO DO RESPONSE
    if (!UserInVector(user, _oList))
        throw ErrChanOpPrivsNeeded(_name);

    switch (mode)
    {
        case 'i':
            _iOnly = (sign == '+');
            break;

        case 'o':
        {
            User* temp = NULL;
            try {
                temp = getUserByUname(parameters, _uList);
            } catch (UserNotFound& e) {
                throw ErrNoSuchNick(parameters);
            }

            if (sign == '+' && !UserInVector(*temp, _oList))
                _oList.push_back(temp);

            if (sign == '-' && UserInVector(*temp, _oList))
                remUserInVector(*temp, _oList);

            break;
        }

        case 'k':
            if (sign == '-') {
                _pwdNeeded = false;
                _pwd.clear();
            }
            else if (sign == '+' && !parameters.empty()) {
                _pwdNeeded = true;
                _pwd = parameters;
            }
            break;

        case 't':
            _otopic = (sign == '+');
            break;

        case 'l':
        {
            if (sign == '-') {
                _uLimit = MAX_USER_BY_CHANNEL;
            } else if (sign == '+') {
                int tmp;
                std::stringstream ss(parameters);
                if (!(ss >> tmp))
                    throw ErrUnknownMode("l");
                _uLimit = tmp;
            }
            break;
        }

        default:
            throw ErrUnknownMode(std::string(1, mode));
    }
    return true;
}

void    Channel::_send_message_to_channel(User & sender, std::string to_send, bool global)
{
    for (size_t i = 0; i < _uList.size(); i++)
    {
        if (global)
        {
            std::cout << to_send << std::endl;
            _send_raw_string(*_uList[i], to_send);
        }
        else if (*(_uList[i]) != sender)
        {
            std::cout << to_send << std::endl;
            _send_raw_string(*_uList[i], to_send);
        }
    }
    return ;
}



// ==================== GETTER ====================

const std::string & Channel::_getName() const { return _name; };
const std::string & Channel::_getTopic() const { return _topic; };
const std::string & Channel::_getTopicSetter() const { return _topicSetter; };
std::string Channel::_getTopicTime() const 
{
    std::ostringstream oss;
    oss << _topicTime;
    return oss.str();
};
