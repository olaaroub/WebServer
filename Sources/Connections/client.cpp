#include "client.hpp"
#include "ServerManager.hpp"
#include "Utils.hpp"
#include "Get.hpp"
#include "CgiExecutor.hpp"
#include "HttpResponse.hpp"
#include "Post.hpp"
#include "Delete.hpp"

client::client(const ServerConfigs &server_config) : network(server_config, false), _errorStute(noError) { _convertMaxBodySize(); }

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

void client::_convertMaxBodySize()
{
    request.max_body_size = get_max_body() * 1024 * 1024;
}

void client::sendResponseString(const std::string &response)
{
    ssize_t bytes_sent = send(this->socket_fd, response.c_str(), response.length(), 0);
    if (bytes_sent < 0 || static_cast<size_t>(bytes_sent) < response.length())
        throw std::runtime_error("Send Error: Failed to send full response to client.");
}

void client::handleHttpError(int statusCode)
{
    HttpResponse responseBuilder;
    responseBuilder.setStatus(statusCode);
    responseBuilder.addHeader("Content-Type", "text/html");

    std::string body;
    bool customPageFound = false;

    if (this->server_config.error_pages.count(statusCode))
    {

        std::string error_page_uri = this->server_config.error_pages.at(statusCode);
        const LocationConfigs *error_loc = findLocation(error_page_uri);
        if (error_loc)
        {
            std::string full_path = joinPaths(error_loc->root, error_page_uri);
            body = getFileContents(full_path);
            if (!body.empty())
                customPageFound = true;
        }
    }

    if (!customPageFound)
    {
        std::stringstream default_body_ss;
        const char *reasonPhrase = getReasonPhrase(statusCode);
        default_body_ss << "<html>\r\n<head><title>Error " << statusCode
                        << "</title></head>\r\n<body>\r\n<h1>" << statusCode << " | "
                        << reasonPhrase
                        << "</h1>\r\n</body>\r\n</html>";
        body = default_body_ss.str();
    }

    responseBuilder.setBody(body);
    sendResponseString(responseBuilder.toString());
}

