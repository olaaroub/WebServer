#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "lib.hpp"
#include "Configs.hpp"
class network
{
    protected:
        int socket_fd;
        bool is_server;
        sockaddr_in network_infos;
        const ServerConfigs &server_config;
        int event;
    public:
        epoll_event ev;
        network(const ServerConfigs &server_config);
        network(const ServerConfigs &server_config , bool is_server);

        int get_socket_fd();

        void epoll_crt(int event_flags);
        void epoll_crt();
        void set_ToNoBlocking();

        sockaddr_in *get_sockaddr();
        bool if_server();
        void set_event(int ev);
        int get_event();

        virtual ~network();
        virtual void onEvent() = 0;

        virtual bool isCgi() const { return false; }

};

#endif