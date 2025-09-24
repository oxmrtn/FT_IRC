#include "../includes/server.hpp"
#include "../includes/user.hpp"

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

int args_check(int ac, char **av)
{
    if (ac != 3)
        return (err_ret("invalid arguments: expecting: <port> <password>"));
    
    if (!is_zero(av[1]))
    {
        long    port = std::atol(av[1]);
        
        if (port < 1 || port > 65535)
            return (err_ret("invalid arguments: <port> is expecting an integer in range 0-65535"));
    }
    
    return (0);
}

int main(int ac, char **av)
{
    if (args_check(ac, av))
        return (1);

    int port = std::atoi(av[1]);

    try
    {
        Server  server = Server(port, av[2]);
        server._run();
    }
    catch(const std::exception& e)
    {
        err_ret(e.what());
    }

    return (0);
}