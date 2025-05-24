#include "client.hpp"

client:: client() : network() {is_server = false;}

client:: client(int kernel_id) : network(kernel_id, false){}

void client:: epoll_modify()
{
    ev.events = EPOLLOUT;
    ev.data.fd = socket_fd;
    if (epoll_ctl(kernel_identifier, EPOLL_CTL_MOD, socket_fd, &ev) < 0)
    {
        perror("epoll_modify");
        throw std::string("");
    }
}

void client:: onEvent(std::map<int, network *> &infos)
{
    if (event & EPOLLIN)
    {
        // int cont = recv(socket_fd, &request_str, 1023, 0);
        // if (cont < 0)
        //     std::cout << "read filed" << std::endl;
        // request_str[cont] = '\0';
        // std::cout << request_str << std::endl;
        request.run_parser(socket_fd);
        epoll_modify();
    }
    else if (event & EPOLLOUT)
    {
        send(socket_fd, "thanks client\n", 14, 0);
        close(socket_fd);

    }
}