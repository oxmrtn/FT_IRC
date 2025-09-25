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
                    _process_polled(i);
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
        
        if (_users.size() == MAX_CONS)
        {
            std::cout << "User connection refused: " << ip << ":" << port << std::endl;
            std::string msg = "Connection failed, server have reached maximum connexions";
            send(fd, msg.c_str(), msg.length(), 0);
        
            close(fd);
        }
        else
        {
            _pfds[i].fd = fd;
            _pfds[i].events = POLLIN;
            user._set_pfd(&_pfds[i]);
            _users.push_back(user);
    
            std::cout << "User connection accepted: " << ip << ":" << port << std::endl;
            send_to_user(user, PASS_MSG, false);
        }
    }
}

void    Server::_handle_auth(User &user, const std::string &msg)
{
    if (msg.length() < 6)
    {
        send_to_user(user, PASS_MSG, true);
        return;
    }
    std::string prefix = msg.substr(0, 5);
    std::string content = msg.substr(5);

    if (prefix != "PASS ")
        send_to_user(user, PASS_MSG, true);
    else if (content == _pass)
    {
        user._set_auth(true);
        send_to_user(user, "[ ===== Successfully connected ===== ]\n" + USER_MSG + "\n" + NICK_MSG, true);
    }
    else
    {
        size_t  retry = user._get_retry();
        if (retry == 0)
        {
            send_to_user(user, "Exceeded max tries, disconnected", true);
            _disconnect_user(user);
        }
        user._decr_retry();
        std::ostringstream  oss;
        oss << "Invalid password, please try again (" << retry << " tries left)";
        send_to_user(user, oss.str(), true);
    }
}

void    Server::_config_msgs(User &user, bool set_nl)
{
    std::string username = user._get_username();
    std::string nickname = user._get_nickname();

    if (username == "" && nickname == "")
        send_to_user(user, USER_MSG + "\n" + NICK_MSG, set_nl);
    else if (username == "")
        send_to_user(user, USER_MSG, set_nl);
    else if (nickname == "")
        send_to_user(user, NICK_MSG, set_nl);
    else
        send_to_user(user, "[ ===== Successfully authenticated ===== ]\nUse \"HELP\" to see available commands", set_nl);
}

void Server::_set_user_field(User &user, std::string &content, const std::string &field, Getter getter, Setter setter)
{
    if ((user.*getter)() == "")
    {
        if (content == "username" && !_is_username_available(content))
        {
            std::ostringstream oss;
            oss << "Username \"" << content << "\" is not available";
            send_to_user(user, oss.str(), true);
            _config_msgs(user, false);
        }
        else
        {
            (user.*setter)(content);
            std::ostringstream oss;
            oss << "Successfully set \"" << content << "\" as " << to_low(field);
            send_to_user(user, oss.str(), true);
            _config_msgs(user, false);
        }
    }
    else
    {
        send_to_user(user, field + " is already set", true);
        _config_msgs(user, false);
    }
}

void    Server::_handle_setup(User &user, const std::string &msg)
{
    if (msg.length() < 6)
    {
        _config_msgs(user, true);
        return;
    }
    std::string prefix = msg.substr(0, 5);
    std::string content = msg.substr(5);

    if (prefix == "USER ")
        _set_user_field(user, content, "Username", &User::_get_username, &User::_set_username);
    else if (prefix == "NICK ")
        _set_user_field(user, content, "Nickname", &User::_get_nickname, &User::_set_nickname);
    else
        _config_msgs(user, true);
}

void    Server::_handle_message(User &user, const std::string &msg)
{

}

void    Server::_process_polled(size_t user_i)
{
    User    &user = _get_user_from_i(user_i);
    char    buf[MSG_BUF_SIZ];

    ssize_t r_bytes = recv(_pfds[user_i].fd, buf, MSG_BUF_SIZ - 1, 0);
    
    if (r_bytes > 0) {
        buf[r_bytes] = '\0';
        user._set_msg(buf, true);
        std::string msg = user._get_msg();
        if (!msg.empty() && msg[msg.length() - 1] != '\n')
            return;
        msg.erase(msg.length() - 1);

        bool    auth = user._get_auth();
        if (!auth)
            _handle_auth(user, msg);
        else if (user._get_username() == "" || user._get_nickname() == "")
            _handle_setup(user, msg);
        else
            _handle_message(user, msg);
        user._set_msg("", false);
    }
    else if (r_bytes == 0)
        _disconnect_user(user);
    else {
        err_ret(strerror(errno));
    }
}

void Server::_disconnect_user(User &user)
{
    char    *ip = inet_ntoa(user._get_addr().sin_addr);
    int     port = ntohs(user._get_addr().sin_port);
    size_t  i = _get_i_from_user(user);

    close(_pfds[i].fd);
    size_t last = _users.size();
    if (i != last) {
        _pfds[i] = _pfds[last];
        _users[last - 1]._set_pfd(&_pfds[i]);
        _users[i - 1] = _users[last - 1];
    }
    _users.pop_back();
    _pfds[last].fd = -1;
    _pfds[last].events = 0;
    _pfds[last].revents = 0;

    std::cout << "User disconnected: " << ip << ":" << port << std::endl;
}

bool    Server::_is_username_available(std::string username)
{
    for (size_t i = 0; i < _users.size(); ++i)
        if (_users[i]._get_username() == username)
            return false;
    return true;
}


// ==================== GETTERS ====================


User    &Server::_get_user_from_i(size_t user_i)
{
    for (size_t i = 0; i < _users.size(); i++)
        if (_users[i]._get_pfd() == &_pfds[user_i])
            return _users[i];
    throw UserNotFoundError();
}

size_t  Server::_get_i_from_user(User &user)
{
    for (size_t i = 1; i < _users.size() + 1; i++)
        if (user._get_pfd() == &_pfds[i])
            return i;
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
        for (size_t i = 0; i < MAX_CONS + 1; i++)
            _pfds[i] = src._pfds[i];
        _addr = src._addr;
        _pass = src._pass;
        _users = src._users;
    }
}

Server::~Server()
{
    for (size_t i = 0; i < _users.size() + 1; i++)
        close(_pfds[i].fd);    
}


// ==================== OPERATORS ====================


Server  &Server::operator=(const Server &src)
{
    if (this != &src)
    {
        for (size_t i = 0; i < MAX_CONS + 1; i++)
            _pfds[i] = src._pfds[i];
        _addr = src._addr;
        _pass = src._pass;
        _users = src._users;
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
    return "user not found";
}
