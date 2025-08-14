#ifndef HEADERS_HPP
#define HEADERS_HPP
#include "lib.hpp"

class ParseError : public std::exception
{
    private:
        std::string _Error;
        short _ErrorStute;
    public:
        ParseError(std::string Error, short stute) : _Error(Error), _ErrorStute(stute){}
        short getStutError() const {return _ErrorStute;}
        const char* what() const throw()
        {
            return _Error.c_str();
        }
        ~ParseError() throw() {}
};

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