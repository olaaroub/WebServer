#include "WebServer.hpp"
#include "ConfigFileParser.hpp"
#include "ConfigFileReader.hpp"

int main(int ac, char **av)
{
	if (ac > 2)
	{
		std::cerr << "Usage: ./webserv [path_to_config_file]" << std::endl;
		return 1;
	}

	std::string configPath = (ac == 2) ? av[1] : "configs/l3robi.conf";

	try
	{
		FileReader reader(configPath);

		ConfigParser parser(reader.getContent());

		const std::vector<ServerConfigs> &servers = parser.getServers();

		WebServer::run_webserver(servers);

	}
	catch (const std::exception &e){
		std::cerr << "\nError: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}