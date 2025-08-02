/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/12 22:13:46 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/02 15:28:46 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib.hpp"
#include "request.hpp"
#include "Configs.hpp"


class CgiServerException : public std::runtime_error {
public:
    explicit CgiServerException(const std::string& what) : std::runtime_error(what) {}
};


class CgiScriptException : public std::runtime_error {
public:
    explicit CgiScriptException(const std::string& what) : std::runtime_error(what) {}
};

class CgiScriptTimeoutException : public std::runtime_error {
public:
    explicit CgiScriptTimeoutException(const std::string& what) : std::runtime_error(what) {}
};


class CgiHandler {
private:
    // Request&                _request;
    const LocationConfigs&  _location;
    std::string             _scriptPath;
    std::string             _cgiExecutable;
    std::string             _requestMethod;
    std::string             _queryString;
    std::string             _url;
    std::map<std::string, std::vector<std::string> > _headers;
    std::string             _body;
    const ServerConfigs&    _serverConfig;

    pid_t                   _pid;
    int                     _pipe_in[2];
    int                     _pipe_out[2];
    char**                  _envp;
    char**                  _argv;

    void                    _setupEnvironment();
    void                    _setupArguments();
    void                    _cleanup();

public:
    CgiHandler(const LocationConfigs& loc, const std::string& path, const Request& req, const ServerConfigs &serverConf);
    ~CgiHandler();

    std::string execute();
};