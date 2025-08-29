#include "server.hpp"
#include "client.hpp"
#include "ServerManager.hpp"

server::server(const ServerConfigs &server_config) : network(server_config, true) {}
server::server(in_port_t port, in_addr_t ip_addres, const ServerConfigs &server_config) : network(server_config, true), _port(port), _ip_addres(ip_addres)
{
	creat_socket();
	bind_and_listen();
	epoll_crt();
}

void server::bind_and_listen()
{
	int yes = 1;
	if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		perror("Server Error: setsocketopt error");
		throw ParseError("Exiting...", _socket_fd);
	}
	if (bind(_socket_fd, (sockaddr *)&_network_infos, sizeof(_network_infos)) < 0)
	{
		perror("Server Error: bind error");
		throw ParseError("Exiting...", _socket_fd);
	}
	set_ToNoBlocking();
	if (listen(_socket_fd, SOMAXCONN) < 0)
	{
		perror("Server Error: listen error");
		throw ParseError("Exiting...", _socket_fd);
		;
	}
}

void server::creat_socket()
{
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd < 0)
		ParseError("Server Error: Socket failed!", _socket_fd);
	_network_infos.sin_family = AF_INET;
	_network_infos.sin_addr.s_addr = this->_ip_addres;
	_network_infos.sin_port = htons(this->_port);
	memset(_network_infos.sin_zero, 0, sizeof(_network_infos.sin_zero));
}

void server::onEvent()
{
	sockaddr_in *clien_struct;

	if (event & EPOLLIN)
	{
		client *client_re = new client(_server_config);
		clien_struct = client_re->get_sockaddr();
		socklen_t addr_size = sizeof(clien_struct);
		int fd = accept(_socket_fd, (sockaddr *)clien_struct, &addr_size);
		if (fd < 0)
		{
			delete client_re;
			return;
		}
		std::cout << GREEN << "[FD: " << fd << "] New connection accepted." << RESET << std::endl;
		client_re->set_fd(fd);
		client_re->set_ToNoBlocking();
		client_re->epoll_crt();
		client_re->setTime(time(NULL));
		serverManager::activeNetworks[fd] = client_re;
	}
}