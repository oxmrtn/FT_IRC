#pragma once

#include "./includes.hpp"
#include "./user.hpp"
#include "./tools.hpp"

class Message
{
    private:
        std::string sender;
        std::string content;
        std::string timestamp;
        Message();

    public:
        Message(User & sender, std::string message);
        Message(Message & other);
        Message & operator=(Message & other);
        ~Message();
        void displayMessage();
};

