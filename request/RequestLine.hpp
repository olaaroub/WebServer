#ifndef REQUESTLINE_HPP
#define REQUESTLINE_HPP

#include <string>
#include <vector>

class RequestLine
{
    private:
        std::string method;
        std::string url;
        std::string HttpVerction;
        std::vector<std::string> Query_parameters;
};

#endif