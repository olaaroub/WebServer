#include "client.hpp"
#include "ServerManager.hpp"
#include "Utils.hpp"
#include "Get.hpp"
client::client(const ServerConfigs &server_config) : network(server_config, false) { request.state = 0; }

void client::epoll_modify()
{
    ev.events = EPOLLOUT | EPOLLRDHUP;
    ev.data.fd = socket_fd;
    if (epoll_ctl(serverManager::kernel_identifier, EPOLL_CTL_MOD, socket_fd, &ev) < 0)
        throw std::runtime_error("Client Error: epoll control failed!");
}

const LocationConfigs *client::findLocation(const std::string &uri) // i should handle the case where
{                                                                   // /images/ or /images and the given uri uses that prefix TODO
    const LocationConfigs *bestMatch = NULL;
    size_t len = 0;

    const std::vector<LocationConfigs> &locations = this->server_config.locations;

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
        throw std::runtime_error("Client disconnected or socket error.");
    else if (event & EPOLLIN)
    {
        bool is_request_complete = request.run_parser(socket_fd);

        if (is_request_complete)
            epoll_modify();
    }
    else if (event & EPOLLOUT)
    {
        // std::cout << "Body :" << request.body_content.rdbuf()->str() << std::endl;
        std::string fullPath;
        const std::string &requestUri = normalizePath(request.RequestLine.getUrl());

        const LocationConfigs *location = findLocation(requestUri);
        if (location)
            fullPath = joinPaths(location->root, requestUri);

        if (!location)
        {
            response res_error(socket_fd, "404", "");
            throw std::runtime_error("Response 404 sent!");
        }
        else if (std::find(location->allowed_methods.begin(), location->allowed_methods.end(), request.RequestLine.get_method()) == location->allowed_methods.end())
        {
            response res_error(socket_fd, "405", "");
            throw std::runtime_error("Response 405 sent!");
        }
        std::cout << requestUri << std::endl;

        std::string extension  = getExtension(fullPath); // this will check for cgi extentions, it will return the extention
        std::cout << "Extension: " << extension << std::endl;

        if (location->cgi_handlers.count(extension)) // i will work here if the extention is cgi
        {
            // std::cout << red << "----------- PART OF METHODS CGI START --------------" << reset << std::endl;
            // Cgi cgi(fullPath, location, request);
            // std::string type_res = cgi.check_path();
            // response res(socket_fd, type_res, cgi.get_final_path());
            // throw std::runtime_error("Response" + type_res + "sent!");

            std::cout << "this is a cgi request" << std::endl;
        }
        else
        {
            std::cout << "this is not a cgi request" << std::endl;
            response res_error(socket_fd, "404", "");
            throw std::runtime_error("Response 404 sent!");
        }


        if (request.RequestLine.get_method() == "GET") // here my teammate will work on get and post after i check that there is no cgi
        {
            std::cout << red << "----------- PART OF METHODS GET START --------------" << reset << std::endl;
            Get get(fullPath, location);
            std::string type_res = get.check_path();
            response res(socket_fd, type_res, get.get_final_path());
            throw std::runtime_error("Response" + type_res + "sent!");
        }
    }
}

client::~client()
{
}