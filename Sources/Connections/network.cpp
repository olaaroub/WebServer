#include "network.hpp"
#include "WebServer.hpp"

int network:: get_socket_fd()
{
    return socket_fd;
}

void network:: set_ToNoBlocking()
{
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags < 0)
        throw std::string("Failed to get socket flags");
    if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1) 
        throw std::string("Failed to set non-blocking mode\n");
}

network:: network(const ServerConfigs &server_config) : server_config(server_config)
{

}

void network:: epoll_crt()
{
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    if (epoll_ctl(WebServer::kernel_identifier, EPOLL_CTL_ADD, socket_fd, &ev) < 0)
    {
        perror("epoll_ctl");
        throw std::string("");
    }
}

network:: network(const ServerConfigs &server_config , bool is_server) : is_server(is_server), server_config(server_config)
{

}

network:: ~network()
{

}

sockaddr_in *network:: get_sockaddr()
{
    return &network_infos;
}
bool network:: if_server()
{
    return is_server;
}
void network:: set_event(int ev)
{
    event = ev;
}
int network:: get_event()
{
    return event;
}