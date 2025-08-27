#ifndef SERVER_HPP
#define SERVER_HPP

#include "network.hpp"
#include "lib.hpp"

// class network;
class server : public network
{
private:
	in_port_t _port;
	in_addr_t _ip_addres;

	void creat_socket();
	void bind_and_listen();

public:
	void onEvent();

	server(const ServerConfigs &server_config);
	server(in_port_t port, in_addr_t ip_addres, const ServerConfigs &server_config);
};

#endif