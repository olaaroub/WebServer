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

        std::string baffer;
        short stute;
    public:
        void run_parser(int socket_fd);
};

#endif