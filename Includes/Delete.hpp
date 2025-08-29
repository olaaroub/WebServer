#ifndef DELETE_HPP
#define DELETE_HPP

#include "lib.hpp"
#include "client.hpp"
class Delete
{
private:
	std::string 				path;
	const LocationConfigs 		*location;

public:
	Delete(std::string path, const LocationConfigs *location);
	~Delete();

	bool 						is_a_file;
	bool 						is_a_dir;
	int 						check_path();
	int 						check_dir(struct stat file_info);
	int 						check_file(struct stat file_info);
	int							delete_file();
	std::string 				get_final_path();
};

#endif