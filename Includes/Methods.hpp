#ifndef METHODS_HPP
#define METHODS_HPP

// #include "network.hpp"
// #include "request.hpp"
#include "client.hpp"


class Methods
{
    private:
        int check_location(ServerConfigs serverconfigs, std::string uri);
        bool check_method(std::string method , std::vector<std::string> allowed_methods);
    public:
        int location; // -1 mean that location not found
        bool method;  // false mean , method not allowed


        Methods(ServerConfigs serverconfigs, Request request);
        ~Methods();
};

#endif