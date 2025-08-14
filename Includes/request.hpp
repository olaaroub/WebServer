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


    unsigned long _chunkSize;
    unsigned long _contentSize;
    bool _waiting_for_new_chunk;

public:
    bool run_parser(int socket_fd);
    Headers headers;
    RequestLine requestLine;
    short state;
    bool request_ended;
    unsigned long long max_body_size;

    std::stringstream body_content;
    std::fstream *file;
    Request() : _chunkSize(0), _contentSize(0),  _waiting_for_new_chunk(true),  state(0),request_ended(false) {}
    ~Request() {}
};

#endif