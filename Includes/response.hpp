#ifndef RESPONSE_HPP
#define RESPONSE_HPP

// #include "network.hpp"
// #include "request.hpp"
// #include "client.hpp"
#include "lib.hpp"


class response
{
    private : 
    std::string getMimeType(const std::string& filePath);
    std::string get_response(long size, std::string file_path, std::string body, std::string status_line);
    std::string get_body(std::string file_path);
    void send_response(int socket_fd, std::string response);
    long getFileSize(const std::string& path);

    public:
        response(int socket_fd, std::string path_file, std::string status_line);
        ~response();
};

#endif