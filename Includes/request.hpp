#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Headers.hpp"
#include "RequestLine.hpp"
class Request
{
private:
    Headers Headers;
    std::string buffer;
    std::string file_name;

    void is_finished();
    void is_number(std::string string);
    void ParsRequstLine();
    void ParsHeaders();
    void ParsBody(int socket_fd);
    void StateOFParser(int socket_fd);
    void ChunkReaContent();
    void ContentLenghtRead(int socket_fd);

public:
    bool run_parser(int socket_fd);
    RequestLine RequestLine;
    short state;
    bool request_ended;
    long max_body_size;

    std::stringstream body_content;
    std::fstream *file;
};

#endif