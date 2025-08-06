#include "client.hpp"
#include "ServerManager.hpp"
#include "Utils.hpp"
#include "Get.hpp"
#include "CGIHandler.hpp"
#include <fstream>
#include "Post.hpp"

client::client(const ServerConfigs &server_config) : network(server_config, false) {_convertMaxBodySize();}

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

void client:: _convertMaxBodySize()
{
    request.max_body_size = get_max_body() * 1024 * 1024;
}

void client::onEvent() // handlehttprequest
{
    lastActivity = time(NULL); // set the last activity time for the client
    if (event & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
        throw std::runtime_error("Client disconnected or socket error.");
    else if (event & EPOLLIN)
    {
        bool is_request_complete = request.run_parser(socket_fd);
        if (is_request_complete)
        {
            epoll_modify();
            std::cout << "Request parsed successfully." << std::endl;
        }
    }
    else if (event & EPOLLOUT)
    {
        // std::cout << "Body :" << request.body_content.rdbuf()->str() << std::endl;
        // std::fstream BodyFile("tesst");
        // BodyFile.write(request.body_content.rdbuf()->str().c_str(), request.body_content.rdbuf()->str().length());
        std::string fullPath;
        const std::string &requestUri = normalizePath(request.requestLine.getUrl());
        // std::cout<< "Request URI: " << request.requestLine.getUrl() << std::endl;

        const LocationConfigs *location = findLocation(requestUri);
        if (location)
            fullPath = joinPaths(location->root, requestUri);

        if (!location)
        {
            response res_error(socket_fd, "404", "");
            throw std::runtime_error("Response 404 sent!");
        }
        // else if (std::find(location->allowed_methods.begin(), location->allowed_methods.end(), request.requestLine.get_method()) == location->allowed_methods.end())
        // {
        //     response res_error(socket_fd, "405", "");
        //     throw std::runtime_error("Response 405 sent!");
        // }

        std::cout << requestUri << std::endl;

        std::string extension  = getExtension(fullPath);
        std::cout << "Extension: " << extension << std::endl;


        std::cout << red << "------------ : " + request.requestLine.get_method() << reset << std::endl;

        if (location->cgi_handlers.count(extension)) // i will work here if the extention is cgi
        {
            std::string cgi_output;
            try
            {
                CgiHandler cgi(*location, fullPath, request);
                cgi_output = cgi.execute();
            }
            catch (const std::exception& e) {
                std::cerr << red << "CGI execution failed: " << e.what() << reset << std::endl;
                response res_error(socket_fd, "500", "");
                throw std::runtime_error("Response 500 sent!");
            }

            std::string headers_str;
            std::string body_str;
            size_t separator = cgi_output.find("\r\n\r\n");
            if (separator == std::string::npos) {
                separator = cgi_output.find("\n\n");
                if (separator != std::string::npos) {
                    headers_str = cgi_output.substr(0, separator);
                    body_str = cgi_output.substr(separator + 2);
                } else {
                    body_str = cgi_output;
                }
            } else {
                headers_str = cgi_output.substr(0, separator);
                body_str = cgi_output.substr(separator + 4);
            }

            std::stringstream final_response;
            final_response << "HTTP/1.1 200 OK\r\n";
            if (!headers_str.empty()) {
                final_response << headers_str << "\r\n";
            }
            final_response << "Content-Length: " << body_str.length() << "\r\n";
            final_response << "\r\n";
            final_response << body_str;

            send(socket_fd, final_response.str().c_str(), final_response.str().length(), 0);
            throw std::runtime_error("CGI response sent successfully.");
        }

        if (request.requestLine.get_method() == "GET")
        {
            std::cout << red << "----------- PART OF METHODS GET START --------------" << reset << std::endl;
            Get get(fullPath, location);
            std::string type_res = get.check_path();
            response res(socket_fd, type_res, get.get_final_path());
            throw std::runtime_error("Response" + type_res + "sent!");
        }
        else if (request.requestLine.get_method() == "POST")
        {
            Post post(location->root);
            if (request.requestLine.queryLine.empty())
                post.path_savedFile = joinPaths(post.get_locationFiles(), generateUniqueFilename());
            else
                post.path_savedFile = joinPaths(post.get_locationFiles(), post.extractfileName(request.requestLine.queryLine));
            std::cout << green << "path : " << post.path_savedFile << reset << std::endl;
            std::ofstream savefile(post.path_savedFile.c_str(), std::ios::binary);
            savefile.write(request.body_content.str().c_str() , request.body_content.str().length());
            savefile.close();
            response res(socket_fd, post.path_savedFile);
            throw std::runtime_error("Response sent successfully!");
        }
        else if (request.requestLine.get_method() == "DELETE")
        {
            
        }
    }
}

// -- //



// -- //
client::~client(){}