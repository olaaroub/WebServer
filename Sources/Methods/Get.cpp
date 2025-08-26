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
    // std::cout <<  red << "----------  " << RESET << std::endl;
    if (location->index_file.empty()) // mean the index file is not found
    {
        if (location->autoindex == true)
            return 0;
        else
            return 403;
    }
    std::string default_path = path + location->index_file;
    this->path = default_path;
    struct stat new_file_info;
    if (stat(this->path.c_str(), &new_file_info) != 0)
        return 404;
    return check_file(new_file_info);
}

std::string Get::generate_Fileautoindex(std::string requestUri)
{
    DIR *dir = opendir(path.c_str());
    // if (dir == NULL)
    //     return 500;

    std::stringstream html;
    html << "<!DOCTYPE html>\n<html>\n<head><title>listing directory " << path << "</title></head>\n";
    html << "<body>\n<h1>" << path << "</h1>\n<ul>\n";

    html << "<li><a href=\"..\">../</a></li>\n";

    struct dirent *entry;

    while (true)
    {
        entry = readdir(dir);
        if (entry == NULL)
            break ;
        std::string name = entry->d_name;


        if (name == "." || name == "..")
        {
            continue;
        }
        std::string link_path = name;

        // Check if the entry is a directory
        if (entry->d_type == DT_DIR)
            link_path += "/";
        // check
        if (path[path.length() - 1] != '/')
        {
            link_path = requestUri + "/" + link_path;
        }
        // Add the link
        html << "<li><a href=\"" <<  link_path << "\">" << name << "</a></li>\n";
        // std::cout << "!!!!!!!!!!!   " << link_path << std::endl;
    }

    html << "</ul>\n</body>\n</html>";
    closedir(dir);
    return html.str();
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
