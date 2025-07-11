#ifndef METHODS_HPP
#define METHODS_HPP

#include "network.hpp"
#include "request.hpp"
#include "client.hpp"
#include "lib.hpp"



class Methods
{
    private:
    const LocationConfigs* location;
    public:
    Methods();
    Methods(const LocationConfigs* location);
    ~Methods();
};

#endif