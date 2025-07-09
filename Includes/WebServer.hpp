#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "server.hpp"
#include "network.hpp"
#include "ConfigFileParser.hpp"

class WebServer
{
    private:
        static struct epoll_event *evlist;

        static void add_server(network *instance);
        static void setup_servers(const std::vector<ServerConfigs>& servers);
        static void listening();
        static void epollEvent(int fd, int event);
        WebServer();
    public:

        static std::map<int, network *> infos;
        static int kernel_identifier;
        static void run_webserver(const std::vector<ServerConfigs>& servers);
};


#endif