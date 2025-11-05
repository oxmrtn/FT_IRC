#pragma once

#include "./includes.hpp"
#include "exception.hpp"

class User;
class Channel;

bool        UserInVector(const User& user, const std::vector<User*>& users);
std::string getTimestamp();
User        *getUserByUname(const std::string& username, std::vector<User*>& list);
User        &getUserByUname_ref(const std::string& username, std::vector<User> & list);
Channel & getChanbyName(const std::string & channame, std::vector<Channel> & list);
void        remUserInVector(const User &user, std::vector<User*> &users);
void        _send_message_to_users(const User & sender, const User & receiver, std::string content);
void        _send_raw_string(const User &receiver, std::string msg);
std::string str_tolower(std::string s);
void handle_bot(Channel & chan, std::string trailing, User & user);
