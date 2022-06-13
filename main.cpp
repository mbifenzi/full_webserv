
# include "Utils.h"
# include "config/http_server.hpp"

int main(int ac, char **av)
{
    std::ifstream config;
    http_server _HTTP_server;

    (void)ac;
    config.open(av[1], std::ifstream::in);
    std::cout << "\033[97m";
    try{
        _HTTP_server.parse(config);
    }
    catch(std::string &s)
    {
        config.close();
        std::cout << s << std::endl;
        return (0);
    }
    signal(SIGPIPE, SIG_IGN);
    _HTTP_server.startServers();
    return 0;
}