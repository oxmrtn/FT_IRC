#include "../includes/tools.hpp"

bool UserInVector(const User& user, const std::vector<User>& users)
{
    return std::find(users.begin(), users.end(), user) != users.end();
}

std::string getTimestamp()
{
    std::time_t now = std::time(NULL);
    std::tm *local_tm = std::localtime(&now);

    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%d/%m/%Y - %H-%M", local_tm);

    return (std::string(buffer));
}
