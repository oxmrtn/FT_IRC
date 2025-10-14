#include "../includes/includes.hpp"


// ==================== GETTERS ====================


sockaddr_in &User::_get_addr(void) { return _addr; }
pollfd      *User::_get_pfd(void) const { return _pfd; }
std::string User::_get_msg(void) const { return _msg; }
bool        User::_get_auth(void) const { return _auth; }
std::string User::_get_username(void) const { return _username; }
std::string User::_get_nickname(void) const { return _nickname; }
std::string User::_get_hostname(void) const { return _hostname; }
std::string User::_get_prefix( void ) const {
    return ( ":" + _nickname + "!" + _username + "@" + _hostname);
}

// ==================== SETTERS ====================


void    User::_set_msg(std::string msg, bool merge)
{
    if (merge)
        _msg += msg;
    else
        _msg = msg;
}

void    User::_set_pfd(pollfd *pfd) { _pfd = pfd; }
void    User::_set_auth(bool auth) { _auth = auth; }
void    User::_set_username(std::string username) { _username = username; }
void    User::_set_nickname(std::string nickname) { _nickname = nickname; }


// ==================== CONSTRUCTORS ====================


User::User(void)
{
    _pfd = NULL;
    _msg = "";
    _username = "";
    _nickname = "";
    _auth = false;
}

User::User(const User &src)
{
    _pfd = src._pfd;
    _addr = src._addr;
    _msg = src._msg;
    _username = src._username;
    _nickname = src._nickname;
    _auth = src._auth;
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
    }
    return *this;
}

bool User::operator!=(const User &src)
{
    return (_username != src._username);
}

bool User::operator==(const User &src)
{
    return (_username == src._username);
}
