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
    // std::string get_response(long size, std::string file_path, std::string body, std::string status_line);
    // std::string get_body(std::string file_path);
    void send_fullresponse(int socket_fd, long size, std::string file_path, std::string status_line);
    void send_header(int socket_fd , long size, std::string file_path, std::string status_line);
    void send_body(int socket_fd, std::string file_path);
    void send_chunk(int socket_fd, const char* data, size_t length);
    void send_string(int socket_fd, std::string response);

    long getFileSize(const std::string &path);
    std::string get_statusLine(std::string type_res);
    std::string create_path_error(std::string type_error);
    
public:
    response(int socket_fd, std::string type_res, std::string final_path); // Get
    response(int socket_fd, std::string location_file); // constructer for POST

    ~response();
};

#endif