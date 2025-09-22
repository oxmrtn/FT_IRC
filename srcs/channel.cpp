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
      _invList(other._invList)
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
    }
    return (*this);
}


bool Channel::kick(User & user)
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

bool Channel::invite(User & user)
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

bool Channel::mode(std::string mode, User & user)
{
    //things to do
    // handle i o k t l mode ! 
    return (true);
}