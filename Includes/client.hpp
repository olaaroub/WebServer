#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "network.hpp"
#include "request.hpp"

class client : public network{
private:
	enum 				ClientState
	{
		READING,
		WRITING
	};
	Request 			_request;
	time_t 				_lastActivity;
	ClientState 		_state;
	std::string			_response_buffer;
	size_t 				_bytes_sent;
	bool 				_is_monitored;
	void 				_handleWrite();

public:
	client(const ServerConfigs &server_config);
	~client();
	bool 				requestComplete;
	void 				prepareResponse(const std::string &response);
	void 				sendResponseString(const std::string &response);
	const 				LocationConfigs *findLocation(const std::string &uri);
	void 				handleHttpError(int statusCode);
	void 				onEvent();
	void 				setMonitored(bool monitored);
	bool 				isMonitored() const;
	void 				set_fd(int fd);
	Request 			&get_request();
	long 				get_max_body();
	time_t				getTime() const;
	void				setTime(time_t time);
};

#endif