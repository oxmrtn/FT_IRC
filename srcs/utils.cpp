#include "../includes/includes.hpp"

std::string to_lowercase(const std::string &str)
{
    std::string low;
    low.reserve(str.length());

    for (size_t i = 0; i < str.length(); i++)
        low += std::tolower(str[i]);
    
    return low;
}

bool    ends_with(const std::string &str, const std::string &suffix)
{
    if (str.length() < suffix.length())
        return false;
    return !str.compare(str.length() - suffix.length(), suffix.length(), suffix);
}

int err_ret(const std::string msg)
{
    std::cerr << "error: " << msg << std::endl;
    return (1);
}

int is_zero(const std::string str)
{
    for (size_t i = 0; i < str.length(); i++)
        if (str[i] != '0')
            return (0);

    return (1);
}
std::string trim_spaces(const std::string &str)
{
    if (str.empty())
        return str;
    
    size_t start = 0;
    while (start < str.length() && str[start] == ' ')
        start++;

    if (start == str.length())
        return "";
    
    size_t end = str.length() - 1;
    while (end > start && str[end] == ' ')
        end--;
    
    return str.substr(start, end - start + 1);
}

std::string deduplicate_spaces(const std::string &str)
{
    if (str.empty())
        return str;
    
    std::string cleaned;
    cleaned.reserve(str.length());
    
    bool    prev_white = false;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == ' ')
        {
            if (!prev_white)
            {
                cleaned += ' ';
                prev_white = true;
            }
        }
        else
        {
            cleaned += str[i];
            prev_white = false;
        }
    }
    
    return cleaned;
}

std::string clean_spaces(const std::string &str)
{
    std::string cleaned = deduplicate_spaces(str);
    cleaned = trim_spaces(cleaned);

    return cleaned;
}

std::pair<std::string, std::string>
    split_first(const std::string &str, char delimiter)
{
    size_t  pos = str.find(delimiter);
    
    if (pos == std::string::npos)
        return std::make_pair(str, "");
    
    return std::make_pair(str.substr(0, pos), str.substr(pos + 1));
}

