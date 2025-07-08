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


	    // std::cout << "-- test --------------------------------" << std::endl;
        // std::string name = "mo";
        // std::string uri = "ismail";
		// unsigned long x=  uri.find(name);
		// if (x == std::string::npos)
		// 	std::cout << "not found" << std::endl;
		// else
		// 	std::cout << "found at index : " << x << std::endl;
        // // std::cout << uri.find(name) << std::endl;



        // std::cout << " -- test --------------------------------" << std::endl;


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