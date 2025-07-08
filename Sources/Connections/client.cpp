#include "client.hpp"
#include "WebServer.hpp"
client:: client(const ServerConfigs& server_config) : network(server_config, false)
{
    // is_server = false;
    request.state = 0;
}

// client:: client() : network(false)
// {
//     request.state = 0;
// }

void client:: epoll_modify()
{
    ev.events = EPOLLOUT | EPOLLRDHUP;
    ev.data.fd = socket_fd;
    if (epoll_ctl(WebServer:: kernel_identifier, EPOLL_CTL_MOD, socket_fd, &ev) < 0)
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
        std::cout << "in input" << std::endl;
        int is_finish = request.run_parser(socket_fd);
        if (is_finish)
            epoll_modify();
    }
    else if (event & EPOLLOUT)
    {
        std::cout << "send output" << std::endl;
        send(socket_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 21\r\n\r\nChunked data received!", 87, 0);
    }
}

client:: ~client()
{

}