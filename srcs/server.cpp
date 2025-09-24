#include "../includes/server.hpp"


// ==================== METHODS ====================


void Server::_init(int port, std::string pass)
{
    _pfds[0].fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_pfds[0].fd < 0)
        throw SocketInitError();
    _pfds[0].events = POLLIN;

    _pass = pass;

    sockaddr_in tmp_addr;
    tmp_addr.sin_family = AF_INET;
    tmp_addr.sin_port = htons(port);
    tmp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(tmp_addr.sin_zero, 0, sizeof(tmp_addr.sin_zero));

    if (bind(_pfds[0].fd, reinterpret_cast<struct sockaddr*>(&tmp_addr), sizeof(tmp_addr)) < 0)
        throw SocketBindError();
        
    if (listen(_pfds[0].fd, CONS_QUEUE) < 0)
        throw SocketListenError();

    socklen_t   len = sizeof(_addr);
    getsockname(_pfds[0].fd, reinterpret_cast<sockaddr*>(&_addr), &len);
    int running_port = ntohs(_addr.sin_port);

    std::cout << "IRC server running on port: " << running_port << std::endl;
}

void    Server::_run(void)
{
    while (true)
    {
        int polled = poll(_pfds, _users.size() + 1, 0);
        if (polled < 0)
            err_ret(strerror(errno));
        else if (polled > 0)
        {
            if (_pfds[0].revents & POLLIN)
                _handle_connection();
            for (size_t i = 1; i < _users.size() + 1; i++)
                if (_pfds[i].revents & POLLIN)
                    _handle_message(i);
        }
    }
}

void    Server::_handle_connection(void)
{
    User        user;
    sockaddr_in addr = user._get_addr();
    socklen_t   len = sizeof(addr);

    int fd = accept(_pfds[0].fd, reinterpret_cast<struct sockaddr*>(&addr), &len);
    if (fd < 0)
        err_ret(strerror(errno));
    else
    {
        char    *ip = inet_ntoa(addr.sin_addr);
        int     port = ntohs(addr.sin_port);
        int     i = _users.size() + 1;
        
        _pfds[i].fd = fd;
        _pfds[i].events = POLLIN;
        user._set_pfd(&_pfds[i]);
        _users.push_back(user);

        std::cout << "New user connection: " << ip << ":" << port << std::endl;
    }
}

void    Server::_handle_message(size_t icli)
{
    User    user = _get_user_from_i(icli);
    char    *buf = user._get_msg_buf();

    ssize_t r_bytes = recv(_pfds[icli].fd, buf, MSG_BUF_SIZ - 1, 0);
    
    if (r_bytes > 0) {
        buf[r_bytes] = '\0';
        std::cout << "msg: " << buf << std::endl;
    }
    else if (r_bytes == 0) {
        // disconnect
    }
    else {
        err_ret(strerror(errno));
    }
}

User &Server::_get_user_from_i(size_t icli)
{
    for (size_t i = 0; i < _users.size(); ++i)
        if (_users[i]._get_pfd() == &_pfds[icli])
            return _users[i];
    throw UserNotFoundError();
}


// ==================== CONSTRUCTORS ====================


Server::Server(void)
{
    _init(6667, "123");
}

Server::Server(int port, std::string pass) 
{
    _init(port, pass);
}

Server::Server(const Server &src)
{
    if (this != &src)
    {
        //
    }
}

Server::~Server()
{
    //
}


// ==================== OPERATORS ====================


Server  &Server::operator=(const Server &src)
{
    if (this != &src)
    {
        //
    }
    return *this;
}


// ==================== EXCEPTIONS ====================


const char *Server::SocketInitError::what() const throw()
{
    return "failed to init socket";
}

const char *Server::SocketBindError::what() const throw()
{
    return "failed to bind port";
}

const char *Server::SocketListenError::what() const throw()
{
    return "failed to enable listening";
}

const char *Server::UserNotFoundError::what() const throw()
{
    return "user not found for given pollfd index";
}
