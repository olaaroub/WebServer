#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "server.hpp"
#include "network.hpp"
#include "ConfigFileParser.hpp"

class CgiExecutor;

struct SessionData{
	std::string 								name;
	time_t										expiry_time;
};

class serverManager
{
private:
	serverManager();
	static struct epoll_event 					evlist;
	static const int 							request_timeout;
	static void 								add_server(network *instance);
	static void 								listening();
	static void 								epollEvent(int fd, int event);
	static void 								checkCgiTimeouts();
	static void 								reapChildProcesses();
	static void 								signal_handler(int);
	static std::map<std::string, SessionData>	s_activeSessions;
	static const std::string    				s_sessionFilePath;
    static void                 				saveSessionsToFile();
    static void                 				loadSessionsFromFile();
public:
	static std::map<int, network *> 			activeNetworks;

	static std::string							createSession(const std::string& username);
	static void 								deleteSession(const std::string& sessionId);
	static bool									validateSession(const std::string& sessionId);
	static int 									kernel_identifier;
	static bool									isShutdown;
	static void 								setupServers(const std::vector<ServerConfigs> &servers);
	static void 								startServers();
};

#endif