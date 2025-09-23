#include "../includes/client.hpp"

// ==================== CONSTRUCTORS ====================


Client::Client(void)
{
    _pfd = NULL;
    memset(_msg_buf, 0, MSG_BUF_SIZ);
    _username = "";
    _nickname = "";
    _auth = NOT_AUTH;
}

Client::Client(const Client &src)
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

Client::~Client()
{}


// ==================== OPERATORS ====================



Client  &Client::operator=(const Client &src)
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