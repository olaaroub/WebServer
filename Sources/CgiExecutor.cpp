#include "CgiExecutor.hpp"
#include "ServerManager.hpp"

CgiExecutor::CgiExecutor(const ServerConfigs &serverConf, const LocationConfigs &loc, const Request &req, client *client_instance, const std::string &path)
	: network(serverConf, false),
	  _pid(-1), _pipe_in_fd(-1), _pipe_out_fd(-1), _state(CGI_WRITING),
	  _requestBody(req.body_content.str()), _bytesWritten(0),
	  _client(client_instance), _envp(NULL), _argv(NULL)
{
	_startTime = time(NULL);

	try
	{
		std::string extension = getExtension(path);
		if (!loc.cgi_handlers.count(extension))
			throw CgiExecutorException("No valid CGI handler for script extension.");
		_setupEnvironment(req, loc, path);
		_setupArguments(loc.cgi_handlers.at(extension), path);

		int cgi_pipe_in[2], cgi_pipe_out[2];
		if (pipe(cgi_pipe_in) < 0 || pipe(cgi_pipe_out) < 0)
			throw CgiExecutorException("pipe() failed.");
		_pipe_in_fd = cgi_pipe_in[1];
		_pipe_out_fd = cgi_pipe_out[0];

		_pid = fork();
		if (_pid < 0)
		{
			close(cgi_pipe_in[0]);
			close(cgi_pipe_in[1]);
			close(cgi_pipe_out[0]);
			close(cgi_pipe_out[1]);
			throw CgiExecutorException("fork() failed.");
		}

		if (_pid == 0)
		{
			close(cgi_pipe_in[1]), close(cgi_pipe_out[0]);
			dup2(cgi_pipe_in[0], STDIN_FILENO);
			dup2(cgi_pipe_out[1], STDOUT_FILENO);
			close(cgi_pipe_in[0]), close(cgi_pipe_out[1]);

			std::string scriptDir = path.substr(0, path.find_last_of("/"));
			// std::cerr << "CGI script directory: " << scriptDir << std::endl;

			if (chdir(scriptDir.c_str()) != 0)
			{
				std::cerr << RED << "CGI Error: chdir to " << scriptDir << " failed." << RESET << std::endl;
				exit(EXIT_FAILURE);
			}
			execve(_argv[0], _argv, _envp);
			std::cerr << RED << "CGI Error: execve failed for " << _argv[0] << RESET << std::endl;
			exit(EXIT_FAILURE);
		}

		close(cgi_pipe_in[0]), close(cgi_pipe_out[1]);
		if(fcntl(_pipe_in_fd, F_SETFL, O_NONBLOCK) < 0 || fcntl(_pipe_out_fd, F_SETFL, O_NONBLOCK) < 0)
			throw CgiExecutorException("fcntl() failed");

		if (!_requestBody.empty())
		{
			_state = CGI_WRITING;
			this->socket_fd = _pipe_in_fd;
			this->epoll_crt(EPOLLOUT);
		}
		else
		{
			_state = CGI_READING;
			close(_pipe_in_fd);
			_pipe_in_fd = -1;
			this->socket_fd = _pipe_out_fd;
			this->epoll_crt(EPOLLIN);
		}
		serverManager::activeNetworks[this->socket_fd] = this;
	}
	catch (...)
	{
		_cleanup();
		throw;
	}
}

CgiExecutor::~CgiExecutor()
{
	_cleanup();
	if (_client)
	{
		delete _client;
		_client = NULL;
	}
}

void CgiExecutor::_cleanup()
{
	_closeFds();
	if (_argv)
	{
		for (int i = 0; _argv[i]; ++i)
			free(_argv[i]);
		delete[] _argv;
		_argv = NULL;
	}
	if (_envp)
	{
		for (int i = 0; _envp[i]; ++i)
			free(_envp[i]);
		delete[] _envp;
		_envp = NULL;
	}
}

