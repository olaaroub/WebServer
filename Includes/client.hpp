#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "network.hpp"
#include "request.hpp"
#include "Methods.hpp"


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

class client : public network
{
    private:
        Request request;
        time_t lastActivity;
        short _errorStute;
        // unsigned long long _maxBodyBytes;
        // const LocationConfigs *findLocation(const std::string &uri);

        void _convertMaxBodySize();

    public:
        void sendResponseString(const std::string& response);
        const LocationConfigs *findLocation(const std::string &uri);

        void handleHttpError(int statusCode);
        // void sendErrorResponse(int statusCode, const std::string& reasonPhrase);
        client(const ServerConfigs &server_config);

        void epoll_modify();
        void onEvent();

        void set_fd(int fd)
        {
            socket_fd = fd;
        }
        Request &get_request()
        {
            return request;
        }
        long get_max_body()
        {
            return server_config.client_max_body_size;
        }
        ~client();
};

#endif