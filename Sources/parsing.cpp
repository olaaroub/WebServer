#include "lib.hpp"
#include "FileReader.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./webserv [configuration_file_path]" << std::endl;
		return 1;
	}
	std::string configPath = argv[1];

	try
	{
		FileReader reader(configPath);

		// ConfigParser parser(reader.getContent());

		// const std::vector<std::string> &server_blocks = parser.getRawServerBlocks();

		// std::cout << "\nFound " << server_blocks.size() << " server block(s)." << std::endl;
		// std::cout << "========================================" << std::endl;

		// for (size_t i = 0; i < server_blocks.size(); ++i)
		// {
		// 	std::cout << "--- Content of Server Block " << i + 1 << " ---" << std::endl;
		// 	std::cout << server_blocks[i] << std::endl;
		// 	std::cout << "========================================" << std::endl;
		// }
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}