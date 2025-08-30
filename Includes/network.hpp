#ifndef NETWORK_HPP
#define NETWORK_HPP

#include "lib.hpp"
#include "Configs.hpp"
class network
{
protected:
	int 						_socket_fd;
	bool 						_is_server;
	sockaddr_in 				_network_infos;
	const ServerConfigs			&_server_config;
	int 						event;

public:
	network(const ServerConfigs &server_config, bool is_server);

	epoll_event 				ev;
	sockaddr_in					*get_sockaddr();
	bool 						if_server();
	int							get_socket_fd();
	int 						get_event();
	void 						epoll_crt(int event_flags);
	void 						epoll_crt();
	void 						set_event(int ev);
	void 						set_ToNoBlocking();
	virtual void 				onEvent() = 0;
	virtual bool 				isCgi() const;
	virtual 					~network();

};

#endif