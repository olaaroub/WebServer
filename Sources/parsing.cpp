#include "ConfigFileReader.hpp"
#include "ConfigFileParser.hpp"

void printServerConfig(const ServerConfigs &server, size_t index);

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

		std::cout << "il y a " << servers.size() << " server configurations." << std::endl;

		for (size_t i = 0; i < servers.size(); ++i){
			printServerConfig(servers[i], i + 1);
		}
	}
	catch (const std::exception &e){
		std::cerr << "\nError: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

void printServerConfig(const ServerConfigs &server, size_t index)
{
	std::cout << "\n========================================" << std::endl;
	std::cout << "     Server Block: " << index << std::endl;
	std::cout << "  Host: " << server.host << std::endl;

	std::cout << "  Ports: ";
	for (size_t i = 0; i < server.ports.size(); ++i)
	{
		std::cout << server.ports[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "Body Size: " << server.client_max_body_size << std::endl;

	if (!server.server_names.empty())
	{
		std::cout << " Server Names: ";
		for (size_t j = 0; j < server.server_names.size(); ++j){
			std::cout << "'" << server.server_names[j] << "' ,";
		}
		std::cout << std::endl;
	}

	if (!server.error_pages.empty())
	{
		std::cout << "  Error Pages:" << std::endl;
		std::map<int, std::string>::const_iterator it;
		for (it = server.error_pages.begin(); it != server.error_pages.end(); ++it){
			std::cout << "    " << it->first << " -> " << it->second << std::endl;
		}
	}

	std::cout << " there is (" << server.locations.size() << ") locations:" << std::endl;
	for (size_t j = 0; j < server.locations.size(); ++j)
	{
		const LocationConfigs &loc = server.locations[j];
		std::cout << "    - Location Path: " << loc.path << std::endl;
		if (!loc.root.empty())
			std::cout << "      Root: " << loc.root << std::endl;
		if (!loc.index_file.empty())
			std::cout << "      Index: " << loc.index_file << std::endl;
		std::cout << "      autoindex: " << (loc.autoindex ? "on" : "off") << std::endl;
		if (loc.redirection_code != 0){
			std::cout << "      Redirect: " << loc.redirection_code << " -> " << loc.redirection_url << std::endl;
		}
		if (!loc.allowed_methods.empty()){
			std::cout << "      Allowed Methods: ";
			for (size_t k = 0; k < loc.allowed_methods.size(); ++k)
				std::cout << loc.allowed_methods[k] << " ";
			std::cout << std::endl;
		}
		if (!loc.upload_path.empty())
			std::cout << "      Upload Path: " << loc.upload_path << std::endl;
        if (!loc.cgi_handlers.empty())
        {
			std::map<std::string, std::string>::const_iterator it;
            std::cout << "      CGI handlers:" << std::endl;
            for (it = loc.cgi_handlers.begin(); it != loc.cgi_handlers.end(); ++it){
                std::cout << "        " << it->first << " -> " << it->second << std::endl;
            }
        }
	}
	std::cout << "========================================" << std::endl;
}
