#ifndef RESPONSE_HPP
#define RESPONSE_HPP

// #include "network.hpp"
// #include "request.hpp"
// #include "client.hpp"
#include "lib.hpp"

class response
{
private:
    std::string getMimeType(const std::string &filePath);
    std::string get_response(long size, std::string file_path, std::string body, std::string status_line);
    std::string get_body(std::string file_path);
    void send_response(int socket_fd, std::string response);
    long getFileSize(const std::string &path);
    std::string get_statusLine(std::string type_res);
    std::string create_path_error(std::string type_error);

public:
    response(int socket_fd, std::string type_res, std::string final_path); // Get
    ~response();
};

#endif