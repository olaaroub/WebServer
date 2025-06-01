#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <iostream>

class ConfigParser
{
    private:
        std::string FilePath;
    public:
        ConfigParser(std::string FileName) : FilePath(FileName) {}
        void run_parser() {}
};

#endif