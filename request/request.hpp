#ifndef REQUEST_HPP
#define REQUEST_HPP

// #include "../lib.hpp"
#include "Headers.hpp"
#include "RequestLine.hpp"
#include "Body.hpp"
#include <fstream>
class Request
{
    private:
        Headers Headers;
        RequestLine RequestLine;
        Body Body;

        std::string buffer;
        short state;
        bool request_ended;

        void is_finished();
        void ParsRequstLine();
        void ParsHeaders();
        void ParsBody();
        void StateOFParser();
    public:
        bool run_parser(int socket_fd);

};

#endif