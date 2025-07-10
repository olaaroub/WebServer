#include "WebServer.hpp"

// std::vector<network *> WebServer::servers;
int WebServer:: kernel_identifier = 0;
struct epoll_event WebServer:: evlist;
std::map<int, network *> WebServer:: infos;

void WebServer:: add_server(network *instance)
{
    infos[instance->get_socket_fd()] = instance;
}

// void WebServer:: epollEvent(int fd, int event)
// {
//     try
//     {
//         infos[fd]->set_event(event);
//         infos[fd]->onEvent();
//         if (event & (EPOLLERR | EPOLLHUP) || (event & EPOLLOUT))
//         {
//             epoll_ctl(kernel_identifier , EPOLL_CTL_DEL, fd, 0);
//             delete infos[fd];
//             infos.erase(fd);
//             close(fd);
//         }
//     }
//     catch(std::string error)
//     {
//         std::cout << error << std::endl;
//     }


// } // hadi tadir leaks ila sefet lia data w closa k connetion dghia. EPOLLRDHUP makantch tatcatcha
// wmakntich tatcleani ghir tatprinti l error


void WebServer::epollEvent(int fd, int event)
{
    try
    {
        if (infos.find(fd) == infos.end()) { return; } // l client deja tmse7
        infos[fd]->set_event(event);
        infos[fd]->onEvent();
    }
    catch(const std::exception& e)
    {
        std::cerr << "cleaning fd " << fd << " hitach: " << e.what() << std::endl;

        epoll_ctl(kernel_identifier, EPOLL_CTL_DEL, fd, 0);

        if (infos.count(fd))
        {
            delete infos[fd];
            infos.erase(fd);
        }
        close(fd);
    }
}


void WebServer:: listening()
{
    epoll_event evlist[infos.size()];

    while(true)
    {
        int event = epoll_wait(kernel_identifier, evlist, infos.size(), -1);
        if (event < 0)
        {
            perror("epoll_wait");
            throw std::runtime_error("");
        }
        for (int i = 0; i < event; i++)
        {
            int fd = evlist[i].data.fd;
            epollEvent(fd, evlist[i].events);

        }
    }
}


void WebServer:: setup_servers(const std::vector<ServerConfigs>& servers)
{
    kernel_identifier = epoll_create(MAX_EPOLL);
    for (std::vector<ServerConfigs>::const_iterator it = servers.begin(); it != servers.end(); it++)
    {
        for (std::vector<int>::const_iterator its = it->ports.begin(); its != it->ports.end(); its++)
        {
            try
            {
                server *new_server = new server((*its), inet_addr((*it).host.c_str()), (*it));
                add_server(new_server);
            }
            catch(std::exception &e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
    }

}


void WebServer:: run_webserver(const std::vector<ServerConfigs> &servers)
{
    try
    {
        std::cout << "setup servers\n";
        setup_servers(servers);
        std::cout << "listening ...\n";
        listening();
    }
    catch(std::exception &e)
    {
        //  matnsach tfriyi lmap
        std::cerr << e.what() << std::endl;
    }

}