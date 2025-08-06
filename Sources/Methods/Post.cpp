#include "Post.hpp"
#include "Utils.hpp"

std::string Post::extractfileName(std::string query)
{
    unsigned long pos = query.find("=");
    if (pos != std::string::npos)
    {
        return query.substr(pos + 1);
    }
    return "error";
}

std::string Post::get_locationFiles()
{
    return location_savedFiles;
}

    Post::Post(std::string location_savedFiles)
    {
        this->location_savedFiles = location_savedFiles;
    }
    Post::~Post(){}
