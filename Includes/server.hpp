#ifndef SERVER_HPP
#define SERVER_HPP

#include "network.hpp"

class server : public network
{
    private:
        in_port_t port;
        in_addr_t ip_addres;

        void creat_socket();
        void bind_and_listen();
    public:
        server();
        server(in_port_t port, in_addr_t ip_addres, int id);
        void onEvent();
};

#endif