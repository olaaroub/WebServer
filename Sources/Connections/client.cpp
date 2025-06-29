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

void client:: onEvent()
{
    if (event & (EPOLLERR | EPOLLHUP))
        perror("ERROR: ");
    else if (event & EPOLLIN)
    {
        int is_finish = request.run_parser(socket_fd);
        if (is_finish)
            epoll_modify();
    }
    else if (event & EPOLLOUT)
        send(socket_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 21\r\n\r\nChunked data received!", 95, 0);
}

client:: ~client()
{

}