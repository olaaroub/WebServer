#ifndef REQUESTLINE_HPP
#define REQUESTLINE_HPP

#include "lib.hpp"

class RequestLine
{
    private:
        std::string method;
        std::string HttpVerction;

        std::string line;
        std::string _url;

        void SeparateUrlAndQuerys(std::string buff);;
    public:
        void ParsRequestLine();
        void set_line(std::string line);
        std::string &get_line();
        std::string get_method();
        const std::string& getUrl()const;
        std::string Query_lien;
};

#endif