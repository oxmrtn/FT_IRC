#include "../includes/message.hpp"


Message::Message(){}

Message::Message(User & sender, std::string message)
{
    this->sender = sender.getNickname();
    this->content = message;
    this->timestamp = getTimestamp();
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

void Message::displayMessage()
{
    std::cout << this->timestamp << " -- " << this->sender << std::endl;
    std::cout << this->content << std::endl;
}
