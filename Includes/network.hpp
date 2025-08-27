#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "lib.hpp"
#include "Configs.hpp"
class network
{
    protected:
        int                     _socket_fd;
        bool                    _is_server;
        sockaddr_in             _network_infos;
        const ServerConfigs     &_server_config;
        int                     event;
    public:
        epoll_event             ev;

        int                     get_socket_fd();
        void                    epoll_crt(int event_flags);
        void                    epoll_crt();
        void                    set_ToNoBlocking();
        sockaddr_in             *get_sockaddr();
        bool                    if_server();
        void                    set_event(int ev);
        int                     get_event();

        network(const ServerConfigs &server_config);
        network(const ServerConfigs &server_config , bool is_server);

        virtual ~network();
        virtual void onEvent() = 0;

        virtual bool            isCgi() const { return false; }

};

#endif