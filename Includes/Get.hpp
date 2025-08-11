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
    int check_path();
    int check_dir(struct stat file_info);
    int check_file(struct stat file_info);
    std::string get_final_path();
    std::string generate_Fileautoindex();
    Get(std::string path, const LocationConfigs *location); //needs to be const ref
    ~Get();
};

#endif