void CgiExecutor::_closeFds()
{
	if (_pipe_in_fd != -1)
	{
		epoll_ctl(serverManager::kernel_identifier, EPOLL_CTL_DEL, _pipe_in_fd, 0);
		close(_pipe_in_fd);
		_pipe_in_fd = -1;
	}
	if (_pipe_out_fd != -1)
	{
		epoll_ctl(serverManager::kernel_identifier, EPOLL_CTL_DEL, _pipe_out_fd, 0);
		close(_pipe_out_fd);
		_pipe_out_fd = -1;
	}
}

void CgiExecutor::onEvent()
{

	if (_state == CGI_WRITING && (event & EPOLLOUT))
        _handleWrite();
    else if (_state == CGI_READING && (event & EPOLLIN))
        _handleRead();

    if (event & (EPOLLHUP | EPOLLERR))
    {

        _handleRead();
        _state = CGI_DONE;
        int status;
        waitpid(_pid, &status, 0);

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		{
			std::cerr << RED << "[CGI] Script exited with non-zero status for PID " << _pid << RESET << std::endl;
			_client->handleHttpError(502);
		}
            // _client->sendErrorResponse(502, "Bad Gateway");
        else
        {
			HttpResponse cgiResponseBuilder;
            cgiResponseBuilder.setFromCgiOutput(_responseBuffer);

			if (cgiResponseBuilder.getHeader("content-type").empty()) // get header kat rje3 kolchi miniscule bach manb9ach nchecki bzf
            {
				std::cerr << YELLOW << "[CGI] Script for PID " << _pid << " did not return a Content-Type header." << RESET << std::endl;
				_client->handleHttpError(502);
			}


            else
            {
				std::string actionHeaderValue = cgiResponseBuilder.getHeader("X-Session-Action");

                if (!actionHeaderValue.empty() && actionHeaderValue.rfind("CREATE", 0) == 0)
                {
                    std::string username = "laaroubi_default";
                    size_t userPos = actionHeaderValue.find("user=");
                    if (userPos != std::string::npos)
                        username = actionHeaderValue.substr(userPos + 5);

                    std::string newSessionId = serverManager::createSession(username);

                    std::string cookieValue = "sessionid=" + newSessionId + "; HttpOnly; Path=/";

                    cgiResponseBuilder.addHeader("Set-Cookie", cookieValue);

            	}
				else if (!actionHeaderValue.empty() && actionHeaderValue.rfind("DELETE", 0) == 0)
				{
					std::string sessionId = _client->get_request().headers.getCookie("sessionid");

					serverManager::deleteSession(sessionId);

					std::string expiredCookie = "sessionid=deleted; HttpOnly; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT";
					cgiResponseBuilder.addHeader("Set-Cookie", expiredCookie);
				}

				// _client->sendResponseString(cgiResponseBuilder.toString());
				_client->prepareResponse(cgiResponseBuilder.toString());
				serverManager::activeNetworks[_client->get_socket_fd()] = _client;
    			_client = NULL; // fach kansift can rje3 l client wkan7ydo mn 3ndi servermanager how li mklef db

			}

    	}
		throw ResponseSentException("Cgi response sent");
	}
}

void CgiExecutor::_handleWrite()
{
	ssize_t bytes = write(_pipe_in_fd, _requestBody.c_str() + _bytesWritten, _requestBody.length() - _bytesWritten);

    if (bytes > 0)
        _bytesWritten += bytes;

    if (_bytesWritten >= _requestBody.length() || bytes <= 0)
    {
        serverManager::activeNetworks.erase(this->socket_fd);
        epoll_ctl(serverManager::kernel_identifier, EPOLL_CTL_DEL, this->socket_fd, 0);
        close(_pipe_in_fd);
        _pipe_in_fd = -1;

        _state = CGI_READING;
        this->socket_fd = _pipe_out_fd;
        this->epoll_crt(EPOLLIN);
        serverManager::activeNetworks[this->socket_fd] = this;
    }
}

