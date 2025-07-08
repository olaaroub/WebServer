#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Headers.hpp"
#include "RequestLine.hpp"
class Request
{
    private:
        Headers Headers;
        RequestLine RequestLine;

        std::string buffer;
        std::string file_name;
        bool request_ended;

        void is_finished();
        void is_number(std::string string);
        void ParsRequstLine();
        void ParsHeaders();
        void ParsBody(int socket_fd);
        void StateOFParser(int socket_fd);
        void ChunkReaContent(std::fstream &body, int socket_fd);
        void ContentLenghtRead(std::fstream &body, int socket_fd);
    public:
        bool run_parser(int socket_fd);
        short state;
        
        std::fstream *file;
};

#endif