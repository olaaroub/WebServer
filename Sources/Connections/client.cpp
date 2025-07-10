#include "client.hpp"
#include "ServerManager.hpp"
#include "Utils.hpp"


//


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
    if (event & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
        throw std::runtime_error("Client disconnected or socket error.");// bach process i cleani, perror does not clean
    else if (event & EPOLLIN)
    {
        try
        {
            bool is_request_complete = request.run_parser(socket_fd);

            if (is_request_complete)
            {
                // std::cout << "parsing request from fd: " << socket_fd << std::endl;

                const std::string& requestUri = normalizePath(request.RequestLine.getUrl());


                const LocationConfigs* location = findLocation(requestUri);// this finds exact match
                if(!location)
                    // location = findBestMatchLocation(requestUri);

                if (location)
                {
                    std::cout << "location block is '" << location->path << "'" << std::endl;

                    std::string fullPath = joinPaths(location->root, requestUri);

                    std::cout << "full path is '" << fullPath << "'" << std::endl;

                    std::string extension = "";
                    size_t dotPos = fullPath.rfind('.');
                    if (dotPos != std::string::npos) {
                        extension = fullPath.substr(dotPos);
                    }

                    if (location->cgi_handlers.count(extension))
                    {
                        std::string method = request.RequestLine.get_method();

                        const std::vector<std::string>& allowed = location->allowed_methods;

                        if (std::find(allowed.begin(), allowed.end(), method) != allowed.end())
                        {
                            std::cout << "Method '" << method << "' is allowed for this CGI." << std::endl;

                        }
                        else
                        {
                            std::cerr << "Error: Method '" << method << "' is not allowed for this CGI location." << std::endl;

                        }

                    }
                    else
                    {
                        std::string method = request.RequestLine.get_method();

                        if (method == "GET") {

                        }

                        else if (method == "POST") {

                        }

                        else if (method == "DELETE") {

                        }
                    }
                }
                else
                {
                    std::cerr << "Error: No matching location found for URI: " << requestUri << std::endl;
                    // hna makaynach dik location li tleb
                    //khasna nsifto lih 404 page
                }

                epoll_modify();
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error processing request for fd " << socket_fd << ": " << e.what() << std::endl;
            // Khasna ndiro error 400 hna
            epoll_modify();
        }
    }
    else if (event & EPOLLOUT)
    {
        std::cout << "send output" << std::endl;
        std::fstream file_test("ddd", std::ios::binary | std::ios::out);
        std::ifstream file("www/index.html");
        std::stringstream ss;

        ss << file.rdbuf();
        // std::cout << ss.rdbuf() << std::endl;

        std::string holder = ss.rdbuf()->str();
        std::stringstream len;
        len << holder.length();
        file_test.write(request.body_content.rdbuf()->str().c_str(), request.body_content.rdbuf()->str().length());

        std::string res = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + len.rdbuf()->str() + "\r\n\r\n" + holder;
        send(socket_fd, res.c_str(), res.length(), 0);
        throw std::runtime_error("finished sending response.");
    }
}

client::~client()
{
}