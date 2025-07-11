#ifndef SERVER_HPP
#define SERVER_HPP

#include "network.hpp"
#include "lib.hpp"

// class network;
class server : public network
{
    private:
        in_port_t port;
        in_addr_t ip_addres;

        void creat_socket();
        void bind_and_listen();
    public:
        server(const ServerConfigs &server_config);
        server(in_port_t port, in_addr_t ip_addres,const ServerConfigs &server_config);
        void onEvent();

};

#endif