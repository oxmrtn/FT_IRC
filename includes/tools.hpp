#pragma once

#include "./includes.hpp"

class User;
class Channel;

bool        UserInVector(const User& user, const std::vector<User*>& users);
std::string getTimestamp();
User        *getUserByUname(const std::string& username, const std::vector<User*>& list);
const Channel & getChanbyName(const std::string & channame, const std::vector<Channel> & list);
void        remUserInVector(const User &user, std::vector<User*> &users);


class UserNotFound : public std::exception
    {   public: virtual const char *what() const throw();};


class ChannelNotFound : public std::exception
    {   public: virtual const char *what() const throw();};
