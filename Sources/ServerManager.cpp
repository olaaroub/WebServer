#include "ServerManager.hpp"
#include "CgiExecutor.hpp"
#include "client.hpp"

int                 		        serverManager::kernel_identifier = 0;
struct epoll_event  		        serverManager::evlist;
std::map<int, network *> 	        serverManager::activeNetworks;
std::map<std::string, SessionData>  serverManager::s_activeSessions;
const int 					        serverManager::request_timeout =  10;

void serverManager:: signal_handler(int)
{ throw std::runtime_error("signal catched"); }

void serverManager::add_server(network *instance)
{ activeNetworks[instance->get_socket_fd()] = instance; }

std::string serverManager::createSession(const std::string& username)
{
    std::stringstream ss;
    // int seed = time(0);
    // srand(seed);
    ss << time(0) << "-" << rand();
    // std::cout << ss.rdbuf()<<std::endl;

    std::string sessionId = ss.str();
    SessionData session;

    session.name = username;
    session.expiry_time = time(0) + 3600;

    s_activeSessions[sessionId] = session;
    return sessionId;
}

bool serverManager::validateSession(const std::string& sessionId)
{
    if (sessionId.empty())
        return false;
    std::map<std::string, SessionData>::iterator it = s_activeSessions.find(sessionId);
    if (it != s_activeSessions.end())
    {
        if (it->second.expiry_time > time(0))
            return true;
        else
            s_activeSessions.erase(it);
    }
    return false;
}

void serverManager::reapChildProcesses()
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    std::cout << "kill zombie process with PID " << pid << std::endl;
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
                executor->getClient()->handleHttpError(504);
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

void serverManager::listening()
{
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

        signal(SIGINT, signal_handler);

        for (std::map<int, network *>::iterator it = activeNetworks.begin(); it != activeNetworks.end(); )
        {
            if (!it->second->isCgi() && (it->second->if_server() == false && (current_time - it->second->get_time()) > request_timeout))
            {
                std::cout << "Client timeout, closing connection." << std::endl;
                client *deletClient = dynamic_cast<client *>(it->second);
                deletClient->handleHttpError(timeout);
                close(it->first);
                epoll_ctl(kernel_identifier, EPOLL_CTL_DEL, it->first, 0);
                delete it->second;
                std::map<int, network *>::iterator to_erase = it;
                ++it;
                activeNetworks.erase(to_erase);
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
                std::cout << red << "Host: " << (*it).host << ":" << *its << RES << std::endl;
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
        listening();
    }
    catch (std::exception &e)
    {
        std::cerr << green << "\n Server shutting down: " << e.what() << reset << std::endl;
        std::map<int, network *>::iterator it;
        for (it = activeNetworks.begin(); it != activeNetworks.end(); ++it)
            delete it->second;
        activeNetworks.clear();
        close(kernel_identifier);
    }
}