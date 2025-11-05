#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <deque>
#include <string>
#include <cerrno>
#include <sstream>
#include <algorithm>
#include "const.hpp"
#include "user.hpp"
#include "server.hpp"

int         err_ret(const std::string msg);
int         is_zero(const std::string str);
std::string to_lowercase(const std::string &str);
bool        ends_with(const std::string &str, const std::string &suffix);
std::string trim_spaces(const std::string &str);
std::string deduplicate_spaces(const std::string &str);
std::string clean_spaces(const std::string &str);
std::pair<std::string, std::string>
            split_first(const std::string &str, char delimiter);

extern int  g_sig;
