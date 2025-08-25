#ifndef HEADERS_HPP
#define HEADERS_HPP
#include "lib.hpp"
#include "Utils.hpp"

// class ParseError : public std::exception
// {
//     private:
//         std::string _Error;
//     public:
//         short ErrorStute;
//         ParseError(std::string Error, short stute) : _Error(Error), ErrorStute(stute){}
//         short getStutError() const {return ErrorStute;}
//         const char* what() const throw()
//         {
//             return _Error.c_str();
//         }
//         ~ParseError() throw() {}
// };

class Headers
{
    private:
        std::string buffer;

        // std::string Body_key;
        void AddToMap(std::string line);
        // std::string toLower(const std::string &string) const;

        void _splitCookie(std::string cookie);
        std::string _percentEncoding(std::string cookie);
        int _toDecimal(std::string::iterator begin, std::string::iterator end);
        bool _isValidHeaderKey(const std::string& key);
    public:
        std::string getCookie(const std::string& key) const;

        void set_buffer(std::string buffer);
        std::string get_buffer();
        void HeadersParser();

        void cookieParser();

        std::map<std::string, std::vector<std::string> > map;
        std::map<std::string, std::string> cookieInfo;
};

#endif