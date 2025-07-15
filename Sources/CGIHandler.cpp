/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 21:50:30 by olaaroub          #+#    #+#             */
/*   Updated: 2025/07/15 18:05:12 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHandler.hpp"
#include "Utils.hpp"

CgiHandler::CgiHandler(const LocationConfigs &loc, const std::string &path, const Request &req)
    : _location(loc),
      _scriptPath(path),
      _requestMethod(req.requestLine.get_method()),
      _queryString(req.requestLine.queryLine),
      _url(req.requestLine.getUrl()),
      _headers(req.headers.map),
      _body(req.body_content.str()),
      _pid(-1), _envp(NULL), _argv(NULL)
{
    _pipe_in[0] = -1;
    _pipe_in[1] = -1;
    _pipe_out[0] = -1;
    _pipe_out[1] = -1;

    std::string extension = getExtension(_scriptPath);
    if (_location.cgi_handlers.count(extension))
        _cgiExecutable = _location.cgi_handlers.at(extension);
}

CgiHandler::~CgiHandler()
{
    _cleanup();
}

void CgiHandler::_setupEnvironment()
{
    std::vector<std::string> env;

    env.push_back("REQUEST_METHOD=" + _requestMethod);
    env.push_back("QUERY_STRING=" + _queryString);
    env.push_back("SCRIPT_NAME=" + normalizePath(_url));
    env.push_back("PATH_INFO=" + _scriptPath);
    env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    env.push_back("REDIRECT_STATUS=200");
    env.push_back("SCRIPT_FILENAME=" + _scriptPath); // hada mazal biingado abch y3tini smia dial script bo7dha

    if (!_body.empty())
    {
        std::stringstream ss;
        ss << _body.length();
        env.push_back("CONTENT_LENGTH=" + ss.str());
    }

    std::map<std::string, std::vector<std::string>>::const_iterator it;
    it = _headers.find("content-type");
    if (it != _headers.end() && !it->second.empty())
    {
        env.push_back("CONTENT_TYPE=" + it->second[0]);
    }

    for (it = _headers.begin(); it != _headers.end(); ++it)
    {
        if (it->first == "content-length" || it->first == "content-type")
            continue;
        std::string header_name = "HTTP_";
        for (size_t i = 0; i < it->first.length(); ++i)
        {
            if (it->first[i] == '-')
                header_name += '_';
            else
                header_name += toupper(it->first[i]);
        }
        if (!it->second.empty())
            env.push_back(header_name + "=" + it->second[0]);
    }

    _envp = new char *[env.size() + 1];
    for (size_t i = 0; i < env.size(); ++i)
        _envp[i] = strdup(env[i].c_str());
    _envp[env.size()] = NULL;
}

void CgiHandler::_setupArguments()
{
    _argv = new char *[3];

    _argv[0] = strdup(_cgiExecutable.c_str());
    _argv[1] = strdup(_scriptPath.c_str());
    _argv[2] = NULL;
}

void CgiHandler::_cleanup()
{
    if (_argv)
    {
        if (_argv[0])
            free(_argv[0]);
        if (_argv[1])
            free(_argv[1]);
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

std::string CgiHandler::execute()
{
    if (_cgiExecutable.empty())
        throw std::runtime_error("CGI Error: No executable found for script.");

    _setupArguments();
    _setupEnvironment();

    if (pipe(_pipe_in) < 0)
        throw std::runtime_error("CGI Error: pipe() failed for input.");

    if (pipe(_pipe_out) < 0)
    {
        close(_pipe_in[0]);
        close(_pipe_in[1]);
        throw std::runtime_error("CGI Error: pipe() failed for output.");
    }
    _pid = fork();

    if (_pid < 0)
    {
        close(_pipe_in[0]);
        close(_pipe_in[1]);
        close(_pipe_out[0]);
        close(_pipe_out[1]);
        throw std::runtime_error("CGI Error: fork() failed.");
    }

    if (_pid == 0)
    {

        close(_pipe_in[1]);
        close(_pipe_out[0]);

        if (dup2(_pipe_in[0], STDIN_FILENO) < 0)
        {
            _cleanup();
            exit(1);
        }

        if (dup2(_pipe_out[1], STDOUT_FILENO) < 0)
        {
            _cleanup();
            exit(1);
        }

        close(_pipe_in[0]);
        close(_pipe_out[1]);

        execve(_argv[0], _argv, _envp);

        std::cerr << "CGI Error: execve failed for script " << _scriptPath << std::endl;
        _cleanup();
        exit(1);
    }

    close(_pipe_in[0]);
    close(_pipe_out[1]);

    if (!_body.empty())
    {
        write(_pipe_in[1], _body.c_str(), _body.length());
    }
    close(_pipe_in[1]);
    close(_pipe_in[1]);

    std::string output;
    char buffer[4096];
    ssize_t bytes_read;
    
    while ((bytes_read = read(_pipe_out[0], buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytes_read] = '\0';
        output += buffer;
    }

    close(_pipe_out[0]);
    waitpid(_pid, NULL, 0);

    _cleanup();

    return output;

}
