#include "lib.hpp"
#include "ConfigFileReader.hpp"
#include "ConfigFileParser.hpp"

int main(int ac, char **av)
{
	if (ac > 2 )
	{
		std::cerr << "Usage: ./webserv [configuration_file_path]" << std::endl;
		return 1;
	}
	std::string configPath;
	if (ac == 2)
		configPath = av[1];
	else
		configPath = "configs/testing.conf";

	try
	{
		FileReader reader(configPath);
		std::cout << "== file read done ==" << std::endl;

		// std::cout << "tetss: " << reader.getContent() << std::endl;
		ConfigParser parser(reader.getContent());
		std::cout << "== file parsedone ==" << std::endl;

		const std::vector<std::string> &server_blocks = parser.getRawServerBlocks();

		std::cout << "\nfound " << server_blocks.size() << " server blocks" << std::endl;
		std::cout << "========================================" << std::endl;

		for (size_t i = 0; i < server_blocks.size(); ++i)
		{
			std::cout << "--- block " << i + 1 << " ---" << std::endl;
			std::cout << server_blocks[i] << std::endl;
			std::cout << "========================================" << std::endl;
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
