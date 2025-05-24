#include "server.hpp"
#include "client.hpp"
#include "../WebServer.hpp"
server:: server()
{
    is_server = true;
}

server::server(in_port_t port, in_addr_t ip_addres, int id) : network(id, true),port(port), ip_addres(ip_addres)
{
    creat_socket();
    bind_and_listen();
    epoll_crt();
}

void server:: bind_and_listen()
{
    if (bind(socket_fd, (sockaddr *)&network_infos, sizeof(network_infos)) < 0) 
    {
        perror("Bind: ");
        throw std::string("the program exit !");
    }
    set_ToNoBlocking();
    if (listen(socket_fd, 1) < 0) 
    {
        perror("Listen");
        throw std::string("the program exit !");
    }
}

void server:: creat_socket()
{
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        std::cout << "ERROR: fd error" << std::endl;
    network_infos.sin_family = AF_INET;
    network_infos.sin_addr.s_addr = this->ip_addres;
    network_infos.sin_port = htons(this->port);
    memset(network_infos.sin_zero, 0, sizeof(network_infos.sin_zero));
}

void server:: onEvent(std::map<int, network *> &infos)
{
    sockaddr_in *clien_struct;

    if (event & EPOLLIN)
    {
        client *client_re = new client(WebServer::kernel_identifier);
        clien_struct = client_re->get_sockaddr();
        socklen_t addr_size = sizeof(clien_struct);
        int fd = accept(socket_fd, (sockaddr *)clien_struct, &addr_size);
        client_re->set_fd(fd);
        client_re->set_ToNoBlocking();
        client_re->epoll_crt();
        infos[fd] = client_re;
    }
}