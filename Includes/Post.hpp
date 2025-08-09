#ifndef POST_HPP
#define POST_HPP

// #include "network.hpp"
// #include "request.hpp"
// #include "client.hpp"
#include "lib.hpp"
#include "Methods.hpp"

class Post
{
private:
    std::string location_savedFiles;
public:
    std::string path_savedFile;
    std::string get_locationFiles();
    std::string extractfileName(std::string query);

    int post_multipartFormData(std::string content_type, std::string body_content);
    void post_Query(std::string queryLine, std::string body_content);

    std::string get_boundary(std::string content_type);
    std::string extract_nameFile(std::string header);

    Post(std::string location_savedFiles);
    ~Post();
};

#endif