#include "../includes/includes.hpp"


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
    sockaddr_in &addr = user._get_addr();
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
        send_to_user(user, "Please authenticate with \"PASS <password>\"\r\n");
    }
}

void    Server::_auth_process(User &user, const std::string &msg)
{
    if (!starts_with(msg, "PASS "))
        send_to_user(user, "\nPlease authenticate with \"PASS <password>\"\r\n");
    else if (msg.substr(5) == _pass)
    {
        user._set_auth(IS_AUTH);
        send_to_user(user, "\nSuccessfully authenticated\nPlease set your username with \"USER <username>\"\nPlease set your nickname with \"NICK <nickname>\"\r\n");
    }
    else
    {
        size_t      retry = user._get_retry();
        if (retry == 0)
        {
            send_to_user(user, "\nExceeded max tries, disconnected\r\n");
            // disconnect
        }
        user._decr_retry();
        std::ostringstream  oss;
        oss << "\nInvalid password, please try again (" << retry << " tries left)\r\n";
        send_to_user(user, oss.str());
    }
}

void Server::_set_user_field(User &user, std::string &content, const std::string &field, Getter getter, Setter setter)
{
    if ((user.*getter)() == "")
    {
        if (content.length() == 0)
            send_to_user(user, ("\n" + field + " cannot be empty\r\n").c_str());
        else
        {
            (user.*setter)(content);
            std::ostringstream oss;
            oss << "\nSuccessfully set \"" << content << "\" as " << to_low(field) << "\r\n";
            send_to_user(user, oss.str());
        }
    }
    else
        send_to_user(user, ("\n" + field + " is already set\r\n").c_str());
}

void    Server::_config_process(User &user, const std::string &msg)
{
    if (msg.length() < 6)
    {
        // send_to_user(user, "\nPlease set your username with \"USER <username>\"\nPlease set your nickname with \"NICK <nickname>\"\r\n");
        return;
    }
    std::string content = msg.substr(5);

    if (starts_with(msg, "USER "))
        _set_user_field(user, content, "Username", &User::_get_username, &User::_set_username);
    else if (starts_with(msg, "NICK "))
        _set_user_field(user, content, "Nickname", &User::_get_nickname, &User::_set_nickname);
    else
    {
        if (user._get_username() == "")
            send_to_user(user, "\nPlease set your username with \"USER <username>\"\r\n");
        if (user._get_nickname() == "")
            send_to_user(user, "\nPlease set your username with \"NICK <nickname>\"\r\n");
    }
}

void    Server::_handle_message(size_t icli)
{
    User    &user = _get_user_from_i(icli);
    char    buf[MSG_BUF_SIZ];

    ssize_t r_bytes = recv(_pfds[icli].fd, buf, MSG_BUF_SIZ - 1, 0);
    
    if (r_bytes > 0) {
        buf[r_bytes] = '\0';
        user._set_msg(buf, true);
        std::string msg = user._get_msg();
        if (!ends_with(msg, "\n"))
            return;
        msg.erase(msg.length() - 1);

        Authentication  auth = user._get_auth();
        if (auth == NOT_AUTH)
            _auth_process(user, msg);
        else if (user._get_username() == "" || user._get_nickname() == "")
            _config_process(user, msg);
        else
        {
            // process message
            std::cout << "msg: " << msg << std::endl;
        }
        user._set_msg("", false);
    }
    else if (r_bytes == 0) {
        // disconnect
    }
    else {
        err_ret(strerror(errno));
    }
}


// ==================== GETTERS ====================


User    &Server::_get_user_from_i(size_t icli)
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
