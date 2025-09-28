#pragma once

#define SERVER_NAME                 "irc.localhost"

#define CON_QUEUE                   5
#define CON_USER_LIMIT              100
#define MSG_BUF_SIZ                 512
#define MIN_NAME_LEN                3
#define MAX_NAME_LEN                9

#define ERR_UNKNOWNCOMMAND_CODE     "412"
#define ERR_UNKNOWNCOMMAND_MSG      "Unknown command"
#define ERR_NEEDMOREPARAMS_CODE     "461"
#define ERR_NEEDMOREPARAMS_MSG      "Not enough parameters"

#define ERR_NOTREGISTERED_CODE      "451"
#define ERR_NOTREGISTERED_MSG       "You have not registered"
#define ERR_PASSWDMISMATCH_CODE     "464"
#define ERR_PASSWDMISMATCH_MSG      "Password incorrect"
#define ERR_ALREADYREGISTERED_CODE  "462"
#define ERR_ALREADYREGISTERED_MSG   "You may not reregister"

#define ERR_NONICKNAMEGIVEN_CODE    "431"
#define ERR_NONICKNAMEGIVEN_MSG     "No nickname given"
#define ERR_ERRONEUSNICKNAME_CODE   "432"
#define ERR_ERRONEUSNICKNAME_MSG    "Erroneous nickname"
#define ERR_NICKNAMEINUSE_CODE      "433"
#define ERR_NICKNAMEINUSE_MSG       "Nickname is already in use"