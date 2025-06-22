#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "fcntl.h"
int main() 
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        std::cout << "ERROR: fd error" << std::endl;

    sockaddr_in info;
    sockaddr_in clien_struct;
    socklen_t addr_size = sizeof(clien_struct);
    char bufr[1024]; 
    info.sin_family = AF_INET;
    info.sin_addr.s_addr = INADDR_ANY;
    info.sin_port = htons(9999);
    memset(info.sin_zero, 0, sizeof(info.sin_zero));
    // bind(socket_fd, (sockaddr *)&info, sizeof(info));
    if (bind(socket_fd, (sockaddr *)&info, sizeof(info)) < 0) 
    {
        perror("Bind: ");
        return 1;
    }
    listen(socket_fd, 1);
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Failed to get socket flags\n";
        return 1;
    }
    if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1) 
    {
        std::cerr << "Failed to set non-blocking mode\n";
        return 1;
    }
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    int kernel_identifier = epoll_create(10);
    if (kernel_identifier < 0)
    {
        perror("epoll_create");
        return 1;
    }
    if (epoll_ctl(kernel_identifier, EPOLL_CTL_ADD, socket_fd, &ev) < 0) {
        perror("epoll_ctl");
        return 1;
    }

    struct epoll_event *evlist = new epoll_event[1];
    while (true)
    {
        std::cout << "server listen ..." << std::endl;
        // set file discriptor to non-bloking mode
        std::cout << "server catch a requst ..." << std::endl;
        int ready_to_connection = epoll_wait(kernel_identifier, evlist, 1, -1);
        if (ready_to_connection < 0)
        {
            perror("accept");
            continue;
        }
        if (evlist[0].data.fd == socket_fd)
        {
            if (evlist[0].events & EPOLLIN)
                std::cout << "EPOLLIN" << std::endl;
            else if (evlist[0].events & EPOLLOUT)
                std::cout << "EPOLLOUT" << std::endl;
            int clien_fd = accept(socket_fd, (sockaddr *)&clien_struct, &addr_size);
            std::cout << "server accept the connection ..." << std::endl;

            int cont = recv(clien_fd, &bufr, 1023, 0);
            if (cont < 0)
                std::cout << "read filed" << std::endl;
            bufr[cont] = '\0';
            std::cout << bufr << std::endl;
            send(clien_fd, "thanks client\n", 14, 0);
            close(clien_fd);
        }
    }
    close(socket_fd);
}