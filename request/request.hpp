#ifndef REQUEST_HPP
#define REQUEST_HPP

// #include "../lib.hpp"
#include "Headers.hpp"
#include "RequestLine.hpp"
#include "Body.hpp"

class Request
{
    private:
        Headers Headers;
        RequestLine RequestLine;
        Body Body;

        // std::string baffer;
        short stute;
        bool request_ended;

        void is_finished(std::string baffer);
    public:
        bool run_parser(int socket_fd);
};

#endif