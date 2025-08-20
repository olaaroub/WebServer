#include "server.hpp"
#include "client.hpp"
#include "ServerManager.hpp"

server:: server(const ServerConfigs &server_config) : network(server_config)
{
    is_server = true;
}

server::server(in_port_t port, in_addr_t ip_addres,const ServerConfigs &server_config) : network(server_config, true),port(port), ip_addres(ip_addres)
{
    creat_socket();
    bind_and_listen();
    epoll_crt();
}

void server:: bind_and_listen()
{
    int yes = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("Server Error: ");
        throw std::runtime_error("!");
    }
    if (bind(socket_fd, (sockaddr *)&network_infos, sizeof(network_infos)) < 0)
    {
        perror("Server Error: ");
        throw std::runtime_error("!");
    }
    set_ToNoBlocking();
    if (listen(socket_fd, SOMAXCONN) < 0)
    {
        perror("Listen Error: ");
        throw std::runtime_error("!");
    }
}

void server:: creat_socket()
{
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        std::runtime_error("Server Error: Socket failed!");
    network_infos.sin_family = AF_INET;
    network_infos.sin_addr.s_addr = this->ip_addres;
    network_infos.sin_port = htons(this->port);
    memset(network_infos.sin_zero, 0, sizeof(network_infos.sin_zero));
}

void server:: onEvent()
{
    sockaddr_in *clien_struct;

    if (event & EPOLLIN)
    {
        client *client_re = new client(server_config);
        clien_struct = client_re->get_sockaddr();
        socklen_t addr_size = sizeof(clien_struct);
        int fd = accept(socket_fd, (sockaddr *)clien_struct, &addr_size);
        if (fd < 0)
        {
            delete client_re;
            return ;
        }
        std::cout << GREEN << "[FD: " << fd << "] New connection accepted." << RESET << std::endl;
        client_re->set_fd(fd);
        client_re->set_ToNoBlocking();
        client_re->epoll_crt();
        client_re->set_time(time(NULL));
        serverManager::activeNetworks[fd] = client_re;
    }
}