#include "../includes/server.hpp"
#include "../includes/user.hpp"

int g_sig = 1;

int args_check(int ac, char **av)
{
    if (ac != 3)
        return (err_ret("invalid arguments: expecting: <port> <password>"));
    
    long    port = std::atol(av[1]);

    if (!port || port < 1023 || port > 65535)
        return (err_ret("invalid arguments: <port> is expecting an integer in range 1023-65535"));
    return (0);
}

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        std::cerr << std::endl;
        g_sig = 0;
    }
}

int main(int ac, char **av)
{
    if (args_check(ac, av))
        return (1);

    struct sigaction sa;
    sa.sa_handler = sig_handler;
    memset(&sa.sa_mask, 0, sizeof(sa.sa_mask));
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    int port = std::atoi(av[1]);

    try
    {
        Server server = Server(port, av[2]);
        server._run();
    }
    catch(const std::exception& e)
    {
        err_ret(e.what());
    }

    std::cerr << "Server stopped" << std::endl;
    return (0);
}