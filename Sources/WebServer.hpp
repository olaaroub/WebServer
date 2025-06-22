#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "server.hpp"
#define MAX_EPOLL 100

class WebServer
{
    private:
        static std::map<int, network *> infos;
        static struct epoll_event *evlist;

        static void add_server(network *instance);
        static void setup_servers();
        static void lisning();
        static void epollEvent(int fd, int event);
        WebServer();
    public:

        static int kernel_identifier;
        static void run_webserver(std::string file_name);
};


#endif