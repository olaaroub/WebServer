#include "ServerManager.hpp"

// std::vector<network *> serverManager::servers;
int serverManager:: kernel_identifier = 0;
struct epoll_event serverManager:: evlist;
std::map<int, network *> serverManager:: activeNetworks;

void serverManager:: add_server(network *instance)
{
    activeNetworks[instance->get_socket_fd()] = instance;
}

// void serverManager:: epollEvent(int fd, int event)
// {
//     try
//     {
//         activeNetworks[fd]->set_event(event);
//         activeNetworks[fd]->onEvent();
//         if (event & (EPOLLERR | EPOLLHUP) || (event & EPOLLOUT))
//         {
//             epoll_ctl(kernel_identifier , EPOLL_CTL_DEL, fd, 0);
//             delete activeNetworks[fd];
//             activeNetworks.erase(fd);
//             close(fd);
//         }
//     }
//     catch(std::string error)
//     {
//         std::cout << error << std::endl;
//     }


// } // hadi tadir leaks ila sefet lia data w closa k connetion dghia. EPOLLRDHUP makantch tatcatcha
// wmakntich tatcleani ghir tatprinti l error


void serverManager::epollEvent(int fd, int event)
{
    try
    {
        if (activeNetworks.find(fd) == activeNetworks.end()) { return; } // l client deja tmse7
        activeNetworks[fd]->set_event(event);
        activeNetworks[fd]->onEvent();
    }
    catch(const std::exception& e)
    {
        std::cerr << "cleaning fd " << fd << " hitach: " << e.what() << std::endl;

        epoll_ctl(kernel_identifier, EPOLL_CTL_DEL, fd, 0);

        if (activeNetworks.count(fd))
        {
            std::cout << "delet Connection" << std::endl;
            delete activeNetworks[fd];
            activeNetworks.erase(fd);
        }
        close(fd);
    }
}


void serverManager:: listening()
{
    std::cout << "--evlist lenghet" << activeNetworks.size() << std::endl;
    std::vector<epoll_event> evlist(1024);
    while(true)
    {
        int event = epoll_wait(kernel_identifier, evlist.data(), evlist.size(), -1);
        if (event < 0)
        {
            perror("Epoll Error: ");
            throw std::runtime_error("!");
        }
        for (int i = 0; i < event; i++)
        {
            int fd = evlist[i].data.fd;
            epollEvent(fd, evlist[i].events);
            if (activeNetworks.size() >= evlist.size())
                evlist.resize(activeNetworks.size() * 2);
            // std::cout << "|____Servers Cont" << activeNetworks.size() << std::endl;
        }
    }
}


void serverManager:: setupServers(const std::vector<ServerConfigs>& servers)
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


void serverManager:: startServers()
{
    try
    {
        std::cout << "listening ...\n";
        listening();
    }
    catch(std::exception &e)
    {
        //  matnsach tfriyi lmap
        std::cerr << e.what() << std::endl;
    }

}