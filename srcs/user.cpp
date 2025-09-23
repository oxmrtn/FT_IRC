#include "../includes/user.hpp"

// ==================== CONSTRUCTORS ====================


User::User(void)
{
    _pfd = NULL;
    memset(_msg_buf, 0, MSG_BUF_SIZ);
    _username = "";
    _nickname = "";
    _auth = NOT_AUTH;
}

User::User(const User &src)
{
    if (this != &src)
    {
        _pfd = src._pfd;
        _addr = src._addr;
        std::strcpy(_msg_buf, src._msg_buf);
        _username = src._username;
        _nickname = src._nickname;
        _auth = src._auth;
    }
}

User::~User()
{}


// ==================== OPERATORS ====================



User  &User::operator=(const User &src)
{
    if (this != &src)
    {
        _pfd = src._pfd;
        _addr = src._addr;
        std::strcpy(_msg_buf, src._msg_buf);
        _username = src._username;
        _nickname = src._nickname;
        _auth = src._auth;
    }
    return *this;
}

bool User::operator==(const User &other) const
{
    return (this->_username == other._username);
}


// ==================== GETTER ====================

const std::string& User::getUsername() const 
{
    return (_username);
}

const std::string& User::getNickname() const
{
    return (_nickname);
}
