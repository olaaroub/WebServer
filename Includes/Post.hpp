#ifndef POST_HPP
#define POST_HPP

#include "client.hpp"
#include "lib.hpp"

class Post
{
private:
	std::string 				location_savedFiles;

public:
	Post(std::string location_savedFiles);
	~Post();

	int 						post_multipartFormData(std::string content_type, std::string body_content);
	void 						post_Query(std::string queryLine, std::string body_content, std::string contentType);
	std::string 				path_savedFile;
	std::string 				get_locationFiles();
	std::string 				extractfileName(std::string query);
	std::string 				get_boundary(std::string content_type);
	std::string 				extract_nameFile(std::string header);

};

#endif