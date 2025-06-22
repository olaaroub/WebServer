#include "WebServer.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cout << "you shold entr only 2 args" << std::endl;
        return -1;
    }
    WebServer::run_webserver();

}