#pragma once

const int           CONS_QUEUE = 5;
const int           MAX_CONS = 100;
const int           MSG_BUF_SIZ = 100;
const int           MAX_RETRY = 3;

const std::string   PASS_MSG = "Please authenticate with \"PASS <password>\"";
const std::string   USER_MSG = "Please set your username with \"USER <username>\"";
const std::string   NICK_MSG = "Please set your nickname with \"NICK <nickname>\"";
const std::string   HELP_MSG =
    "[ ===== HELP MENU ===== ]\n"
    "|    GLOBAL COMMANDS    |\n"
    "- HELP\n\tDisplays help menu\n"
    "- DISCONNECT\n\tDisconnect yourself from the IRC server\n"
    "- ME\n\tDisplays your username and nickname\n"
    "- NICK <nickname>\n\tChange your nickname\n"
    "|    CHANNEL COMMANDS   |\n"
    "- KICK <username>\n\tKick a user from the channel\n"
    "- INVITE <username>\n\tInvite a user in the channel\n"
    "- TOPIC <topic (optionnal)>\n\tEdit or display the channel\'s topic\n"
    "- MODE\n"
    "[ ===================== ]";
    