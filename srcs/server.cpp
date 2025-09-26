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
        
    if (listen(_pfds[0].fd, CON_QUEUE) < 0)
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
        
        if (_users.size() == CON_USER_LIMIT)
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
            send_to_user(user, PASS_MSG, true);
        }
    }
}

void    Server::_handle_auth(User &user, std::string cmd, std::string arg)
{
    if (cmd != "PASS")
        send_to_user(user, PASS_MSG, true);
    else if (arg.empty())
        send_to_user(user, "Missing parameter: PASS <password>", true);
    else if (arg == _pass)
    {
        user._set_auth(true);
        send_to_user(user, "[ ===== Successfully connected ===== ]\n" + USER_MSG + "\n" + NICK_MSG, true);
    }
    else
    {
        size_t  retry = user._get_retry();
        if (retry == 0)
        {
            std::string msg = "Exceeded max tries, disconnected";
            send(user._get_pfd()->fd, msg.c_str(), msg.length(), 0);
            _disconnect_user(user);
        }
        else {
            user._decr_retry();
            std::ostringstream  oss;
            oss << "Invalid password, please try again (" << retry << " tries left)";
            send_to_user(user, oss.str(), true);
        }
    }
}

void    Server::_config_msgs(User &user, bool prompt)
{
    std::string username = user._get_username();
    std::string nickname = user._get_nickname();
    bool        username_notset = username.empty();
    bool        nickname_notset = nickname.empty();

    if (username_notset && nickname_notset)
        send_to_user(user, USER_MSG + "\n" + NICK_MSG, prompt);
    else if (username_notset)
        send_to_user(user, USER_MSG, prompt);
    else if (nickname_notset)
        send_to_user(user, NICK_MSG, prompt);
    else
        send_to_user(user, "[ ===== Successfully authenticated ===== ]\nType \"HELP\" to see available commands", prompt);
}

void Server::_user_infos_setup(User &user, std::string name, const std::string field, Getter getter, Setter setter)
{
    if ((user.*getter)().empty())
    {
        if (!_is_name_valid(user, name, field, false))
            _config_msgs(user, true);
        else if (field == "Username" && !_is_username_available(name))
        {
            std::string msg = field + "\"" + name + "\" is not available";
            send_to_user(user, msg, false);
            _config_msgs(user, true);
        }
        else
        {
            (user.*setter)(name);
            std::string msg = "Successfully set \"" + name + "\" as " + to_lowercase(field);
            send_to_user(user, msg, false);
            _config_msgs(user, true);
        }
    }
    else
    {
        send_to_user(user, field + " is already set", false);
        _config_msgs(user, true);
    }
}

void    Server::_handle_setup(User &user, std::string cmd, std::string arg)
{
    if (cmd == "USER")
        _user_infos_setup(user, arg, "Username", &User::_get_username, &User::_set_username);
    else if (cmd == "NICK")
        _user_infos_setup(user, arg, "Nickname", &User::_get_nickname, &User::_set_nickname);
    else
        _config_msgs(user, true);
}

void    Server::_help_cmd(User &user)
{
    send_to_user(user, HELP_MSG, true);
}

void    Server::_logout_cmd(User &user)
{
    std::string msg = "Disconnected";
    send(user._get_pfd()->fd, msg.c_str(), msg.length(), 0);
    _disconnect_user(user);
}

void    Server::_whoami_cmd(User &user)
{
    std::string msg = "Username: " + user._get_username() + "\nNickname: " + user._get_nickname();
    send_to_user(user, msg, true);
}

void    Server::_update_nickname_cmd(User &user, std::string arg)
{
    if (_is_name_valid(user, arg, "Nickname", true))
    {
        std::string old = user._get_nickname();
        user._set_nickname(arg);
        std::string msg = "Successfully updated nickname from \"" + old + "\" to \"" + arg + "\"";
        send_to_user(user, msg, true);
    }
    
}

