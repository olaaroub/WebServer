#ifndef DELETE_HPP
#define DELETE_HPP

// #include "network.hpp"
// #include "request.hpp"
// #include "client.hpp"
#include "lib.hpp"
#include "Methods.hpp"

class Delete
{
private:
    std::string path;
    const LocationConfigs *location;

public:
    bool is_a_file;
    bool is_a_dir;
    int check_path();
    int check_dir(struct stat file_info);
    int check_file(struct stat file_info);
    std::string get_final_path();
    int delete_file();
    Delete(std::string path, const LocationConfigs *location);
    ~Delete();
};

#endif