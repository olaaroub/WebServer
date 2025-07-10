#include "client.hpp"
#include "ServerManager.hpp"
#include "Utils.hpp"

client::client(const ServerConfigs &server_config) : network(server_config, false) { request.state = 0; }

void client::epoll_modify()
{
    ev.events = EPOLLOUT | EPOLLRDHUP;
    ev.data.fd = socket_fd;
    if (epoll_ctl(serverManager::kernel_identifier, EPOLL_CTL_MOD, socket_fd, &ev) < 0)
        throw std::runtime_error("Client Error: epoll control failed!");
}

const LocationConfigs* client::findLocation(const std::string& uri)
{
    const LocationConfigs* bestMatch = NULL;
    size_t len = 0;

    const std::vector<LocationConfigs>& locations = this->server_config.locations;

    for (std::vector<LocationConfigs>::const_iterator it = locations.begin(); it != locations.end(); ++it)
    {
        if (uri.rfind(it->path, 0) == 0)
        {
            if (it->path.length() > len)
            {
                len = it->path.length();
                bestMatch = &(*it);
            }
        }
    }
     return bestMatch;
}

void client::onEvent() // handlehttprequest
{
    std::string red = "\033[31m";
    std::string reset = "\033[0m";

    if (event & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
        throw std::runtime_error("Client disconnected or socket error.");
    else if (event & EPOLLIN)
    {
       bool is_request_complete = request.run_parser(socket_fd);

            if (is_request_complete)
                epoll_modify();
    }
    else if (event & EPOLLOUT)
    {
        std::string fullPath;
        const std::string& requestUri = normalizePath(request.RequestLine.getUrl());

        const LocationConfigs* location = findLocation(requestUri);
        if(location)
            fullPath = joinPaths(location->root, requestUri);

        std::cout  << red << "--- full path : "<< reset << fullPath << std::endl;

        if (!location)
        {
            response res_error(socket_fd, "./Pages/Errors/404.html", "HTTP/1.1 404 Not Found");
            throw std::runtime_error("Response 404 sent!") ;
            
        }
        else if (std::find(location->allowed_methods.begin(), location->allowed_methods.end(), request.RequestLine.get_method()) == location->allowed_methods.end())
        {
            std::cerr << "Error: Method '" << request.RequestLine.get_method() << "' is not allowed for this location." << std::endl;
            response res_error(socket_fd, "./Pages/Errors/405.html", "HTTP/1.1 405 Method Not Allowed");
            throw std::runtime_error("Response 405 sent!") ;
             

        }

        // std::string extension  = "wa7d";
        // = getExtention(requestUri); .php .py .sh /// todo


        // if (location->cgi_handlers.count(extension))
        // {
        //     // CgiClass obj;

        //     // hna ankhdem cgi


            if (request.RequestLine.get_method() == "GET")
            {
                std::cout  << red << "----------- PART OF METHODS GET START --------------"  << reset <<  std::endl;
                // uri  | index_file  | root_path
                //  build the full path to the file .
                // check is this path exsist !
                struct stat file_info;

                std::cout << "the full path !!!  : " << fullPath << std::endl;

                if (stat(fullPath.c_str(), &file_info) != 0)
                {
                    std::cout << "error , the file not found !" << std::endl;

                    response res_error(socket_fd, "./Pages/Errors/404.html", "HTTP/1.1 404 Not Found");
                        throw std::runtime_error("Response 404 sent!");
                         

                }
                bool is_a_file = S_ISREG(file_info.st_mode);
                bool is_a_dir = S_ISDIR(file_info.st_mode);

                if (is_a_file == true)
                {
                    // check  permession !
                    if ((file_info.st_mode & S_IRUSR) == 0)
                    {
                        std::cout << "error, 403 Forbiden ! (there is no permession for read)" << std::endl;
                        response res_error(socket_fd, "./Pages/Errors/403.html", "HTTP/1.1 403 Forbidden");
                            throw std::runtime_error("Response 403 sent!");
                             


                    }
                    else
                    {
                        // sending the response

                        std::cout << "the response send successfully !" << std::endl;
                        response res_success(socket_fd, fullPath , "HTTP/1.1 200 OK\r\n");
                            throw std::runtime_error("Response  sent successfully!");
                             




                    }
                }
                else if (is_a_dir == true)
                {
                    // check  permession !
                    if ((file_info.st_mode & S_IXUSR) == 0)
                    {
                        std::cout << "error, 403 Forbiden ! (there is no permession for execute the dir)" << std::endl;
                        response res_error(socket_fd, "./Pages/Errors/403.html", "HTTP/1.1 403 Forbidden");
                            throw std::runtime_error("Response 403 sent!");
                             


                    }
                    else
                    {
                        std::cout << "it is a dir , need a response" << std::endl;


                        std::string default_path = fullPath + location->index_file;
                        std::cout << "default path : " << default_path << std::endl;

                        response res_success(socket_fd, default_path , "HTTP/1.1 200 OK\r\n");
                            throw std::runtime_error("Response sent successfully!");
                             



                    }

                }


            }
    }
    
}


client::~client()
{
}