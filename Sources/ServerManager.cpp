#include "ServerManager.hpp"
#include "CgiExecutor.hpp"
#include "client.hpp"

// std::vector<network *> serverManager::servers;
int serverManager::kernel_identifier = 0;
struct epoll_event serverManager::evlist;
std::map<int, network *> serverManager::activeNetworks;
std::map<int, CgiExecutor *> serverManager::activeCgi;

const int serverManager:: request_timeout =  10; // seconds

void serverManager::add_server(network *instance)
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
        if (activeNetworks.find(fd) == activeNetworks.end())
            return;
        activeNetworks[fd]->set_event(event);
        activeNetworks[fd]->onEvent();
    }
    catch (const std::exception &e)
    {
        std::cerr << "cleaning fd " << fd << " 7itach: " << e.what() << std::endl;
        bool is_cgi = false;
        if (activeNetworks.count(fd))
            is_cgi = activeNetworks[fd]->isCgi();

        epoll_ctl(kernel_identifier, EPOLL_CTL_DEL, fd, 0);

        if (activeNetworks.count(fd))
        {
            delete activeNetworks[fd];
            activeNetworks.erase(fd);
        }

        if (!is_cgi)
            close(fd);
    }
}

void serverManager::reapChildProcesses()
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        std::cout << "kill zombie process with PID " << pid << std::endl;
    }
}

void serverManager::checkCgiTimeouts()
{
    const int CGI_TIMEOUT_SECONDS = 10;
    std::map<int, network *>::iterator it;

    for (it = activeNetworks.begin(); it != activeNetworks.end();)
    {
        if (it->second && it->second->isCgi())
        {
            CgiExecutor *executor = static_cast<CgiExecutor *>(it->second);
            bool should_delete = false;

            if (executor->getState() == CgiExecutor::CGI_DONE || executor->getState() == CgiExecutor::CGI_ERROR)
                should_delete = true;
            else if (time(NULL) - executor->getStartTime() > CGI_TIMEOUT_SECONDS)
            {
                std::cerr << red << "CGI script with PID " << executor->getPid() << " timed out. Terminating." << reset << std::endl;
                kill(executor->getPid(), SIGKILL);
                // waitpid(executor->getPid(), NULL, 0);
                executor->getClient()->sendErrorResponse(504, "Gateway Timeout");
                should_delete = true;
            }

            if (should_delete)
            {
                delete executor;
                activeNetworks.erase(it++);
            }
            else
                ++it;
        }
        else
            ++it;
    }
}

void serverManager::listening()
{
    std::cout << "--evlist lenghet" << activeNetworks.size() << std::endl;
    std::vector<epoll_event> evlist(1024);
    while (true)
    {
        int event = epoll_wait(kernel_identifier, evlist.data(), evlist.size(), 1000);

        time_t current_time = time(NULL);
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
        }

        checkCgiTimeouts();
        reapChildProcesses();

        for (std::map<int, network *>::iterator it = activeNetworks.begin(); it != activeNetworks.end(); )
        {
            if (it->second->if_server() == false && (current_time - it->second->get_time()) > request_timeout)
            {
                std::cout << "Client timeout, closing connection." << std::endl;
                close(it->first);
                epoll_ctl(kernel_identifier, EPOLL_CTL_DEL, it->first, 0);
                delete it->second;
                std::map<int, network *>::iterator to_erase = it;
                ++it;
                activeNetworks.erase(to_erase);
                // khasni nsift response 408 Request Timeout
            }
            else
                ++it;
        }
    }
}

void serverManager::setupServers(const std::vector<ServerConfigs> &servers)
{
    kernel_identifier = epoll_create(1024);
    for (std::vector<ServerConfigs>::const_iterator it = servers.begin(); it != servers.end(); it++)
    {
        for (std::vector<int>::const_iterator its = it->ports.begin(); its != it->ports.end(); its++)
        {
            try
            {
                server *new_server = new server((*its), inet_addr((*it).host.c_str()), (*it));
                add_server(new_server);
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
    }
}

void serverManager::startServers()
{
    try
    {
        std::cout << "listening ...\n";
        listening();
    }
    // catch(std::exception &e)
    // {
    //     //  matnsach tfriyi lmap
    //     std::cerr << e.what() << std::endl;
    // }
    catch (std::exception &e)
    {
        std::cerr << "Server shutting down: " << e.what() << std::endl;
        // Cleanup map
        for (std::map<int, network *>::iterator it = activeNetworks.begin(); it != activeNetworks.end(); ++it)
        {
            delete it->second;
        }
        activeNetworks.clear();
        close(kernel_identifier);

    }
}