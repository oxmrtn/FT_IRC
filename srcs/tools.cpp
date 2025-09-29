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

User* getUserByUname(const std::string& username, const std::vector<User*>& list)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        if (list[i]->_get_username() == username)
            return list[i];
    }
    throw UserNotFound();
}

const char *UserNotFound::what() const throw()
{
    return "error: user not found";
}


