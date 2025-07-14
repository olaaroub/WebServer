#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Headers.hpp"
#include "RequestLine.hpp"
class Request
{
private:
    std::string buffer;
    std::string file_name;

    void is_finished();
    void is_number(std::string string);
    void ParsRequstLine();
    void ParsHeaders();
    void ParsBody();
    void StateOFParser();
    void ChunkReaContent();
    void ContentLenghtRead();
    void SetMaxBodySize();

public:
    bool run_parser(int socket_fd);
    Headers headers;
    RequestLine RequestLine;
    short state;
    bool request_ended;
    long long max_body_size;

    std::stringstream body_content;
    std::fstream *file;
};

#endif