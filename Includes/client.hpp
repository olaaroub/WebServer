#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "network.hpp"
#include "request.hpp"
#include "Methods.hpp"

class client : public network
{
    private:
        Request request;
        time_t lastActivity;
        short _errorStute;
        void _convertMaxBodySize();

        enum ClientState {
            READING,
            WRITING
        };

        ClientState _state;
        std::string _response_buffer;
        size_t _bytes_sent;

        void _handleWrite();
        bool _is_monitored;

    public:

        void prepareResponse(const std::string& response);

        void sendResponseString(const std::string& response);
        const LocationConfigs *findLocation(const std::string &uri);

        void handleHttpError(int statusCode);
        // void sendErrorResponse(int statusCode, const std::string& reasonPhrase);
        client(const ServerConfigs &server_config);

        void epoll_modify();
        void onEvent();

        void setMonitored(bool monitored);
        bool isMonitored() const;

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