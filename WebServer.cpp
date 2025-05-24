#include "WebServer.hpp"

// std::vector<network *> WebServer::servers;
int WebServer:: kernel_identifier = 0;
struct epoll_event *WebServer:: evlist;
std::map<int, network *> WebServer:: infos;

WebServer:: WebServer() {}

void WebServer:: add_server(network *instance)
{
    infos[instance->get_socket_fd()] = instance;
}

void WebServer::pars_config_file(std::string file_name)
{
    /*
        here parsing
    */
}

void WebServer:: epollEvent(int fd, int event)
{
    infos[fd]->set_event(event);
    infos[fd]->onEvent(infos);
}

void WebServer:: lisning()
{
    evlist = new epoll_event[infos.size()];

    while(true)
    {
        int event = epoll_wait(kernel_identifier, evlist, 1, -1);
        if (event < 0)
        {
            perror("epoll_wait");
            throw std::string("");
        }
        for (int i = 0; i < event; i++)
        {
            int fd = evlist[i].data.fd;
            epollEvent(fd, evlist[i].events);
            
        }
    }
}


void WebServer:: setup_servers()
{
    kernel_identifier = epoll_create(MAX_EPOLL);
    try
    {
        server *instance = new server(9999, inet_addr("127.0.0.1"), kernel_identifier);
        add_server(instance);

    }
    catch(std::string error)
    {
        std::cerr << error << std::endl; 
    }
    try
    {
        server *instance1 = new server(9991, inet_addr("127.0.0.2"), kernel_identifier);
        add_server(instance1);

    }
    catch(std::string error)
    {
        std::cerr << error << std::endl; 
    }
    try
    {
        server *instance2 = new server(9992, inet_addr("127.0.0.3"), kernel_identifier);
        add_server(instance2);

    }
    catch(std::string error)
    {
        std::cerr << error << std::endl; 
    }

}


void WebServer:: run_webserver()
{
    try
    {
        std::cout << "setup servers\n";
        setup_servers();
        std::cout << "lisning ...\n";
        lisning();
    }
    catch(std::string error)
    {
        std::cerr << error << std::endl; 
    }
    
}