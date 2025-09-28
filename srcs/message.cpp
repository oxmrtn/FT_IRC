#include "../includes/message.hpp"
#include "../includes/includes.hpp"


// ==================== METHODS ====================


void    Message::_default_init(void)
{
    _prefix = "";
    _command = "";
    _params = std::vector<std::string>();
    _trailing = "";
}

void Message::_parse(const std::string &msg)
{
    std::istringstream  iss(msg);
    std::string         token;

    if (iss.peek() == ':')
    {
        iss.get();
        std::getline(iss, _prefix, ' ');
    }
    iss >> _command;

    while (iss >> token)
    {
        if (!token.empty() && token[0] == ':')
        {
            _trailing = token.substr(1);
            std::string rest;
            std::getline(iss, rest);
            _trailing += rest;
            break;
        }
        else
            _params.push_back(token);
    }
}

std::string Message::_compose(void) const
{
    std::string msg;

    if (!_prefix.empty())
        msg += ":" + _prefix + " ";
    msg += _command;
    for (size_t i = 0; i < _params.size(); ++i)
        msg += " " + _params[i];
    if (!_trailing.empty())
        msg += " :" + _trailing;
    msg += "\r\n";

    return msg;
}

void Message::_send(User &target) const
{
    int fd = target._get_pfd()->fd;
    std::string msg = _compose();
    send(fd, msg.c_str(), msg.length(), 0);
}


// ==================== GETTERS ====================


const std::vector<std::string> &Message::_get_params(void) const { return _params; }
const std::string &Message::_get_trailing(void) const { return _trailing; }
const std::string &Message::_get_command(void) const { return _command; }
const std::string &Message::_get_prefix(void) const { return _prefix; }


// ==================== CONSTRUCTORS ====================


Message::Message(void)
{
    _default_init();
}

Message::Message(const Message &src)
{
    *this = src;
}

Message::Message(const std::string &msg)
{
    _default_init();
    _parse(msg);
}

Message::Message(const std::string prefix, const std::string command,
                 const std::vector<std::string> params, const std::string trailing)
{
    _prefix = prefix;
    _command = command;
    _params = params;
    _trailing = trailing;
}

Message::~Message() {}


// ==================== OPERATORS ====================


Message &Message::operator=(const Message &src)
{
    if (this != &src)
    {
        _prefix = src._prefix;
        _command = src._command;
        _params = src._params;
        _trailing = src._trailing;
    }
    return *this;
}
