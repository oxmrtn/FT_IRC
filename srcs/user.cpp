#include "../includes/includes.hpp"


// ==================== GETTERS ====================


sockaddr_in &User::_get_addr(void)
{
    return _addr;
}

pollfd  *User::_get_pfd(void) const
{
    return _pfd;
}

std::string User::_get_msg(void) const
{
    return _msg;
}

bool    User::_get_auth(void) const
{
    return _auth;
}

size_t  User::_get_retry(void) const
{
    return _retry;
}

std::string User::_get_username(void) const
{
    return _username;
}

std::string User::_get_nickname(void) const
{
    return _nickname;
}


// ==================== SETTERS ====================


void    User::_set_pfd(pollfd *pfd)
{
    _pfd = pfd;
}

void    User::_set_msg(std::string msg, bool merge)
{
    if (merge)
        _msg += msg;
    else
        _msg = msg;
}

void    User::_set_auth(bool auth)
{
    _auth = auth;
}

void    User::_decr_retry(void)
{
    _retry--;
}

void    User::_set_username(std::string username)
{
    _username = username;
}

void    User::_set_nickname(std::string nickname)
{
    _nickname = nickname;
}


// ==================== CONSTRUCTORS ====================


User::User(void)
{
    _pfd = NULL;
    _msg = "";
    _username = "";
    _nickname = "";
    _auth = false;
    _retry = CON_RETRIES;
}

User::User(const User &src)
{
    if (this != &src)
    {
        _pfd = src._pfd;
        _addr = src._addr;
        _msg = src._msg;
        _username = src._username;
        _nickname = src._nickname;
        _auth = src._auth;
        _retry = src._retry;
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
        _msg = src._msg;
        _username = src._username;
        _nickname = src._nickname;
        _auth = src._auth;
        _retry = src._retry;
    }
    return *this;
}