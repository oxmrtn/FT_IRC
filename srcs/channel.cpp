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

bool Channel::kick(const User & user)
{
    for (std::vector<User>::iterator it = this->_uList.begin();
         it != this->_uList.end(); ++it)
    {
        if (user == *it) {
            this->_uList.erase(it);
            return (true);
        }
    }
    return (false);
}

bool Channel::invite(const User & user)
{
    if (!UserInVector(user, this->_uList))
    {
        this->_uList.push_back(user);
        return (true);
    }
    return (false);
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
                User temp = getUserByUname(parameters, _uList);
                if (sign == '+' && !UserInVector(temp, _oList))
                        _oList.push_back(temp);
                if (sign == '-' && UserInVector(temp, _oList))
                {
                   remUserInVector(temp, _oList);
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
                    return false;
                _uLimit = tmp;
            }
            break;
        }
        default :
            return (false);
    }
    return (true);
}


void Channel::displayAllMessage()
{
    for (std::deque<Message>::iterator it = this->_messList.begin();
         it != this->_messList.end(); ++it)
    {
            std::string temp = it->getMessage();
            
    }
    return ;
}  

void Channel::displayLastMessage()
{
    if (_messList.size() == 0)
        return ;
    _messList[this->_messList.size() - 1].displayMessage();
    return ;
}

void Channel::addMessage(const Message & message)
{
    if (_messList.size() == MAX_MSG_CHANNEL)
    {
        _messList.pop_front();
    }
    _messList.push_back(message);
    return ;
}
