#include "../includes/server.hpp"


// ==================== METHODS ====================


void Server::_init(int port)
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);

    if (_socket < 0)
        throw SocketInitError();

    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(_addr.sin_zero, 0, sizeof(_addr.sin_zero));

    if (bind(_socket, reinterpret_cast<struct sockaddr*>(&_addr), sizeof(_addr)) < 0)
        throw SocketBindError();
        
    if (listen(_socket, 5) < 0)
        throw SocketListenError();
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
    {
        _socket = src._socket;
        _addr = src._addr;
        _clients = src._clients;
    }  
}

Server::~Server()
{
    close(_socket);
}


// ==================== OPERATORS ====================



Server  &Server::operator=(const Server &src)
{
    if (this != &src)
    {
        _socket = src._socket;
        _addr = src._addr;
        _clients = src._clients;
    }
    return *this;
}


// ==================== EXCEPTIONS ====================


const char *Server::SocketInitError::what() const throw()
{
    return "failed to init server socket";
}

const char *Server::SocketBindError::what() const throw()
{
    return "failed to bind port";
}

const char *Server::SocketListenError::what() const throw()
{
    return "failed to enable socket listening";
}
