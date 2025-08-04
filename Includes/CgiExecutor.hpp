#ifndef CGI_EXECUTOR_HPP
#define CGI_EXECUTOR_HPP

#include "network.hpp"
#include "request.hpp"
#include "client.hpp"
#include "HttpResponse.hpp"
#include "Utils.hpp"

class CgiExecutorException : public std::runtime_error {
public:
    explicit CgiExecutorException(const std::string& what) : std::runtime_error(what) {}
};

class CgiExecutor : public network
{
public:
    enum CgiState {
        CGI_WRITING,
        CGI_READING,
        CGI_DONE,
        CGI_ERROR
    };

    CgiExecutor(const ServerConfigs& serverConf, const LocationConfigs& loc, const Request& req, client* client_instance, const std::string& path);
    virtual ~CgiExecutor();

    virtual void onEvent();
    bool isCgi() const { return true; }

    pid_t getPid() const { return _pid; }
    time_t getStartTime() const { return _startTime; }
    client* getClient() const { return _client; }
    CgiState getState() const { return _state; }
    void setState(CgiState state) { _state = state; }


private:
    pid_t       _pid;
    int         _pipe_in_fd;  // Parent writes to this (CGI's stdin)
    int         _pipe_out_fd; // Parent reads from this (CGI's stdout)
    time_t      _startTime;
    CgiState    _state;

    std::string _requestBody;
    size_t      _bytesWritten;
    std::string _responseBuffer;

    client* _client;
    char** _envp;
    char** _argv;

    void _setupEnvironment(const Request& req, const LocationConfigs& loc, const std::string& path);
    void _setupArguments(const std::string& cgi_path, const std::string& script_path);
    void _cleanup();
    void _closeFds();

    void _handleWrite();
    void _handleRead();

    CgiExecutor();
};

#endif
