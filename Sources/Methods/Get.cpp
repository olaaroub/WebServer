#include "Get.hpp"

int Get::check_file(struct stat file_info)
{
    if ((file_info.st_mode & S_IRUSR) == 0)
        return 403;
    return 1;
}

int Get::check_dir(struct stat file_info)
{
    if ((file_info.st_mode & S_IXUSR) == 0)
        return 403;

    // auto index handle !
    // std::cout <<  red << "----------  " << reset << std::endl;
    // if (location->index_file.empty()) // mean the index file is not found
    // {
    //     if (location->autoindex == true)
    //     {

    //     }
    // }else
    // {
    // }
    std::string default_path = path + location->index_file;
    this->path = default_path;
    struct stat new_file_info;
    if (stat(this->path.c_str(), &new_file_info) != 0)
        return 404;
    return check_file(new_file_info);
}

int Get::check_path()
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

std::string Get::get_final_path()
{
    return path;
}

Get::Get(std::string path, const LocationConfigs *location)
{
    this->location = location;
    this->path = path;
}
Get::~Get() {}
