#pragma once

#include "./includes.hpp"

class User;
class Channel;

bool        UserInVector(const User& user, const std::vector<User*>& users);
std::string getTimestamp();
User        *getUserByUname(std::string username, std::vector<User*> list);
void        remUserInVector(const User &user, std::vector<User*> &users);


class UserNotFound : public std::exception
    {   public: virtual const char *what() const throw();};
