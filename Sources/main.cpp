#include "ServerManager.hpp"
#include "ConfigFileParser.hpp"
#include "ConfigFileReader.hpp"

int main(int ac, char **av)
{
	if (ac > 2)
	{
		std::cerr << RED << "Usage: ./webserv [path_to_config_file]" << RESET << std::endl;
		return 1;
	}

	std::string configPath = (ac == 2) ? av[1] : "configs/l3robi.conf";

	try
	{
		srand(time(0));
		FileReader reader(configPath);

		ConfigParser parser(reader.getContent());

		const std::vector<ServerConfigs> &servers = parser.getServers();

		serverManager::setupServers(servers);

		serverManager::startServers();

	}
	catch (const std::exception &e){
		std::cerr << RED << "\nError: " << e.what() << RESET << std::endl;
		return 1;
	}

	return 0;
}