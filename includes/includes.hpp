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
#include <consts.hpp>
#include <cerrno>

int err_ret(const std::string msg);
