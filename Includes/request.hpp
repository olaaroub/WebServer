#ifndef REQUEST_HPP
#define REQUEST_HPP

// #include "lib.hpp"
#include "Headers.hpp"
#include "RequestLine.hpp"
#include <fstream>
class Request
{
    private:
        Headers Headers;
        RequestLine RequestLine;

        std::string buffer;
        short state;
        bool request_ended;

        void is_finished();
        void ParsRequstLine();
        void ParsHeaders();
        void ParsBody(int socket_fd);
        void StateOFParser(int socket_fd);
    public:
        bool run_parser(int socket_fd);

};

#endif