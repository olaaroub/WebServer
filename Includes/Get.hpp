#ifndef GET_HPP
#define GET_HPP

// #include "network.hpp"
// #include "request.hpp"
// #include "client.hpp"
#include "lib.hpp"
#include "Methods.hpp"

class Get
{
private:
    bool is_a_file;
    bool is_a_dir;
    std::string path;
    const LocationConfigs *location;

public:
    std::string check_path();
    std::string check_dir(struct stat file_info);
    std::string check_file(struct stat file_info);
    std::string get_final_path();
    Get(std::string path, const LocationConfigs *location);
    ~Get();
};

#endif