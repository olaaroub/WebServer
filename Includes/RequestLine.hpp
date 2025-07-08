#ifndef REQUESTLINE_HPP
#define REQUESTLINE_HPP

#include "lib.hpp"

class RequestLine
{
    private:
        std::string method;
        std::string url;
        std::string HttpVerction;
        std::vector<std::string> Query_parameters;
        std::string line;

        void SeparateMethod();
        void SeparateUrl();
        void SeparateQuerys();
    public:
        void set_line(std::string line);
        std::string &get_line();
        void ParsRequestLine();
        std::string get_method();
        std::string get_url();
        std::string get_httpversion();
};

#endif