#pragma once

#include <poll.h>
#include <netinet/in.h>
#include <string>

enum    Authentication  {
    NOT_AUTH,
    IS_AUTH
};

class User
{
    private:
        pollfd          *_pfd;
        sockaddr_in     _addr;
        std::string     _msg;
        std::string     _username;
        std::string     _nickname;
        Authentication  _auth;
        size_t          _retry;

    public:
        User();
        User(const User &src);
        User &operator=(const User &src);
        ~User();
    
        sockaddr_in     &_get_addr(void);
        pollfd          *_get_pfd(void) const;
        std::string     _get_msg(void) const;
        Authentication  _get_auth(void) const;
        size_t          _get_retry(void) const;
        std::string     _get_username(void) const;
        std::string     _get_nickname(void) const;
    
        void            _set_pfd(pollfd *pfd);
        void            _set_msg(std::string msg, bool merge);
        void            _set_auth(Authentication auth);
        void            _set_username(std::string username);
        void            _set_nickname(std::string nickname);
        void            _decr_retry(void);
};