void client::onEvent() // handlehttprequest
{
    lastActivity = time(NULL); // set the last activity time for the client
    if (event & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
        throw std::runtime_error("Client disconnected or socket error.");
    else if (event & EPOLLIN)
    {
        try
        {
            bool is_request_complete = request.run_parser(socket_fd);
            if (is_request_complete)
            {
                epoll_modify();
                std::cout << "Request parsed successfully." << std::endl;
            }
        }
        catch(const ParseError &e)
        {
            epoll_modify();
            _errorStute = e.getStutError();
            std::cerr << e.what() << '\n';
            if (_errorStute == closeConnection)
                throw std::runtime_error("client close connection");
        }
        catch(const std::exception& e)
        {
            epoll_modify();
            _errorStute = ServerError;
            std::cerr << e.what() << '\n';
        }

    }
    else if (event & EPOLLOUT)
    {
        if (_errorStute != noError)
        {
            handleHttpError(_errorStute);
            throw std::runtime_error("send Response !");
        }
        const std::string &requestUri = normalizePath(request.requestLine.getUrl());
        std::cout << "Uri  after normalizing: " << requestUri << std::endl;
        const LocationConfigs *location = findLocation(requestUri);
        if (!location)
        {
            handleHttpError(404);
            throw std::runtime_error("Response error sucess !");
        }
        if (location->auth_required)
        {

            std::string sessionId = request.headers.getCookie("sessionid");

            if (serverManager::validateSession(sessionId) == false)
            {
                std::cout << red << "Access denied: Wlah la dkhelti l " << requestUri << ". sir sayb compte." << reset << std::endl;
                handleHttpError(403);
                throw std::runtime_error("Access denied due to invalid session.");
            }
            std::cout << green << "Access granted:  mar7ba biiik  " << requestUri << ". Valid session." << reset << std::endl;
        }

        std::cout << "returned location is " << location->path << std::endl;
        std::string fullPath = joinPaths(location->root, requestUri);
        std::cout << green << fullPath << reset << std::endl;
        std::string extension = getExtension(fullPath);
        std::cout << "Extension: " << extension << std::endl;

        if (location->cgi_handlers.count(extension))
        {
            struct stat script_stat;
            if (request.requestLine.get_method() == "DELETE" || (std::find(location->allowed_methods.begin(), location->allowed_methods.end(),
             request.requestLine.get_method()) == location->allowed_methods.end()))//  check if method is allowed
        {
            handleHttpError(405);
            throw std::runtime_error("Method Not Allowed");
        }
            if (stat(fullPath.c_str(), &script_stat) != 0)
            {
                handleHttpError(404);
                throw std::runtime_error("Response 404 sent for non-existent CGI script!");
            }
            if (!(script_stat.st_mode & S_IRUSR))
            {
                handleHttpError(403);
                throw std::runtime_error("Response 403 sent for unreadable CGI script!");
            }
            try
            {

                serverManager::activeNetworks.erase(this->socket_fd);
                epoll_ctl(serverManager::kernel_identifier, EPOLL_CTL_DEL, this->socket_fd, 0);

                new CgiExecutor(this->server_config, *location, request, this, fullPath);
            }
            catch (const std::exception &e)
            {
                std::cerr << red << "Failed to launch CGI: " << e.what() << reset << std::endl;
                handleHttpError(500);
                delete this;
            }
            return;
        }
        if (location->redirection_set)// hada ila kant redirect
        {
            std::cout << "Redirecting to: " << location->redirection_url << std::endl;
            if (location->redirection_code != 301 && location->redirection_code !=
                302)
            {
                handleHttpError(500);
                throw std::runtime_error("Invalid redirection code.");
            }
            HttpResponse responseBuilder;
            responseBuilder.setStatus(location->redirection_code);
            responseBuilder.addHeader("Location", location->redirection_url);
            sendResponseString(responseBuilder.toString());
            std::cout << red << responseBuilder.toString() << reset << std::endl;
            // exit(0);
            throw std::runtime_error("Redirect response sent.");
        }

        if (std::find(location->allowed_methods.begin(), location->allowed_methods.end(),
             request.requestLine.get_method()) == location->allowed_methods.end())//  check if method is allowed
        {
            handleHttpError(405);
            throw std::runtime_error("Method Not Allowed");
        }

        if (request.requestLine.get_method() == "GET")
        {
            std::cout << red << "----------- GET START --------------" << reset << std::endl;
            HttpResponse SendResp;
            try
            {
                Get get(fullPath, location);
                int type_res = get.check_path();
                if (type_res == 0) // mean autoindex is on . !
                {
                    SendResp.setStatus(200);
                    SendResp.addHeader("Content-Type", "text/html");
                    SendResp.setBody(get.generate_Fileautoindex());
                    sendResponseString(SendResp.toString());
                    throw std::runtime_error("Auto index response sent!");
                }
                else if (type_res == 1)
                {
                    SendResp.setStatus(200);
                    SendResp.addHeader("Content-Type", getMimeType(get.get_final_path()));
                    SendResp.setBody(generate_body_FromFile(get.get_final_path()));
                    sendResponseString(SendResp.toString());
                    throw std::runtime_error("Response Get sent sucess !");
                }
                else
                {
                    handleHttpError(type_res);
                    std::cerr << "error: " << type_res << " sent!" << std::endl;
                    throw std::runtime_error("Response error sucess !");
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Response  sent!: " << std::endl;
                throw std::runtime_error(e.what());
            }
        }
        else if (request.requestLine.get_method() == "POST")
        {
            HttpResponse SendResp;
            try
            {
                Post post(location->upload_path);
                std::map<std::string, std::vector<std::string> >::const_iterator it;
                it = request.headers.map.find("content-type");
                if (it == request.headers.map.end()) // it not found the content-type correctly !
                {
                    handleHttpError(400);
                    throw std::runtime_error("Response error sucess !");
                }

                std::string content_type = request.headers.map["content-type"].at(0);

                unsigned long check_multipartFOrmData = content_type.find("multipart/form-data");
                if (check_multipartFOrmData != std::string::npos)
                {
                    int type_res = post.post_multipartFormData(content_type, request.body_content.str());
                    if (type_res != 1)
                    {
                        handleHttpError(type_res);
                        throw std::runtime_error("Response error sucess !");
                    }
                }
                else
                    post.post_Query(request.requestLine.queryLine, request.body_content.str());

                SendResp.setStatus(201);
                SendResp.addHeader("Content-Type", "text/html");
                SendResp.setBody(generate_body_FromFile("./www/response.html"));
                sendResponseString(SendResp.toString());
                throw std::runtime_error("Response Post sent sucess !");
            }
            catch (const std::exception &e)
            {
                std::cerr << "Response  sent!: " << std::endl;
                throw std::runtime_error(e.what());
            }

        }
        else if (request.requestLine.get_method() == "DELETE")
        {
            HttpResponse SendResp;
            try
            {
                Delete del(fullPath, location);
                int type_res = del.check_path();
                if (type_res != 1)
                {
                    handleHttpError(type_res);
                    throw std::runtime_error("Response error sucess !");
                }
                if (del.is_a_file == true)
                {
                    int code = del.delete_file();
                    if (code == 1)
                    {
                        SendResp.setStatus(204);
                        SendResp.addHeader("Content-Type", "text/html");
                        // SendResp.setBody(generate_body_FromFile("./Pages/response.html"));
                        sendResponseString(SendResp.toString());
                        throw std::runtime_error("Response Delete sent sucess !");

                        // SendResponse.delete_response();
                    }
                    else
                    {
                        handleHttpError(code);
                        throw std::runtime_error("Response error sucess !");
                    }
                }
                else
                {
                    handleHttpError(403);
                    throw std::runtime_error("Response error sucess !");
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Response  sent!: " << std::endl;
                throw std::runtime_error(e.what());
            }
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
