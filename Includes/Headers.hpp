#ifndef HEADERS_HPP
#define HEADERS_HPP
#include "lib.hpp"

class Headers
{
    private:
        std::string buffer;

        // std::string Body_key;
        void AddToMap(std::string line);
        void to_lower(std::string &string);
    public:
        void set_buffer(std::string buffer);
        std::string get_buffer();
        void HeadersParser();


        std::map<std::string, std::vector<std::string> > map;
};

#endif