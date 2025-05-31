#ifndef HEADERS_HPP
#define HEADERS_HPP

#include <map>
#include <string>
#include "iostream"
class Headers
{
    private:
        std::map<std::string, std::string> map;
        std::string buffer;

        void AddToMap(std::string line);
    public:
        void set_buffer(std::string buffer);
        std::string get_buffer();
        void HeadersParser();
};

#endif