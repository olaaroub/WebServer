#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "network.hpp"

class client : public network
{
    private:
        char request_str[1024];
    public:
        client();
        client(int kernel_id);

        void epoll_modify();
        void onEvent(std::map<int, network *> &infos);
        void set_fd(int fd)
        {
            socket_fd = fd;
        }
};

#endif