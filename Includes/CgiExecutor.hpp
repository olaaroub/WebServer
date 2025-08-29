#ifndef CGI_EXECUTOR_HPP
#define CGI_EXECUTOR_HPP

#include "network.hpp"
#include "request.hpp"
#include "client.hpp"
#include "HttpResponse.hpp"
#include "Utils.hpp"

class CgiExecutorException : public std::runtime_error{
public:
	explicit CgiExecutorException(const std::string &what) : std::runtime_error(what) {}
};

class CgiExecutor : public network{
public:
	enum 			CgiState
	{
		CGI_WRITING,
		CGI_READING,
		CGI_DONE,
		CGI_ERROR
	};

	CgiExecutor(const ServerConfigs &serverConf, const LocationConfigs &loc,
				 const Request &req, client *client_instance, const std::string &path);

	pid_t			getPid() const;
	time_t			getStartTime() const;
	client			*getClient() const;
	CgiState		getState() const;
	void			removeClient();
	bool			isCgi() const;
	void			setState(CgiState state);
	virtual void 	onEvent();
	virtual			~CgiExecutor();

private:
	pid_t			_pid;
	time_t			_startTime;
	int				_pipe_in_fd;
	int				_pipe_out_fd;
	CgiState		_state;
	std::string		_requestBody;
	size_t			_bytesWritten;
	std::string		_responseBuffer;
	client			*_client;

	char			**_envp;
	char			**_argv;
	void 			_setupEnvironment(const Request &req, const LocationConfigs &loc, const std::string &path);
	void 			_setupArguments(const std::string &cgi_path, const std::string &script_path);
	void 			_cleanup();
	void 			_closeFds();
	void 			_handleWrite();
	void 			_handleRead();
	CgiExecutor();
};

#endif
