#pragma once

#include "./includes.hpp"

class User;


class Message
{
    private:
        std::string sender;
        std::string content;
        std::string timestamp;
        Message();

    public:
        Message(User sender, std::string message);
        Message(Message & other);
        Message & operator=(Message & other);
        ~Message();
        void displayMessage();
};