void CgiExecutor::_handleRead()
{
	// char buffer[4096];
	// ssize_t bytes = read(_pipe_out_fd, buffer, sizeof(buffer));
	// std::cout << "CGI Output: " << buffer;
	// //print bytes read
	// std::cout << "Bytes read: " << bytes << std::endl;

	// if (bytes > 0)
	// {
	// 	_responseBuffer.append(buffer, bytes);
	// }
	// else
	// {
	// 	_state = CGI_DONE;
	// 	int status;
	// 	waitpid(_pid, &status, 0);

	// 	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	// 	{
	// 		_client->sendErrorResponse(502, "Bad Gateway");
	// 		return;
	// 	}

	// 	HttpResponse cgiResponse;
	// 	cgiResponse.setFromCgiOutput(_responseBuffer);
	// 	cgiResponse.sendResponse(_client->get_socket_fd());
	// }

	 while (true)
    {
        char buffer[4096];
        ssize_t bytes_read = read(_pipe_out_fd, buffer, sizeof(buffer));

        if (bytes_read > 0)
            _responseBuffer.append(buffer, bytes_read);
        else
            break;
    }
}

void CgiExecutor::_setupEnvironment(const Request &req, const LocationConfigs &loc, const std::string &path)
{
	(void)loc;
	std::vector<std::string> env;
	std::stringstream ss;

	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("REQUEST_METHOD=" + req.requestLine.get_method());
	env.push_back("QUERY_STRING=" + req.requestLine.queryLine);
	env.push_back("SCRIPT_NAME=" + req.requestLine.getUrl());
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("REDIRECT_STATUS=200");

	if (!server_config.server_names.empty())
		env.push_back("SERVER_NAME=" + server_config.server_names[0]);
	else
		env.push_back("SERVER_NAME=" + server_config.host);

	if (!server_config.ports.empty())
	{
		ss << server_config.ports[0];
		env.push_back("SERVER_PORT=" + ss.str());
		ss.str("");
	}

	char resolved_path[PATH_MAX];
	if (realpath(path.c_str(), resolved_path))
		env.push_back("SCRIPT_FILENAME=" + std::string(resolved_path));
	else
		env.push_back("SCRIPT_FILENAME=" + path);

	if (!_requestBody.empty())
	{
		ss << _requestBody.length();
		env.push_back("CONTENT_LENGTH=" + ss.str());
		ss.str("");
	}

	// if (req.headers.map.count("content-type")) {
	//     env.push_back("CONTENT_TYPE=" + req.headers.map.at("content-type")[0]);
	// }
	std::map<std::string, std::vector<std::string> >::const_iterator it;

	it = req.headers.map.find("content-type");
	if (it != req.headers.map.end() && !it->second.empty())
		env.push_back("CONTENT_TYPE=" + it->second[0]);

	for (it = req.headers.map.begin(); it != req.headers.map.end(); ++it)
	{
		if (it->first == "content-length" || it->first == "content-type")
			continue;

		// std::string header_name = "HTTP_";
		// for (size_t i = 0; i < it->first.length(); ++i)
		// {
		// 	if (it->first[i] == '-')
		// 		header_name += '_';
		// 	else
		// 		header_name += toupper(it->first[i]);
		// }

		std::string header_name = "HTTP_";
        std::string key = it->first;
        std::transform(key.begin(), key.end(), key.begin(), ::toupper);
        std::replace(key.begin(), key.end(), '-', '_');
        header_name += key;

		if (!it->second.empty())
			env.push_back(header_name + "=" + it->second[0]);
	}

	_envp = new char *[env.size() + 1];
	for (size_t i = 0; i < env.size(); ++i)
		_envp[i] = strdup(env[i].c_str());
	_envp[env.size()] = NULL;
}

void CgiExecutor::_setupArguments(const std::string &cgi_path, const std::string &script_path)
{
	_argv = new char *[3];
	_argv[0] = strdup(cgi_path.c_str());
	// std::cout << "script_path: " << script_path << std::endl;
	_argv[1] = strdup(script_path.substr(script_path.find_last_of("/") + 1).c_str());
	// std::cout << "argv[1]: " << _argv[1] << std::endl;
	// exit(0);
	_argv[2] = NULL;
}
