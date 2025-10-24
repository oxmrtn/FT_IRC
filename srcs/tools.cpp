#include "../includes/tools.hpp"

bool UserInVector(const User& user, const std::vector<User*>& users)
{
        for (std::vector<User*>::const_iterator it = users.begin(); it != users.end(); ++it) {
        if (*it == &user)
            return true;
    }
    return false;
}

void remUserInVector(const User &user, std::vector<User*> &users)
{
    for (std::vector<User*>::iterator it = users.begin(); it != users.end(); ++it)
    {
        if (*it == &user)
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
    int fd = receiver._get_pfd()->fd;
    if (msg.size() < 2 || msg.substr(msg.size() - 2) != "\r\n")
        msg += "\r\n";
    send(fd, msg.c_str(), msg.size(), 0);
}