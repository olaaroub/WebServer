#include "Delete.hpp"
#include "Utils.hpp"

int Delete::check_file(struct stat file_info)
{
	if ((file_info.st_mode & S_IRUSR) == 0)
		return 403;
	return 1;
}

int Delete::check_dir(struct stat file_info)
{
	if ((file_info.st_mode & S_IXUSR) == 0)
		return 403;

	// std::string default_path = path + location->index_file;
	// this->path = default_path;
	// struct stat new_file_info;
	// if (stat(this->path.c_str(), &new_file_info) != 0)
	//     return "404";
	// return check_file(new_file_info);
	// std::cout << RED << "you try to delete a dirctory !! " << RESET << std::endl;
	// std::cout << RED << "path of the dir : " << this->path << RESET << std::endl;
	return 1;
}

int Delete::check_path()
{
	struct stat file_info;
	if (stat(this->path.c_str(), &file_info) != 0)
		return 404;
	is_a_file = S_ISREG(file_info.st_mode);
	is_a_dir = S_ISDIR(file_info.st_mode);
	if (is_a_file == true)
		return check_file(file_info);
	return check_dir(file_info);
}

std::string Delete::get_final_path()
{
	return path;
}

int Delete::delete_file()
{
	if (std::remove(this->path.c_str()) == 0)
		std::cout << GREEN << "file deleted !" << RESET << std::endl;
	else
	{
		perror("Error deleting file");

		if (errno == ENOENT)
		{
			return 404;
			// Send 404 Not Found response
		}
		else if (errno == EACCES)
		{
			return 403;
			// Send 403 Forbidden response
		}
		else
		{
			return 404;
			// Handle other potential errors
		}
	}
	return 1;
}

Delete::Delete(std::string path, const LocationConfigs *location)
{
	this->location = location;
	this->path = path;
}
Delete::~Delete() {}
