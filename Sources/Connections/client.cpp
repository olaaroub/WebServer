#include "client.hpp"
#include "ServerManager.hpp"
#include "Utils.hpp"
#include "Get.hpp"
#include "CgiExecutor.hpp"
#include "HttpResponse.hpp"
#include "Post.hpp"
#include "Delete.hpp"

client::client(const ServerConfigs &server_config) : network(server_config, false),
            _state(READING), _bytes_sent(0), _is_monitored(true),  is_request_complete(false) { request.max_body_size = get_max_body(); }


void client::setMonitored(bool monitored) { _is_monitored = monitored; }
bool client::isMonitored() const { return _is_monitored; }


void client:: set_fd(int fd)
{
   socket_fd = fd;
}

Request &client:: get_request()
{
    return request;
}

long client:: get_max_body()
{
    return server_config.client_max_body_size;
}

// void client::epoll_modify()
// {
//     ev.events = EPOLLOUT | EPOLLRDHUP;
//     ev.data.fd = socket_fd;
//     if (epoll_ctl(serverManager::kernel_identifier, EPOLL_CTL_MOD, socket_fd, &ev) < 0)
//         throw std::runtime_error("Client Error: epoll control failed!");

// }

void client::prepareResponse(const std::string& response)
{
    _response_buffer = response;
    _bytes_sent = 0;
    _state = WRITING;


    ev.events = EPOLLOUT | EPOLLRDHUP;
    ev.data.fd = socket_fd;

    if (_is_monitored)// machi cgi
    {
        if (epoll_ctl(serverManager::kernel_identifier, EPOLL_CTL_MOD, socket_fd, &ev) < 0) {
            throw std::runtime_error("Client Error: epoll_ctl MOD failed on a monitored client!");
        }
    }

   else // cgi
    {
        if (epoll_ctl(serverManager::kernel_identifier, EPOLL_CTL_ADD, socket_fd, &ev) < 0) {
             throw std::runtime_error("Client Error: epoll_ctl ADD failed on an unmonitored client!");
        }
        _is_monitored = true;
    }

}

void client::_handleWrite()
{
    if (_response_buffer.empty()) {
        throw ResponseSentException("response Sent");
    }

    size_t remaining = _response_buffer.length() - _bytes_sent;
    if (remaining == 0) {
        throw ResponseSentException("response Sent");
    }

    ssize_t bytes_sent_now = send(this->socket_fd, _response_buffer.c_str() + _bytes_sent, remaining, 0);

    if (bytes_sent_now < 0) {
        throw std::runtime_error("Send Error: Failed to send data to client.");
    }

    _bytes_sent += bytes_sent_now;

    if (_bytes_sent >= _response_buffer.length()) {
        throw ResponseSentException("response Sent");
    }
}


void client::handleHttpError(int statusCode)
{
    std::cout << RED << "[FD: " << this->socket_fd << "] Sending error response: " << statusCode
            << " " << getReasonPhrase(statusCode) << RESET << std::endl;
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
    // sendResponseString(responseBuilder.toString());
    prepareResponse(responseBuilder.toString());
    // _handleWrite();
}

