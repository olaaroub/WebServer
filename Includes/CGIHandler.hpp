#pragma once

#include "lib.hpp"
#include "request.hpp"
#include "Configs.hpp"

class CgiHandler {
private:

    Request&    _request;
    const LocationConfigs& _location;
    std::string _scriptPath;

public:
    CgiHandler(Request& req, const LocationConfigs& loc, const std::string& path);
    ~CgiHandler();


    void execute();

};