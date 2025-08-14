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

        void _splitCookie(std::string cookie);
        std::string _percentEncoding(std::string cookie);
        int _toDecimal(std::string::iterator begin, std::string::iterator end);
    public:
        void set_buffer(std::string buffer);
        std::string get_buffer();
        void HeadersParser();

        void cookieParser();

        std::map<std::string, std::vector<std::string> > map;
        std::map<std::string, std::string> cookieInfo;
};

#endif