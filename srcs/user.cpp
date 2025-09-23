#include "../includes/user.hpp"


// ==================== METHODS ====================


sockaddr_in &User::_get_addr(void)
{
    return _addr;
}

void    User::_set_pfd(pollfd *pfd)
{
    _pfd = pfd;
}

pollfd  *User::_get_pfd(void)
{
    return _pfd;
}

char    *User::_get_msg_buf(void)
{
    return _msg_buf;
}


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