#ifndef RESPONSE_HPP
#define RESPONSE_HPP

// #include "network.hpp"
// #include "request.hpp"
// #include "client.hpp"
#include "lib.hpp"


class response
{
    private : 
    std::string response::getMimeType(const std::string& filePath);
    std::string response::get_response(long size, std::string file_path, std::string body, std::string status_line);
    std::string response::get_body(std::string file_path);
    void response::send_response(int socket_fd, std::string response);
    long response::getFileSize(const std::string& path);

    public:
        response(int socket_fd, std::string error_fileName, std::string status_line);
        response(int socket_fd, std::string path_file, std::string status_line);
        ~response();
};

#endif