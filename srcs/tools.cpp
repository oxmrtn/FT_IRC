#include "../includes/tools.hpp"

bool UserInVector(const User& user, const std::vector<User*>& users)
{
        for (std::vector<User*>::const_iterator it = users.begin(); it != users.end(); ++it) {
        if (**it == user)
            return true;
    }
    return false;
}

void remUserInVector(const User &user, std::vector<User*> &users)
{
    for (std::vector<User*>::iterator it = users.begin(); it != users.end(); ++it)
    {
        if (**it == user)
        {
            users.erase(it);
            return;
        }
    }
    return ;
}

std::string getTimestamp()
{
    std::time_t now = std::time(NULL);
    std::tm *local_tm = std::localtime(&now);

    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%d/%m/%Y - %H-%M", local_tm);

    return (std::string(buffer));
}

User *getUserByUname(const std::string& nickname, std::vector<User*> & list)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        if (list[i]->_get_nickname() == nickname)
            return list[i];
    }
    throw ErrUserNotFound(nickname);
}

User & getUserByUname_ref(const std::string& nickname, std::vector<User> & list)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        if (list[i]._get_nickname() == nickname)
            return list[i];
    }
    throw ErrUserNotFound(nickname);
}

Channel & getChanbyName(const std::string & channame, std::vector<Channel> & list)
{
        for (size_t i = 0; i < list.size(); i++)
    {
        if (list[i]._getName() == channame)
            return list[i];
    }
    throw ErrNoSuchChannel(channame);
}

void    _send_message_to_users(const User & sender, const User & receiver, std::string content)
{
    std::string message = sender._get_prefix() + " PRIVMSG " + receiver._get_nickname() + " :" + content;
    _send_raw_string(receiver, message);
}

void _send_raw_string(const User &receiver, std::string msg)
{   
    int         fd = receiver._get_pfd()->fd;
    size_t      msg_len = msg.length();
    size_t      start = 0;
    size_t      end = 0;
    std::string pre = "";
    if (!msg.empty() && msg[0] == ':')
    {
        size_t  pos = msg.find(' ');
        pre = msg.substr(0, pos + 1);
    }
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

static unsigned char tolower_func(unsigned char c)
{
    return static_cast<unsigned char>(std::tolower(c));
}

std::string str_tolower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), tolower_func);
    return s;
}

void handle_bot(Channel & chan, std::string trailing, User & user)
{
    bool bot = false;
    std::string to_send = ":bot!bot@irc.localhost PRIVMSG " + chan._getName() + " :";
    if (trailing.find("!hello") != std::string::npos)
    {
        bot = true;
        to_send += " Hello " + user._get_nickname() + " ! How are you ?";
    }
    else if (trailing.find("!count") != std::string::npos)
    {
        bot = true;
        std::ostringstream oss;
        oss << chan._getCount();
        to_send += " There is actually " + oss.str() + " people connected on this channel !";
    }
    else if (trailing.find("!time") != std::string::npos)
    {
        bot = true;
        std::time_t now = std::time(NULL);
        std::tm *ltm = std::localtime(&now);

        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%H:%M:%S", ltm);

        to_send +=  "It is exactly ";
        to_send += buffer ;
        to_send += " right now !";
    }
    if (bot) { chan._send_message_to_channel(user, to_send, true);}
    return ;
}