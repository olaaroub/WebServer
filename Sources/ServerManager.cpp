#include "ServerManager.hpp"
#include "CgiExecutor.hpp"
#include "client.hpp"

int                 		        serverManager::kernel_identifier = 0;
struct epoll_event  		        serverManager::evlist;
std::map<int, network *> 	        serverManager::activeNetworks;
std::map<std::string, SessionData>  serverManager::s_activeSessions;
const int 					        serverManager::request_timeout =  60;
const std::string                   serverManager::s_sessionFilePath = "sessions.db";
bool isShutdown = false;
void serverManager:: signal_handler(int)
{
    isShutdown = true;
}

void serverManager::add_server(network *instance)
{ activeNetworks[instance->get_socket_fd()] = instance; }

std::string serverManager::createSession(const std::string& username)
{
    std::stringstream ss;

    ss << time(0) << "-" << rand();

    std::string sessionId = ss.str();
    SessionData session;

    session.name = username;
    session.expiry_time = time(0) + 3600;

    s_activeSessions[sessionId] = session;
    std::cerr << MAGENTA << "[SESSION] Created session for user '"
        << username << "': " << sessionId << RESET << std::endl;
    saveSessionsToFile();
    return sessionId;
}

void serverManager::deleteSession(const std::string& sessionId)
{
    if (sessionId.empty())
        return;
    s_activeSessions.erase(sessionId);
    saveSessionsToFile();
    std::cout << MAGENTA << "[SESSION] Deleted session: " << sessionId << RESET << std::endl;
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
        {
            s_activeSessions.erase(it);
            std::cout << YELLOW << "[SESSION] Expired session removed: " << sessionId << RESET << std::endl;
        }
    }
    return false;
}


void serverManager::loadSessionsFromFile()
{
    std::ifstream sessionFile(s_sessionFilePath.c_str());
    if (!sessionFile.is_open()) {
        std::cout << "[SESSION] No session file found." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(sessionFile, line))
    {
        std::stringstream ss(line);
        std::string sessionId, username;
        time_t expiry_time;

        if (ss >> sessionId >> username >> expiry_time) {
            if (expiry_time > time(0))
            {
                SessionData session;
                session.name = username;
                session.expiry_time = expiry_time;
                s_activeSessions[sessionId] = session;
            }
        }
    }
    std::cout << "[SESSION] Loaded " << s_activeSessions.size() << " active sessions from file." << std::endl;
}

void serverManager::saveSessionsToFile()
{
    std::ofstream sessionFile(s_sessionFilePath.c_str());
    if (!sessionFile.is_open()) {
        std::cerr << "Error: Could not open session file for writing!" << std::endl;
        return;
    }

    for (std::map<std::string, SessionData>::const_iterator it = s_activeSessions.begin(); it != s_activeSessions.end(); ++it) {
        sessionFile << it->first << " " << it->second.name << " " << it->second.expiry_time << std::endl;
    }
}

void serverManager::reapChildProcesses()
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0){

        std::cout << YELLOW << "[SERVER] Reaped zombie CGI process with PID " << pid << RESET << std::endl;
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
                std::cout << YELLOW << "[CGI] Timeout for PID " << executor->getPid() << ". Terminating." << RESET << std::endl;
                kill(executor->getPid(), SIGKILL);
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
    catch (const ResponseSentException &e)
    {
        std::cout << GREEN << "[FD: " << fd << "] Closing connection 7itach " << e.what() << RESET << std::endl;

        epoll_ctl(kernel_identifier, EPOLL_CTL_DEL, fd, 0);
        if (activeNetworks.count(fd))
        {
            delete activeNetworks[fd];
            activeNetworks.erase(fd);
        }
        close(fd);
    }
    catch (const std::exception &e)
    {
        std::cout << RED << "[FD: " << fd << "] Client disconnected or error: " << e.what() << RESET << std::endl;
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
    std::cout << CYAN << "[SERVER] Now listening for connections..." << RESET << std::endl;
    while (true)
    {
        int event = epoll_wait(kernel_identifier, evlist.data(), evlist.size(), 1000);

        time_t current_time = time(NULL);
        if (isShutdown)
            throw std::runtime_error("signal catched");
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
        signal(SIGTERM, signal_handler);
        signal(SIGPIPE, SIG_IGN);
        // for (std::map<int, network *>::iterator it = activeNetworks.begin(); it != activeNetworks.end(); )
        // {
        //     client *Client = dynamic_cast<client *>(it->second);
        //     if (Client != NULL && !Client->is_request_complete && (current_time - Client->get_time()) > request_timeout)
        //     {
        //         std::cout << RED << "[FD: " << it->first << "] Client timed out. Closing connection." << RESET << std::endl;
        //         Client->handleHttpError(timeout);
        //         close(it->first);
        //         epoll_ctl(kernel_identifier, EPOLL_CTL_DEL, it->first, 0);
        //         delete it->second;
        //         std::map<int, network *>::iterator to_erase = it;
        //         ++it;
        //         activeNetworks.erase(to_erase);
        //     }
        //     else
        //         ++it;
        // }

        for (std::map<int, network *>::iterator it = activeNetworks.begin(); it != activeNetworks.end(); )
        {
            if (!it->second->if_server())
            {
                client *c = dynamic_cast<client *>(it->second);

                if (c && c->isMonitored() && !c->is_request_complete && (current_time - c->get_time()) > request_timeout)
                {
                    std::cout << YELLOW << "[FD: " << it->first << "] Client timed out. Closing connection." << RESET << std::endl;
                    c->handleHttpError(timeout);


                    epoll_ctl(kernel_identifier, EPOLL_CTL_DEL, it->first, 0);
                    close(it->first);
                    delete it->second;
                    activeNetworks.erase(it++);
                    continue;
                }
            }
            ++it;
        }
    }
}

void serverManager::setupServers(const std::vector<ServerConfigs> &servers)
{
    loadSessionsFromFile();
    kernel_identifier = epoll_create(1024);
    for (std::vector<ServerConfigs>::const_iterator it = servers.begin(); it != servers.end(); it++)
    {
        for (std::vector<int>::const_iterator its = it->ports.begin(); its != it->ports.end(); its++)
        {
            try
            {
                server *new_server = new server((*its), inet_addr((*it).host.c_str()), (*it));
                std::cout << CYAN << "[SERVER] Listening on " << (*it).host << ":" << *its << RESET << std::endl;
                add_server(new_server);
            }
            catch (ParseError &e)
            {
                if (e.ErrorStute > 0)
                    close(e.ErrorStute);
                std::cerr << RED << e.what() << RESET;
            }
        }
    }
}

void serverManager::startServers()
{
    try
    {
        if (!activeNetworks.size())
            throw std::runtime_error("No active networks");
        listening();
    }
    catch (std::exception &e)
    {
        std::cerr << YELLOW << "\nServer shutting down: " << e.what() << RESET << std::endl;
        std::map<int, network *>::iterator it;
        for (it = activeNetworks.begin(); it != activeNetworks.end(); ++it)
        {
            close(it->first);
            delete it->second;
        }
        activeNetworks.clear();
        close(kernel_identifier);
    }
}