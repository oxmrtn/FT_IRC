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

void Message::_send(User &target)
{
    int         fd = target._get_pfd()->fd;
    std::string msg = _compose();
    size_t      msg_len = msg.length();
    size_t      start = 0;
    size_t      end = 0;
    std::string pre = ":" + _prefix + " ";
    size_t      pre_len = pre.length();

    while (end != std::string::npos)
    {
        std::string to_send;
        size_t      pre_reserved = start != 0 ? pre_len : 0;

        if (msg_len - (start + pre_reserved) > 512)
            end = 510 - pre_reserved;
        else
            end = std::string::npos;
        to_send = msg.substr(start, end);
        if (pre_reserved != 0)
            to_send = pre + to_send;
        if (!ends_with(to_send, "\r\n"))
            to_send += "\r\n";
        send(fd, to_send.c_str(), to_send.length(), 0);
        start += 510 - pre_reserved;
    }
}


// ==================== GETTERS ====================


const std::vector<std::string>  &Message::_get_params(void) const { return _params; }
const std::string               &Message::_get_trailing(void) const { return _trailing; }
const std::string               &Message::_get_command(void) const { return _command; }
const std::string               &Message::_get_prefix(void) const { return _prefix; }


// ==================== CONSTRUCTORS ====================


Message::Message(void)
{
    _default_init();
}

Message::Message(const Message &src)
{
    _prefix = src._prefix;
    _command = src._command;
    _params = src._params;
    _trailing = src._trailing;
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

Message::~Message()
{}


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
