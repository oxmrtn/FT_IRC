#include "../includes/message.hpp"


Message::Message(){}

Message::Message(User & sender, std::string message)
{
    this->sender = sender.getNickname();
    this->content = message;
    this->timestamp = getTimestamp();
    this->_message = this->timestamp + " -- " + this->sender + "\n" + this->content +  "\n";
}

Message::Message(Message & other)
        : sender(other.sender),
        content(other.content),
        timestamp(other.timestamp)
{}

Message & Message::operator=(Message &other)
{
    if (this != &other)
    {
        sender = other.sender;
        content = other.content;
        timestamp = other.timestamp;
    }
    return (*this);
}

std::string & Message::getMessage()
{
    return _message;
}