void client::onEvent() // handlehttprequest
{
    if (event & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
        throw std::runtime_error("Client disconnected or socket error.");

    if (_state == WRITING && (event & EPOLLOUT)) {
        std::cout << "=== how many times did you enter here" << std::endl;
        _handleWrite();
        return;
    }


    if(_state == READING && (event & EPOLLIN))
    {
        try
        {
            lastActivity = time(NULL);
            is_request_complete = request.run_parser(socket_fd);
            if (is_request_complete)
            {
                // reque = true;
                 std::cout << MAGENTA << "[FD: " << this->socket_fd << "] Request received: "
                          << request.requestLine.get_method() << " "
                          << request.requestLine.getUrl() << RESET << std::endl;

				// if (_errorStute != noError) {
                //     handleHttpError(_errorStute);
                //     return;
                // }

				const std::string &requestUri = normalizePath(request.requestLine.getUrl());
                const LocationConfigs *location = findLocation(requestUri);

                if (!location) {
                    handleHttpError(404);
                    // _handleWrite();
                    return;
                }

				std::string fullPath = joinPaths(location->root, requestUri);
                std::string extension = getExtension(fullPath);


                if (location->cgi_handlers.count(extension))
                {

                    std::cout << MAGENTA << "[FD: " << this->socket_fd << "] Passing to CGI handler for " << fullPath << RESET << std::endl;
                    struct stat script_stat;
					if (request.requestLine.get_method() == "DELETE" || (std::find(location->allowed_methods.begin(), location->allowed_methods.end(),
						request.requestLine.get_method()) == location->allowed_methods.end()))//  check if method is allowed
					{
						handleHttpError(405);
						// throw ResponseSentException();
						return;
					}
					if (stat(fullPath.c_str(), &script_stat) != 0)
					{
						handleHttpError(404);
						std::cerr << RED << "Response 404 sent for non-existent CGI script!" << RESET << std::endl;
						return;
						// throw ResponseSentException();
					}
					if (!(script_stat.st_mode & S_IRUSR))
					{
						handleHttpError(403);
						std::cerr << RED << "Response 403 sent for unreadable CGI script!" << RESET << std::endl;
						return;
						// throw ResponseSentException();
					}

                    this->setMonitored(false);
					serverManager::activeNetworks.erase(this->socket_fd);
                    epoll_ctl(serverManager::kernel_identifier, EPOLL_CTL_DEL, this->socket_fd, 0);
                    new CgiExecutor(this->server_config, *location, request, this, fullPath);
                    return;
                }


				if (location->auth_required)
				{

					std::string sessionId = request.headers.getCookie("sessionid");

					if (serverManager::validateSession(sessionId) == false)
					{
						std::cout << YELLOW << "[FD: " << this->socket_fd << "] Access denied for " << requestUri << ". Invalid session." << RESET << std::endl;
						handleHttpError(403);
						return;
						// throw ResponseSentException();
						// throw std::runtime_error("Access denied due to invalid session.");
					}
					std::cout << GREEN << "[FD: " << this->socket_fd << "] Access granted for " << requestUri << ". Valid session." << RESET << std::endl;
				}


				if (location->redirection_set)
                {
                    if (location->redirection_code != 301 && location->redirection_code != 302)
                    {
                        handleHttpError(500);
                        return;
                        // _handleWrite();
                    }
                    std::cout << MAGENTA << "[FD: " << this->socket_fd << "] Redirecting to: " << location->redirection_url << RESET << std::endl;
					HttpResponse responseBuilder;
                    responseBuilder.setStatus(location->redirection_code);
                    responseBuilder.addHeader("Location", location->redirection_url);
                    prepareResponse(responseBuilder.toString());
                    _handleWrite();
                    return;
                }


				if (std::find(location->allowed_methods.begin(), location->allowed_methods.end(),
					request.requestLine.get_method()) == location->allowed_methods.end())//  check if method is allowed
				{
					handleHttpError(405);
					return;
					// throw ResponseSentException();
				}


				HttpResponse SendResp;
				if (request.requestLine.get_method() == "GET") {
					std::cout << MAGENTA << "[FD: " << this->socket_fd
						<< "] Responding with GET for " << fullPath << RESET << std::endl;
                    Get get(fullPath, location);
                    int type_res = get.check_path();
                    if (type_res == 0) {
                        SendResp.setStatus(200);
                        SendResp.addHeader("Content-Type", "text/html");
                        SendResp.setBody(get.generate_Fileautoindex());
                    } else if (type_res == 1) {
                        SendResp.setStatus(200);
                        SendResp.addHeader("Content-Type", getMimeType(get.get_final_path()));
                        SendResp.setBody(generate_body_FromFile(get.get_final_path()));
                    } else {
                        handleHttpError(type_res);
                        // _handleWrite();
                        return;
                    }

                }
				else if (request.requestLine.get_method() == "POST") {
					std::cout << MAGENTA << "[FD: " << this->socket_fd
						<< "] Responding with POST for " << fullPath << RESET << std::endl;
                    Post post(location->upload_path);

					// Post post(location->upload_path);
					std::map<std::string, std::vector<std::string> >::const_iterator it;
					it = request.headers.map.find("content-type");
					if (it == request.headers.map.end()) // it not found the content-type correctly !
					{
						handleHttpError(400);
						return;
						// throw std::runtime_error("Response error sucess !");
					}

					std::string content_type = request.headers.map["content-type"].at(0);

					unsigned long check_multipartFOrmData = content_type.find("multipart/form-data");
					if (check_multipartFOrmData != std::string::npos)
					{
						int type_res = post.post_multipartFormData(content_type, request.body_content.str());
						if (type_res != 1)
						{
							handleHttpError(type_res);
							return;
							// throw std::runtime_error("Response error sucess !");
						}
					}
					else
						post.post_Query(request.requestLine.queryLine, request.body_content.str());

					SendResp.setStatus(201);
					SendResp.addHeader("Content-Type", "text/html");
					SendResp.setBody(generate_body_FromFile("./www/response.html"));
					// sendResponseString(SendResp.toString());
					// throw std::runtime_error("Response Post sent sucess !");

                    SendResp.setStatus(201);
                }
				else if (request.requestLine.get_method() == "DELETE") {
					std::cout << MAGENTA << "[FD: " << this->socket_fd
						<< "] Responding with DELETE for " << fullPath << RESET << std::endl;
                    Delete del(fullPath, location);


					int type_res = del.check_path();
					if (type_res != 1){
						handleHttpError(type_res);
						return;
						// throw std::runtime_error("Response error sucess !");
					}
					if (del.is_a_file == true){
						int code = del.delete_file();
						if (code == 1){
							SendResp.setStatus(204);
							SendResp.addHeader("Content-Type", "text/html");
							// SendResp.setBody(generate_body_FromFile("./Pages/response.html"));
							// sendResponseString(SendResp.toString());
							// throw std::runtime_error("Response Delete sent sucess !");

						}
						else{
							handleHttpError(code);
							return;
						}
					}
					else{
						handleHttpError(403);
						return;
					}

                    SendResp.setStatus(204);
                }

                prepareResponse(SendResp.toString());
                _handleWrite();

            }
        }
        catch(const ParseError &e)
        {
            // _errorStute = e.getStutError();
            std::cerr << RED << e.what() << RESET << '\n';
            if (e.ErrorStute == closeConnection)
                throw std::runtime_error("client close connection");
            handleHttpError(e.ErrorStute);
            return;
            // _handleWrite();
        }
        catch(const ResponseSentException& e)
        {
            std::cout << GREEN << "[FD: " << this->socket_fd << "] Response sent successfully." << RESET << std::endl;
            throw ResponseSentException("Response sent successfully.");
            return;
        }
        catch(const std::exception& e)
        {
            std::cerr << " jit l second catch in client.cpp"  << e.what() << '\n';
            std::cerr << RED << "Internal Server Error in client.cpp: "  << e.what() << RESET << '\n';
            handleHttpError(ServerError);
            return;
            // _handleWrite();
        }

    }
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


void client::sendResponseString(const std::string &response)
{
    ssize_t bytes_sent = send(this->socket_fd, response.c_str(), response.length(), 0);
    if (bytes_sent < 0 || static_cast<size_t>(bytes_sent) < response.length())
        throw std::runtime_error("Send Error: Failed to send full response to client.");
}

client::~client()
{
    if (this->socket_fd != -1)
    {
        close(this->socket_fd);
    }
}

