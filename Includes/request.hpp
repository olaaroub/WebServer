#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Headers.hpp"
#include "RequestLine.hpp"

// class ParseError : public std::exception
// {
//     private:
//         std::string _Error;
//         short _ErrorStute;
//     public:
//         ParseError(std::string Error, short stute) : _Error(Error), _ErrorStute(stute){}
//         short getStutError() const {return _ErrorStute;}
//         const char* what() const throw()
//         {
//             return _Error.c_str();
//         }
// };

class Request
{
private:
    std::string buffer;
    std::string file_name;

    void is_finished();
    void is_number(std::string string);
    void ParsRequstLine();
    void ParsHeaders();
    void ParsBody();
    void StateOFParser();
    void ChunkReaContent();
    void ContentLenghtRead();

    
    unsigned long _chunkSize;
    unsigned long _contentSize;
    bool _waiting_for_new_chunk;
    
    enum ParserStute
    {
        _InRequestLine,
        _InHeaders,
        _InPost
    };
    public:
        bool run_parser(int socket_fd);
        Headers headers;
        RequestLine requestLine;
        short state;
        bool request_ended;
        unsigned long long max_body_size;
        
        // enum ErrorStute
        // {
        //     noError = 0,
        //     badRequest = 400,
        //     ServerError = 500
        // };

        std::stringstream body_content;
        std::fstream *file;
        Request() : _chunkSize(0), _contentSize(0),  _waiting_for_new_chunk(true),  state(_InRequestLine),request_ended(false) {}
        ~Request() {}
};

#endif