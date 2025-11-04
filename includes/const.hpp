#pragma once

#define SERVER_NAME                 "irc.localhost"

#define CON_QUEUE                   10
#define CON_USER_LIMIT              100
#define MSG_BUF_SIZ                 512
#define MIN_NAME_LEN                3
#define MAX_NAME_LEN                9
#define MAX_USER_BY_CHANNEL         128

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

#define ERR_NOSUCHNICK_CODE        "401"
#define ERR_NOSUCHNICK_MSG         "<nick> :No such nick/channel"

#define ERR_NOSUCHCHANNEL_CODE     "403"
#define ERR_NOSUCHCHANNEL_MSG      "<channel> :No such channel"

#define ERR_TOOMANYCHANNELS_CODE   "405"
#define ERR_TOOMANYCHANNELS_MSG    "<channel> :You have joined too many channels"

#define ERR_UNKNOWNMODE_CODE       "472"
#define ERR_UNKNOWNMODE_MSG        "<char> :is unknown mode char to me"

#define ERR_CHANNELISFULL_CODE     "471"
#define ERR_CHANNELISFULL_MSG      "<channel> :Cannot join channel (+l)"

#define ERR_INVITEONLYCHAN_CODE    "473"
#define ERR_INVITEONLYCHAN_MSG     "<channel> :Cannot join channel (+i)"

#define ERR_BANNEDFROMCHAN_CODE    "474"
#define ERR_BANNEDFROMCHAN_MSG     "<channel> :Cannot join channel (+b)"

#define ERR_BADCHANNELKEY_CODE     "475"
#define ERR_BADCHANNELKEY_MSG      "<channel> :Cannot join channel (+k)"

#define ERR_BADCHANMASK_CODE       "476"
#define ERR_BADCHANMASK_MSG        "<channel> :Bad Channel Mask"

#define ERR_NOCHANMODES_CODE       "477"
#define ERR_NOCHANMODES_MSG        "<channel> :Channel doesn't support modes"

#define ERR_USERNOTINCHANNEL_CODE  "441"
#define ERR_USERNOTINCHANNEL_MSG   "<nick> <channel> :They aren't on that channel"

#define ERR_NOTONCHANNEL_CODE      "442"
#define ERR_NOTONCHANNEL_MSG       "<channel> :You're not on that channel"

#define ERR_USERONCHANNEL_CODE     "443"
#define ERR_USERONCHANNEL_MSG      "<nick> <channel> :is already on channel"

#define ERR_CHANOPRIVSNEEDED_CODE  "482"
#define ERR_CHANOPRIVSNEEDED_MSG   "<channel> :You're not channel operator"

#define ERR_USERSDONTMATCH_CODE    "502"
#define ERR_USERSDONTMATCH_MSG     ":Cannot change mode for other users"

#define ERR_INVALIDMODEPARAMS_CODE "696" 