#pragma once

#include <string>
#include <vector>
#include "user.hpp"

class Message
{
    private:
        Message(void);
        std::string                 _prefix;
        std::string                 _command;
        std::vector<std::string>    _params;
        std::string                 _trailing;
        // METHODS
        void                        _default_init(void);
        void                        _parse(const std::string &raw);

    public:
        Message(const std::string &msg);
        Message(const std::string prefix,
                const std::string command,
                const std::vector<std::string> params,
                const std::string trailing);
        Message(const Message &src);
        Message &operator=(const Message &src);
        ~Message();
        // METHODS
        void                            _send(User &dest);
        // GETTERS
        std::string                     _compose(void) const;
        const std::vector<std::string>  &_get_params(void) const;
        const std::string               &_get_prefix(void) const;
        const std::string               &_get_command(void) const;
        const std::string               &_get_trailing(void) const;
};
