#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "lib.hpp"
#include <cstdio>
#include <sys/socket.h>
class network
{
    protected:
        int socket_fd;
        bool is_server;
        sockaddr_in network_infos;
        int kernel_identifier;
        int event;
    public:
        epoll_event ev;

        network();
        network(int kernel_identifier, bool is_server);

        int get_socket_fd();

        void epoll_crt();
        void set_ToNoBlocking();

        sockaddr_in *get_sockaddr();
        bool if_server();
        void set_event(int ev);
        int get_event();

        virtual ~network();
        virtual void onEvent() = 0;
};

#endif