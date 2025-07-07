#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "network.hpp"
#include "request.hpp"

class client : public network
{
    private:
        Request request;

    public:
        client(const ServerConfigs& server_config);
        // client(int kernel_id);

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