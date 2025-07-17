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

const LocationConfigs *client::findLocation(const std::string &uri)
{
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
        std::cout << "I am here\n";
        bool is_request_complete = request.run_parser(socket_fd);

        if (is_request_complete)
        {
            std::cout << request.RequestLine.getUrl() << " method: " << request.RequestLine.get_method() << " Query: " << request.RequestLine.Query_lien << std::endl;
            epoll_modify();
        }
    }
    else if (event & EPOLLOUT)
    {
        // std::cout << "Body :" << request.body_content.rdbuf()->str() << std::endl;
        std::fstream BodyFile("tesst");
        BodyFile.write(request.body_content.rdbuf()->str().c_str(), request.body_content.rdbuf()->str().length());
        std::string fullPath;
        const std::string &requestUri = normalizePath(request.RequestLine.getUrl());

        const LocationConfigs *location = findLocation(requestUri);
        if (location)
            fullPath = joinPaths(location->root, requestUri);

        if (!location)
        {
            response res_error(socket_fd, "404", " ");
            throw std::runtime_error("Response 404 sent!");
        }
        else if (std::find(location->allowed_methods.begin(), location->allowed_methods.end(), request.RequestLine.get_method()) == location->allowed_methods.end())
        {
            response res_error(socket_fd, "405", " ");
            throw std::runtime_error("Response 405 sent!");
        }

        // std::string extension  = "wa7d";
        // = getExtention(requestUri); .php .py .sh /// todo

        // if (location->cgi_handlers.count(extension))
        // {
        //     // CgiClass obj;

        //     // hna ankhdem cgi

        if (request.RequestLine.get_method() == "GET")
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