#ifndef HEADERS_HPP
#define HEADERS_HPP

#include <map>
#include <string>
#include "iostream"
class Headers
{
    private:
        std::string buffer;

        // std::string Body_key;
        void AddToMap(std::string line);
    public:
        void set_buffer(std::string buffer);
        std::string get_buffer();
        void HeadersParser();


        std::map<std::string, std::string> map;
};

#endif