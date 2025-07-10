#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "server.hpp"
#include "network.hpp"
#include "ConfigFileParser.hpp"

class serverManager
{
    private:
        static struct epoll_event evlist;

        static void add_server(network *instance);
        static void listening();
        static void epollEvent(int fd, int event);
        serverManager();
    public:

        static std::map<int, network *> activeNetworks;
        static int kernel_identifier;

        static void setupServers(const std::vector<ServerConfigs>& servers);
        static void startServers();
};


#endif