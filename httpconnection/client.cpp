#include "client.hpp"

client:: client() : network()
{
    is_server = false;
}

client:: client(int kernel_id) : network(kernel_id, false)
{

}

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
    if (event & (EPOLLERR | EPOLLHUP))
    {
        perror("ERROR: ");
        close(socket_fd);
        epoll_ctl(kernel_identifier , EPOLL_CTL_DEL, socket_fd, 0);
    }
    else if (event & EPOLLIN)
    {
        int is_finish = request.run_parser(socket_fd);
        if (is_finish)
            epoll_modify();
    }
    else if (event & EPOLLOUT)
    {
        send(socket_fd, "thanks client\n", 14, 0);
        epoll_ctl(kernel_identifier , EPOLL_CTL_DEL, socket_fd, 0);
        close(socket_fd);

    }
}