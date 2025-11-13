#include "../includes/includes.hpp"
#include "../includes/message.hpp"


// ==================== METHODS ====================


void Server::_init(int port, std::string pass)
{
    _users.reserve(CON_USER_LIMIT);
    _pfds[0].fd = STDIN_FILENO;
    _pfds[0].events = POLLIN;

    _pfds[1].fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_pfds[1].fd < 0)
        throw SocketInitError();
    int opt = 1;
    if (setsockopt(_pfds[1].fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw SocketInitError();
    if (setsockopt(_pfds[1].fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0)
        throw SocketInitError();

    _pfds[1].events = POLLIN;

    _pass = pass;

    sockaddr_in tmp_addr;
    tmp_addr.sin_family = AF_INET;
    tmp_addr.sin_port = htons(port);
    tmp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(tmp_addr.sin_zero, 0, sizeof(tmp_addr.sin_zero));

    if (bind(_pfds[1].fd, reinterpret_cast<struct sockaddr*>(&tmp_addr), sizeof(tmp_addr)) < 0)
        throw SocketBindError();
        
    if (listen(_pfds[1].fd, CON_QUEUE) < 0)
        throw SocketListenError();

    socklen_t   len = sizeof(_addr);
    getsockname(_pfds[1].fd, reinterpret_cast<sockaddr*>(&_addr), &len);
    int running_port = ntohs(_addr.sin_port);

    std::cout << "IRC server running on port: " << running_port << std::endl;
}

bool    Server::_handle_sigquit(void)
{
    char    buf[MSG_BUF_SIZ];

    ssize_t r_bytes = read(_pfds[0].fd, buf, MSG_BUF_SIZ - 1);

    if (r_bytes == 0)
    {
        std::cerr << "^D" << std::endl;
        g_sig = 0;
        return true;
    }
    else if (r_bytes < 0)
        err_ret(strerror(errno));
    return false;
}

void    Server::_run(void)
{
    while (g_sig)
    {
        int polled = poll(_pfds, _users.size() + 2, 0);
        if (polled < 0 && errno != EINTR)
            err_ret(strerror(errno));
        else if (polled > 0)
        {
            if (_pfds[0].revents == POLLIN)
                if (_handle_sigquit())
                    return;
            if (_pfds[1].revents == POLLIN)
                _handle_connection();
            for (size_t i = 2; i < _users.size() + 2; i++)
                if (_pfds[i].revents == POLLIN)
                    _process_polled(i);
        }
    }
}

void    Server::_handle_connection(void)
{
    User        user;
    sockaddr_in &addr = user._get_addr();
    socklen_t   len = sizeof(addr);

    int fd = accept(_pfds[1].fd, reinterpret_cast<struct sockaddr*>(&addr), &len);
    if (fd < 0)
        err_ret(strerror(errno));
    else
    {
        char    *ip = inet_ntoa(addr.sin_addr);
        int     port = ntohs(addr.sin_port);
        int     i = _users.size() + 2;
        
        if (_users.size() == CON_USER_LIMIT)
        {
            std::cout << "User connection refused: " << ip << ":" << port << std::endl;        
            close(fd);
        }
        else
        {
            _pfds[i].fd = fd;
            _pfds[i].events = POLLIN;
            _pfds[i].revents = -1;
            user._set_pfd(&_pfds[i]);
            _users.push_back(user);
    
            std::cout << "User connection accepted: " << ip << ":" << port << std::endl;
        }
    }
}

void Server::_disconnect_user(User &user)
{
    std::string nick = user._get_nickname();
    if (nick.empty())
        nick = "Unauthenticated user";
    else
        nick = "User \"" + nick + "\"";
    char        *ip = inet_ntoa(user._get_addr().sin_addr);
    int         port = ntohs(user._get_addr().sin_port);
    size_t      pfd_i = _get_i_from_user(user);
    size_t      user_i = pfd_i - 2;
    size_t      last_pfd_i = _users.size() + 1;
    size_t      last_user_i = last_pfd_i - 2;
    
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

    std::cout << nick << " (" << ip << ":" << port << ")" << " disconnected" << std::endl;
}

void    Server::_send_response(User &user, std::string command, std::string error_code, std::string trailing)
{
    std::vector<std::string>    response_params;
    std::string                 nickname = user._get_nickname();

    if (nickname.empty())
        response_params.push_back("*");
    else
        response_params.push_back(nickname);
    if (!command.empty())
        response_params.push_back(command);
    Message reponse(SERVER_NAME, error_code, response_params, trailing);
    reponse._send(user);
}

void    Server::_send_response(User & user,  std::string error_code, std::vector<std::string> response_params, std::string trailing)
{
    std::string nickname = user._get_nickname();

    if (nickname.empty())
        response_params.insert(response_params.begin(), "*");
    Message reponse(SERVER_NAME, error_code, response_params, trailing);
    reponse._send(user);
}

void Server::_send_error(User &user, const std::string &command, const ChannelException &e)
{
    std::string code;
    std::vector<std::string> params;
    std::string target = e.getTarget();

    if      (dynamic_cast<const ErrNoSuchNick*>(&e))        code = ERR_NOSUCHNICK_CODE;
    else if (dynamic_cast<const ErrNoSuchChannel*>(&e))     code = ERR_NOSUCHCHANNEL_CODE;
    else if (dynamic_cast<const ErrTooManyChannels*>(&e))   code = ERR_TOOMANYCHANNELS_CODE;
    else if (dynamic_cast<const ErrUnknownMode*>(&e))       code = ERR_UNKNOWNMODE_CODE;
    else if (dynamic_cast<const ErrChannelIsFull*>(&e))     code = ERR_CHANNELISFULL_CODE;
    else if (dynamic_cast<const ErrInviteOnlyChan*>(&e))    code = ERR_INVITEONLYCHAN_CODE;
    else if (dynamic_cast<const ErrBannedFromChan*>(&e))    code = ERR_BANNEDFROMCHAN_CODE;
    else if (dynamic_cast<const ErrBadChannelKey*>(&e))     code = ERR_BADCHANNELKEY_CODE;
    else if (dynamic_cast<const ErrBadChanMask*>(&e))       code = ERR_BADCHANMASK_CODE;
    else if (dynamic_cast<const ErrNoChanModes*>(&e))       code = ERR_NOCHANMODES_CODE;
    else if (dynamic_cast<const ErrUserNotInChannel*>(&e))  code = ERR_USERNOTINCHANNEL_CODE;
    else if (dynamic_cast<const ErrNotOnChannel*>(&e))      code = ERR_NOTONCHANNEL_CODE;
    else if (dynamic_cast<const ErrUserOnChannel*>(&e))     code = ERR_USERONCHANNEL_CODE;
    else if (dynamic_cast<const ErrChanOpPrivsNeeded*>(&e)) code = ERR_CHANOPRIVSNEEDED_CODE;
    else if (dynamic_cast<const ErrUsersDontMatch*>(&e))    code = ERR_USERSDONTMATCH_CODE;
    else if (dynamic_cast<const ErrNeedMoreParams*>(&e))    code = ERR_NEEDMOREPARAMS_CODE;
    else if (dynamic_cast<const ErrInvalidModeParams*>(&e)) code = ERR_INVALIDMODEPARAMS_CODE;
    else if (dynamic_cast<const ErrUserNotFound*>(&e))      code = ERR_NOSUCHNICK_CODE;
    else                                                    code = "400";

    params.push_back(user._get_nickname());

    if (!target.empty() && target[0] == '#')
        params.push_back(target);
    else if (dynamic_cast<const ErrNeedMoreParams*>(&e))
        params.push_back(command);

    else if (dynamic_cast<const ErrUnknownMode*>(&e) || dynamic_cast<const ErrInvalidModeParams*>(&e))
    {
        params.push_back(command);
    }
    _send_response(user, code, params, e.what());
}

bool    Server::_is_supported_command(std::string command, std::vector<std::string> params)
{
    bool        valid_cmd = false;
    const char  *supported_cmds_arr[] = {
        "PASS", "NICK", "USER", "JOIN", "KICK", "INVITE", "TOPIC", "MODE", "PRIVMSG", "CAP", "PING", "QUIT"
    };

    for (size_t i = 0; i < sizeof(supported_cmds_arr) / sizeof(char *); i++)
        if (supported_cmds_arr[i] == command)
            valid_cmd = true;

    if (command.empty() || params.size() > 15 || !valid_cmd)
        return false;
    return true;
}

void    Server::_handle_pass(User &user, Auth auth, std::string command, std::vector<std::string> params)
{
    if (auth != NOT)
        _send_response(user, "", ERR_ALREADYREGISTERED_CODE, ERR_ALREADYREGISTERED_MSG);
    else if (params.empty())
        _send_response(user, command, ERR_NEEDMOREPARAMS_CODE, ERR_NEEDMOREPARAMS_MSG);
    else if (params[0] != _pass)
    {
        _send_response(user, "", ERR_PASSWDMISMATCH_CODE, ERR_PASSWDMISMATCH_MSG);
        _disconnect_user(user);
    }
    else
        user._set_auth(PASS);
}

void Server::_handle_names(User &user, Auth auth, std::string command, std::vector<std::string> params, std::string trailing)
{
    std::string name;

    if (!params.empty())
        name = params[0];

    if (auth == NOT)
        _send_response(user, "", ERR_NOTREGISTERED_CODE, ERR_NOTREGISTERED_MSG);
    else if (command == "USER")
    {
        if (!user._get_username().empty())
            _send_response(user, "", ERR_ALREADYREGISTERED_CODE, ERR_ALREADYREGISTERED_MSG);
        else if (params.size() < 3 || trailing.empty())
            _send_response(user, "", ERR_NEEDMOREPARAMS_CODE, ERR_NEEDMOREPARAMS_MSG);
        else if (!_is_name_valid(user, name, false))
            return;
        std::string hostname = params[1];
        user._set_username(name);
        user._set_hostname(hostname);
    }
    else if (command == "NICK")
    {
        if (!_is_name_valid(user, name, true))
            return;
        else if (!user._get_nickname().empty() && user._get_nickname() == name)
            return;
        else if (!_is_nickname_available(name))
            _send_response(user, name, ERR_NICKNAMEINUSE_CODE, ERR_NICKNAMEINUSE_MSG);
        else
            user._set_nickname(name);
    }

    std::string username = user._get_username();
    std::string nickname = user._get_nickname();
    if (auth == PASS && !username.empty() && !nickname.empty())
    {
        std::string welcome = "Welcome to the Internet Relay Network " + nickname + "!" + username + "@" + SERVER_NAME;
        _send_response(user, "", "001", welcome);

        std::string yourhost = "Your host is " + std::string(SERVER_NAME) + ", running version 1.0";
        _send_response(user, "", "002", yourhost);

        std::string created = "This server was created Tue Oct 14 2025";
        _send_response(user, "", "003", created);

        std::string myinfo = std::string(SERVER_NAME) + " 1.0";
        _send_response(user, myinfo, "004", "");

        user._set_auth(FULL);
    }
}

void    Server::_handle_ping(User &user, std::string trailing)
{
    std::string message = "PONG :" + trailing + "\r\n";
    _send_raw_string(user, message);
}

void    Server::_handle_message(User &user, Message &parsed)
{
    Auth                        auth = user._get_auth();
    std::string                 command = parsed._get_command();
    std::string                 trailing = parsed._get_trailing();
    std::vector<std::string>    params = parsed._get_params();

    if (!_is_supported_command(command, params))
        _send_response(user, command, ERR_UNKNOWNCOMMAND_CODE, ERR_UNKNOWNCOMMAND_MSG);
    else if (command == "QUIT")
        _disconnect_user(user);
    else if (command == "PASS")
        _handle_pass(user, auth, command, params);
    else if (command == "USER" || command == "NICK")
        _handle_names(user, auth, command, params, trailing);
    else if (command == "PING")
        _handle_ping(user, trailing);
    else if (command == "CAP")
        _send_response(user, "LS :", command, "");
    else if (auth == NOT)
        _send_response(user, "", ERR_NOTREGISTERED_CODE, ERR_NOTREGISTERED_MSG);
    else if (command == "JOIN")
    {
        std::string chan_name = str_tolower(params[0]);
        try {
            Channel &chan = getChanbyName(chan_name, _channels);
            std::string key = "";
            if (params.size() == 2)
            {
                key = params[1];
            }
            if (chan.join(&user, key))
                std::cout << user._get_username() << " successfully joined " << chan._getName() << std::endl;
            else
                std::cout << user._get_username() << " didnt joined " << chan._getName() << std::endl;
        }catch (ErrNoSuchChannel &e)
        {
            Channel chan(chan_name);
            chan.addOpp(&user, true);
            _channels.push_back(chan);
        }
        catch(ChannelException &e)
        {
            _send_error(user, command, e);
        }
    }
    else if (command == "KICK")
    {
        std::string chan_name = str_tolower(params[0]);
        try{
            Channel &chan = getChanbyName(chan_name, _channels);
            User & tokick = getUserByUname_ref(params[1], _users );
            if (chan.kick(&tokick, &user, parsed._get_trailing()))
            {
            std::cout << user._get_username() << "successfully kicked " << tokick._get_username() << "out of " << chan._getName() << std::endl;
            }
            return ;
        }
        catch(ChannelException &e)
        {
            _send_error(user, command, e);
            return ;
        }
    }
    else if (command == "INVITE")
    {
        std::string chan_name = str_tolower(params[1]);
        try{
            if (params.size() != 2)
            {
                throw ErrNeedMoreParams("KICK");
            }
            Channel & chan = getChanbyName(chan_name, _channels);
            User & toinvite = getUserByUname_ref(params[0], _users);
            if (chan.invite(&toinvite, &user))
            {
                    std::cout << toinvite._get_username() << " was succcessfully invited to " << chan._getName() << std::endl;
            }
            return ;
        }catch(ChannelException &e)
        {
            _send_error(user, command, e);
            return ;
        }
    }
    else if (command == "TOPIC")
    {
        try{
            if (params.empty())
                throw ErrNeedMoreParams("Topic");
            Channel  &chan = getChanbyName(str_tolower(params[0]), _channels);
            if (parsed._get_trailing().size() == 0)
            {
                if (!chan._is_user_in_chan(user))
                    throw ErrUserNotInChannel(user._get_nickname(), chan._getName());
                if (chan._getTopic().size() == 0)
                {
                    std::string message = ":";
                    message += SERVER_NAME;
                    message += " 331 " + user._get_nickname() + " " + chan._getName() + " :No topic is set";
                    _send_raw_string(user, message);
                }
                else
                {
                    std::string topic = ":";
                    topic += SERVER_NAME;
                    std::string whotime = topic;
                    topic += " 332 " + user._get_nickname() + " " + chan._getName() + " :" + chan._getTopic();
                    whotime  += " 333 " + user._get_nickname() + " " + chan._getTopicSetter() + " " + chan._getTopicTime();
                    _send_raw_string(user, topic);
                    _send_raw_string(user, whotime);
                }
                return ;
            }
            else
            {
                if (chan.setTopic(parsed._get_trailing(), user))
                {
                    std::cout << user._get_username() << "successfully changed topic " << std::endl;
                }
            }
        }catch(ChannelException &e)
        {
            _send_error(user, command, e);
            return ;
        }
        
    } 
    else if (command == "MODE" && !params.empty() && params[0][0] == '#')
    {
        std::string chan_name = str_tolower(params[0]);
        try {
            Channel &chan = getChanbyName(chan_name, _channels);
            size_t nbr_command = (params[1].size() > 1 ? params[1].size() - 1 : 1);
            size_t arg_command = 2;
            for (size_t i = 0; i < nbr_command; i++)
            {
                char sign = (params.size() > 1 ? params[1][0] : '*');
                char mode = (params.size() > 1 ? params[1][i + 1] : '*');
                std::string args = (params.size() > arg_command ? params[arg_command] : "");
                if ((sign != '+' && sign != '-') || (std::string("iotkl").find(mode)) == std::string::npos)
                    throw ErrUnknownMode(std::string(mode, 1));
                if (std::string("o").find(mode) != std::string::npos || (std::string("lk").find(mode) != std::string::npos && sign == '+'))
                    arg_command++;
                chan.mode(mode, user, sign, args);
            }
            return ;
        }catch(ChannelException &e)
        {
            _send_error(user, command, e);
            return ;
        }
    }
    else if (command == "PRIVMSG")
    {
        bool message_to_channel = (params[0][0] == '#' ? true : false);
        std::string chan_name = str_tolower(params[0]);
        if (message_to_channel)
        {
            try{
                Channel &chan = getChanbyName(chan_name, _channels);
                std::string to_send = user._get_prefix() + " PRIVMSG " + chan._getName() + " :" + parsed._get_trailing();
                chan._send_message_to_channel(user, to_send, false);
                handle_bot(chan, trailing, user);
                return ;
            }catch(ChannelException &e)
            {
                _send_error(user, command, e);
                return ;
            }
        }
        else
        {
            try{
                User & receiver = getUserByUname_ref(chan_name, _users);
                _send_message_to_users(user, receiver, parsed._get_trailing());
                return ;
            }catch (ChannelException &e)
            {
                _send_error(user, command, e);
                return ;
            }
         
        }
    }
}

void    Server::_process_polled(size_t user_i)
{
    User    &user = _get_user_from_i(user_i);
    char    buf[MSG_BUF_SIZ];

    size_t r_bytes = recv(_pfds[user_i].fd, buf, MSG_BUF_SIZ - 1, 0);
    
    if (r_bytes > 0) {
        buf[r_bytes] = '\0';
        user._set_msg(buf, true);
        std::string msg = user._get_msg();
        size_t pos = 0;

        while ((pos = msg.find("\r\n")) != std::string::npos) {
            std::string line = msg.substr(0, pos);
            line = clean_spaces(line);
            if (!line.empty()) {
                Message parsed(line);
                _handle_message(user, parsed);
            }
            if (_pfds[user_i].fd == -1)
                return;
            msg.erase(0, pos + 2);
        }
        
        user._set_msg(msg, false);
    }
    else if (r_bytes == 0)
        _disconnect_user(user);
    else
        err_ret(strerror(errno));
}

bool    Server::_is_nickname_available(std::string nickname)
{
    for (size_t i = 0; i < _users.size(); ++i)
        if (_users[i]._get_nickname() == nickname)
            return false;
    return true;
}

bool    Server::_is_char_accepted(char c, bool is_nick)
{
    if (is_nick && c != '-' && c != '[' && c != ']' && c != '{' && c != '}' && c != '^' && c != '_' && c != '|')
        return false;
    else if (!is_nick && c != '_')
        return false;
    return true;
}

bool    Server::_is_name_valid(User &user, std::string name, bool is_nick)
{
    if (name.empty())
    {
        if (is_nick)
            _send_response(user, name, ERR_NONICKNAMEGIVEN_CODE, ERR_NONICKNAMEGIVEN_MSG);
        else
            _send_response(user, name, ERR_NEEDMOREPARAMS_CODE, ERR_NEEDMOREPARAMS_MSG);
        return false;
    }

    size_t  max_len = !is_nick ? (MAX_NAME_LEN + 1) : MAX_NAME_LEN;
    if (name.length() < MIN_NAME_LEN || name.length() > max_len)
    {
        if (is_nick)
            _send_response(user, name, ERR_ERRONEUSNICKNAME_CODE, ERR_ERRONEUSNICKNAME_MSG);
        else
            _send_response(user, name, ERR_NEEDMOREPARAMS_CODE, ERR_NEEDMOREPARAMS_MSG);
        return false;
    }
 
    if (is_nick && !std::isalpha(name[0]))
    {
        _send_response(user, name, ERR_ERRONEUSNICKNAME_CODE, ERR_ERRONEUSNICKNAME_MSG);
        return false;
    }

    for (size_t i = 0; i < name.length(); ++i)
    {
        char c = name[i];

        if (!std::isalnum(c) && !_is_char_accepted(c, is_nick))
        {
            if (is_nick)
                _send_response(user, name, ERR_ERRONEUSNICKNAME_CODE, ERR_ERRONEUSNICKNAME_MSG);
            else
                _send_response(user, name, ERR_NEEDMOREPARAMS_CODE, ERR_NEEDMOREPARAMS_MSG);
            return false;
        }
    }

    return true;
}


// ==================== GETTERS ====================


User    Server::_get_user_from_username(std::string username) const
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
    throw ErrUserNotFound("null");
}

size_t  Server::_get_i_from_user(User &user) const
{
    for (size_t i = 2; i < _users.size() + 2; i++)
        if (user._get_pfd() == &_pfds[i])
            return i;
    throw ErrUserNotFound(user._get_nickname());
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
    for (size_t i = 0; i < CON_USER_LIMIT + 2; i++)
        _pfds[i] = src._pfds[i];
    _addr = src._addr;
    _pass = src._pass;
    _users = src._users;
}

Server::~Server()
{
    close(_pfds[1].fd);
    for (size_t i = 2; i < _users.size() + 2; i++)
        close(_pfds[i].fd);
}


// ==================== OPERATORS ====================


Server  &Server::operator=(const Server &src)
{
    if (this != &src)
    {
        for (size_t i = 0; i < CON_USER_LIMIT + 2; i++)
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

