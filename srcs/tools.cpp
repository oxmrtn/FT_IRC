#include "../includes/tools.hpp"

bool UserInVector(const User& user, const std::vector<User>& users)
{
    return std::find(users.begin(), users.end(), user) != users.end();
}
