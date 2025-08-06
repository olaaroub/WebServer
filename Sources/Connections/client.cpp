#include "client.hpp"
#include "ServerManager.hpp"
#include "Utils.hpp"
#include "Get.hpp"
#include "CGIHandler.hpp"
#include "CgiExecutor.hpp"
#include "HttpResponse.hpp"
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

void client::sendErrorResponse(int statusCode, const std::string &reasonPhrase) // hadi tatsifet ghir error pages safi.
{
    HttpResponse errorResponse;
    errorResponse.setStatus(statusCode, reasonPhrase);
    errorResponse.addHeader("Content-Type", "text/html");

    std::string errorBody;
    if (this->server_config.error_pages.count(statusCode))
    {
        std::string errorPageUri = this->server_config.error_pages.at(statusCode);
        const LocationConfigs *errorLocation = findLocation(errorPageUri);
        if (errorLocation)
        {
            std::string errorPagePath = joinPaths(errorLocation->root, errorPageUri);
            errorBody = getFileContents(errorPagePath);
        }
    }

    if (errorBody.empty())
    {
        std::stringstream ss;
        ss << "<html><body><h1>" << statusCode << " - " << reasonPhrase << "</h1></body></html>";
        errorBody = ss.str();
    }

    errorResponse.setBody(errorBody);
    errorResponse.sendResponse(socket_fd);
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

        std::string extension = getExtension(fullPath);
        std::cout << "Extension: " << extension << std::endl;

        // if (location->cgi_handlers.count(extension))
        // {
        //     std::string cgi_output;
        //     struct stat script_stat;
        //     if (stat(fullPath.c_str(), &script_stat) != 0)
        //     {
        //         sendErrorResponse(404, "Not Found"); // had function tatkhdem ghir m3a error pages li fl config file
        //                                              // khasna nkhdmo biha 7itach error pages khsna nhzohom mn config file
        //                                              // machi nb9aw n7to path dialhom fl funtion l9dima
        //         throw std::runtime_error("Response 404 sent for non-existent CGI script!");
        //     }
        //     if (!(script_stat.st_mode & S_IRUSR))
        //     {
        //         sendErrorResponse(403, "Forbidden");
        //         throw std::runtime_error("Response 403 sent for unreadable CGI script!");
        //     }
        //     try
        //     {
        //         CgiHandler cgi(*location, fullPath, request, this->server_config);
        //         std::string cgi_output = cgi.execute();

        //         // std::cout << "ALOOOOOOOO : " << cgi_output << std::endl;

        //         if (cgi_output.find("Content-Type:") == std::string::npos && cgi_output.find("content-type:") == std::string::npos &&
        //             cgi_output.find("Content-type:") == std::string::npos) // php wld l97ba howa li khlani nzid had check kaml
        //             throw CgiScriptException("Script response missing Content-Type header.");

        //         HttpResponse cgiResponse;
        //         cgiResponse.setFromCgiOutput(cgi_output);
        //         cgiResponse.sendResponse(socket_fd);
        //     }
        //     catch (const CgiScriptException &e)
        //     {
        //         std::cerr << red << "CGI Script Error: " << e.what() << reset << std::endl;
        //         sendErrorResponse(502, "Bad Gateway");
        //     }
        //     catch (const CgiScriptTimeoutException &e)
        //     {
        //         std::cerr << red << "CGI Script Error: " << e.what() << reset << std::endl;
        //         sendErrorResponse(504, "Gateway Timeout");
        //     }
        //     catch (const std::exception &e)
        //     {
        //         std::cerr << red << "An unexpected error occurred: " << e.what() << reset << std::endl;
        //         sendErrorResponse(500, "Internal Server Error");
        //     }
        //     throw std::runtime_error("CGI response sent successfully.");
        // }

        if (location->cgi_handlers.count(extension))
        std::cout << red << "------------ : " + request.requestLine.get_method() << reset << std::endl;
        if (location->cgi_handlers.count(extension)) // i will work here if the extention is cgi
        {
            try {

                serverManager::activeNetworks.erase(this->socket_fd);
                epoll_ctl(serverManager::kernel_identifier, EPOLL_CTL_DEL, this->socket_fd, 0);

                new CgiExecutor(this->server_config, *location, request, this, fullPath);

            } catch (const std::exception& e) {
                std::cerr << red << "Failed to launch CGI: " << e.what() << reset << std::endl;
                sendErrorResponse(500, "Internal Server Error");
                delete this;
            }
            return;
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
            // sendErrorResponse(404, "not found");
            response res(socket_fd, post.path_savedFile);
            throw std::runtime_error("Response sent successfully!");
        }
        else if (request.requestLine.get_method() == "DELETE")
        {

        }
    }
}

client::~client()
{
    if (this->socket_fd != -1)
    {
        close(this->socket_fd);
    }
}