void    Server::_whisper_cmd(User &user, std::string arg)
{
    std::pair<std::string, std::string> splitted = split_first(arg, ' ');
    std::string dest_username = splitted.first;
    std::string msg = splitted.second;

    if (dest_username.empty())
    {
        send_to_user(user, "Missing parameter: WHISPER <username> <message>", true);
        return;
    }

    User    dest = _get_user_from_username(dest_username);
    if (dest._get_username().empty())
        send_to_user(user, "User not found", true);
    else
    {
        std::string final = "\033[2K\r" + user._get_nickname() + " (@" + user._get_username() + "): " + msg;
        send_to_user(dest, final, true);
        send(user._get_pfd()->fd, "> ", 2, 0);
    }
}

void    Server::_handle_message(User &user, std::string cmd, std::string arg)
{
    if (cmd == "HELP")
        _help_cmd(user);
    else if (cmd == "LOGOUT")
        _logout_cmd(user);
    else if (cmd == "WHOAMI")
        _whoami_cmd(user);
    else if (cmd == "NICK")
        _update_nickname_cmd(user, arg);
    else if (cmd == "WHISPER")
        _whisper_cmd(user, arg);
    else
        send_to_user(user, "Unknown command, type \"HELP\" to see available commands", true);
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
        if (msg.empty() || msg[msg.length() - 1] != '\n')
            return;

        msg.erase(msg.length() - 1);
        msg = clean_spaces(msg);
        std::pair<std::string, std::string> splitted = split_first(msg, ' ');
        std::string cmd = splitted.first;
        std::string arg = splitted.second;

        bool    auth = user._get_auth();
        if (!auth)
            _handle_auth(user, cmd, arg);
        else if (user._get_username().empty() || user._get_nickname().empty())
            _handle_setup(user, cmd, arg);
        else
            _handle_message(user, cmd, arg);
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
    size_t  pfd_i = _get_i_from_user(user);    
    size_t  user_i = pfd_i - 1;
    size_t  last_pfd_i = _users.size();
    size_t  last_user_i = last_pfd_i - 1;
    
    close(_pfds[pfd_i].fd);

    if (pfd_i != last_pfd_i)
    {
        _pfds[pfd_i] = _pfds[last_pfd_i];
        _users[user_i] = _users[last_user_i];
        _users[user_i]._set_pfd(&_pfds[pfd_i]);
    }

    _users.pop_back();
    _pfds[last_pfd_i].fd = -1;
    _pfds[last_pfd_i].events = 0;
    _pfds[last_pfd_i].revents = 0;

    std::cout << "User disconnected: " << ip << ":" << port << std::endl;
}

bool    Server::_is_username_available(std::string username)
{
    for (size_t i = 0; i < _users.size(); ++i)
        if (_users[i]._get_username() == username)
            return false;
    return true;
}

bool    Server::_is_name_valid(User &user, std::string name, const std::string field, bool prompt)
{
    if (name.empty())
    {
        if (field == "Username")
            send_to_user(user, "Missing parameter: USER <username>", prompt);
        else
            send_to_user(user, "Missing parameter: NICK <nickname>", prompt);
        return false;
    }
    if (name.length() < MIN_NAME_LEN)
    {
        std::ostringstream  oss;
        oss << field << " cannot be less than " << MIN_NAME_LEN << " characters";
        send_to_user(user, oss.str(), prompt);
        return false;
    }
    else if (name.length() > MAX_NAME_LEN)
    {
        std::ostringstream  oss;
        oss << field << " cannot be more than " << MAX_NAME_LEN << " characters";
        send_to_user(user, oss.str(), prompt);
        return false;
    }
    for (size_t i = 0; i < name.length(); i++)
    {
        if (!std::isalnum(name[i]) && name[i] != '_')
        {
            send_to_user(user, field + " can only contain letters, numbers and \'_\'", prompt);
            return false;
        }
    }
    
    return true;
}


// ==================== GETTERS ====================


User    Server::_get_user_from_username(std::string username)
{
    for (size_t i = 0; i < _users.size(); i++)
        if (_users[i]._get_username() == username)
            return _users[i];
    return User();
}


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
        for (size_t i = 0; i < CON_USER_LIMIT + 1; i++)
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
        for (size_t i = 0; i < CON_USER_LIMIT + 1; i++)
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
