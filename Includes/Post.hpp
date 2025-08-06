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
    Post(std::string location_savedFiles);
    ~Post();
};

#endif