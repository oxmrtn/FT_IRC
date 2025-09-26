#pragma once

const int           MAX_MSG_CHANNEL = 20;
const int           CON_QUEUE = 5;
const int           CON_USER_LIMIT = 100;
const int           CON_RETRIES = 3;
const int           MSG_BUF_SIZ = 512;
const int           MIN_NAME_LEN = 3;
const int           MAX_NAME_LEN = 10;

const std::string   PASS_MSG = "Please authenticate with \"PASS <password>\"";
const std::string   USER_MSG = "Please set your username with \"USER <username>\"";
const std::string   NICK_MSG = "Please set your nickname with \"NICK <nickname>\"";
const std::string   HELP_MSG =
    "[ ===== HELP MENU ===== ]\n"
    "|    GLOBAL COMMANDS    |\n"
    "- HELP\n\tDisplays the help menu\n"
    "- LOGOUT\n\tDisconnect yourself from the IRC server\n"
    "- WHOAMI\n\tDisplays your username and nickname\n"
    "- NICK <nickname>\n\tChange your nickname\n"
    "- WHISPER <username> <message>\n\tSend a private message to the specified user\n"
    "|    CHANNEL COMMANDS   |\n"
    "- KICK <username>\n\tKick a user from the channel\n"
    "- INVITE <username>\n\tInvite a user in the channel\n"
    "- TOPIC <topic (optionnal)>\n\tEdit or display the channel\'s topic\n"
    "- MODE\n"
    "[ ===================== ]";
    
