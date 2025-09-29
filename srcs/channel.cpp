#include "../includes/channel.hpp"


// ==================== CONSTRUCTORS ====================

Channel::Channel()
{
    this->_name = "{ default_channel_name }";
    this->_topic = "{ default_channel_topic }";
    this->_otopic = false;
    this->_iOnly = false;
    this->_pwd = "{ default_channel_pwd }";
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
    this->_iOnly = false;
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
bool Channel::kick(User* user)
{
    bool deleted = false;

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

    return deleted;
}

bool Channel::join(User* user, const std::string& parameters)
{
    if (UserInVector(*user, _uList))
        return true;

    if (_uList.size() >= static_cast<size_t>(_uLimit))
        return false;

    if (_iOnly && !UserInVector(*user, _invList))
        return false;

    if (_pwdNeeded && _pwd != parameters)
        return false;

    _uList.push_back(user);

    for (std::vector<User*>::iterator it = _invList.begin(); it != _invList.end(); ++it)
    {
        if (*it == user)
        {
            _invList.erase(it);
            break;
        }
    }

    return true;
}


bool Channel::invite(User* user)
{
    if (!UserInVector(*user, _invList))
    {
        _invList.push_back(user);
        return true;
    }
    return false;
}


bool Channel::setTopic(std::string topic, User & user)
{
    if (!this->_otopic || UserInVector(user, this->_oList) )
    {
        this->_topic = topic;
        return (true);
    }
    return (false);
}

bool Channel::mode(char mode, User & user, char sign, std::string parameters)
{
    if (!UserInVector(user, this->_oList))
        return (false);
    switch (mode)
    {
        case 'i':
        {
            _iOnly = (sign == '+');
            break;
        }
        case 'o':
        {
            try
            {
                User *temp = getUserByUname(parameters, _uList);
                if (sign == '+' && !UserInVector(*temp, _oList))
                        _oList.push_back(temp);
                if (sign == '-' && UserInVector(*temp, _oList))
                {
                   remUserInVector(*temp, _oList);
                }
            }catch(UserNotFound &e)
            {
                e.what();
                return (false);
            }
            break;
        }
        case 'k':
        {
            if (sign == '-')
            {
                _pwdNeeded = false;
                _pwd = "";
            }
            else if (sign == '+' && !parameters.empty())
            {
                _pwdNeeded = true;
                _pwd = parameters;
            }
            break;
        }
        case 't':
        {
            _otopic = (sign == '+');
            break;
        }
        case 'l':
        {
            if (sign == '-')
                _uLimit = MAX_USER_BY_CHANNEL;
            if (sign == '+')
            {
                int tmp;
                std::stringstream ss(parameters);
                if (!(ss >> tmp))
                {
                    _uLimit = MAX_USER_BY_CHANNEL;
                    return false;
                }
                _uLimit = tmp;
            }
            break;
        }
        default :
            return (false);
    }
    return (true);
}


