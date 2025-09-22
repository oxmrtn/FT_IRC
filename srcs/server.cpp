#include "../includes/server.hpp"


// ==================== METHODS ====================


void Server::_init(int port)
{
    _fds[0].fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fds[0].fd < 0)
        throw SocketInitError();
    _fds[0].events = POLLIN;

    _fds_size = 1;

    _addrs[0].sin_family = AF_INET;
    _addrs[0].sin_port = htons(port);
    _addrs[0].sin_addr.s_addr = htonl(INADDR_ANY);
    memset(_addrs[0].sin_zero, 0, sizeof(_addrs[0].sin_zero));

    if (bind(_fds[0].fd, reinterpret_cast<struct sockaddr*>(&_addrs[0]), sizeof(_addrs[0])) < 0)
        throw SocketBindError();
        
    if (listen(_fds[0].fd, CONS_QUEUE) < 0)
        throw SocketListenError();

    sockaddr_in tmp_addr;
    socklen_t   len = sizeof(tmp_addr);
    getsockname(_fds[0].fd, reinterpret_cast<sockaddr*>(&tmp_addr), &len);
    int running_port = ntohs(tmp_addr.sin_port);
    std::cout << "IRC server running on port: " << running_port << std::endl;
}

void    Server::_run(void)
{
    while (true)
    {
        int polled = poll(_fds, _fds_size, 0);
        if (polled < 0)
            std::cerr << "server: failed to poll";
        else if (polled > 0)
        {
            if (_fds[0].revents & POLLIN) {
                _accept();
            }
            // for (size_t i = 0; i < _fds_size; i++)
            // {
 
            // }
        }
    }
}

void    Server::_accept(void)
{
    socklen_t   len = sizeof(_addrs[_fds_size]);
    int fd = accept(_fds[0].fd, reinterpret_cast<struct sockaddr*>(&_addrs[_fds_size]), &len);
    if (fd < 0)
        std::cerr << "server: failed to connect new client";
    else
    {
        char    *ip = inet_ntoa(_addrs[_fds_size].sin_addr);
        int     port = ntohs(_addrs[_fds_size].sin_port);
        
        std::cout << "New client connection: " << ip << ":" << port << std::endl;
        
        _fds[_fds_size].fd = fd;
        _fds[_fds_size].events = POLLIN;
        _fds_size++;
    }
}


// ==================== CONSTRUCTORS ====================


Server::Server(void)
{
    _init(6667);
}

Server::Server(int port)
{
    _init(port);
}

Server::Server(const Server &src)
{
    if (this != &src)
        for (size_t i = 0; i < MAX_CONS; i++)
            _fds[i] = src._fds[i];
}

Server::~Server()
{
    for (size_t i = 0; i < _fds_size; i++)
        close(_fds[i].fd);
}


// ==================== OPERATORS ====================



Server  &Server::operator=(const Server &src)
{
    if (this != &src)
    {
        for (size_t i = 0; i < MAX_CONS; i++)
        {
            if (_fds[i].fd > 0)
                close(_fds[i].fd);
            _fds[i] = src._fds[i];
        }
    }
    return *this;
}


// ==================== EXCEPTIONS ====================


const char *Server::SocketInitError::what() const throw()
{
    return "server: failed to init socket";
}

const char *Server::SocketBindError::what() const throw()
{
    return "server: failed to bind port";
}

const char *Server::SocketListenError::what() const throw()
{
    return "server: failed to enable listening";
}
