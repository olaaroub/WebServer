#include "network.hpp"
#include "ServerManager.hpp"

network::network(const ServerConfigs &server_config, bool is_server) : _is_server(is_server),
					 _server_config(server_config){}
					 
network::~network() {}
bool network::isCgi() const{ return false; }
sockaddr_in *network::get_sockaddr() { return &_network_infos; }
bool network::if_server() { return _is_server; }
void network::set_event(int ev){ event = ev; }
int network::get_event() { return event; }
int network::get_socket_fd() { return _socket_fd; }

void network::set_ToNoBlocking()
{
	int flags = fcntl(_socket_fd, F_GETFL, 0);
	if (flags < 0)
		throw std::runtime_error("Network Error: Failed to get socket flags");
	if (fcntl(_socket_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("Network Error: Failed to set non-blocking mode\n");
}

void network::epoll_crt()
{
	ev.events = EPOLLIN;
	ev.data.fd = _socket_fd;
	if (epoll_ctl(serverManager::kernel_identifier, EPOLL_CTL_ADD, _socket_fd, &ev) < 0)
		throw std::runtime_error("Network Error: epoll control failed!");
}

void network::epoll_crt(int event_flags)
{
	ev.events = event_flags | EPOLLRDHUP;
	ev.data.fd = _socket_fd;
	if (epoll_ctl(serverManager::kernel_identifier, EPOLL_CTL_ADD, _socket_fd, &ev) < 0)
		throw std::runtime_error("Network Error: epoll control failed!");
}

