#include "../includes/includes.hpp"

std::string to_low(const std::string &str)
{
    std::string low(str.length(), ' ');
    for (size_t i = 0; i < str.length(); i++)
        low[i] = std::tolower(str[i]);
    return low;
}

bool    ends_with(const std::string &str, const std::string &suffix)
{
    if (str.length() < suffix.length())
        return false;
    return !str.compare(str.length() - suffix.length(), suffix.length(), suffix);
}

bool    starts_with(const std::string &str, const std::string &prefix)
{
    if (str.length() < prefix.length())
        return false;
    return !str.compare(0, prefix.length(), prefix);
}

void    send_to_user(User &user, const std::string &msg)
{
    int fd = user._get_pfd()->fd;
    send(fd, msg.c_str(), msg.length(), 0);
}

int err_ret(const std::string msg)
{
    std::cerr << "error: " << msg << std::endl;
    return (1);
}

int is_zero(const std::string str)
{
    for (size_t i = 0; i < str.length(); i++)
        if (str[i] != '0')
            return (0);

    return (1);
}
