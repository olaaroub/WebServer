#include "Headers.hpp"

void Headers:: set_buffer(std::string buffer)
{
    this->buffer += buffer;
}
std::string Headers:: get_buffer()
{
    return buffer;
}

void Headers:: AddToMap(std::string line)
{
    size_t cont = line.find(":");
    if (cont == std::string::npos)
        throw std::string("ERROR: header error");
    std::string key = line.substr(0, cont);
    std::string value = line.substr(cont + 2);
    map[key] = value;
}

void Headers:: HeadersParser()
{
    size_t cont;

    while (true)
    {
        cont = buffer.find("\r\n");
        if (cont == std::string::npos)
            break;
        AddToMap(buffer.substr(0, cont));
        buffer = buffer.substr(cont + 2);
    }
    // for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); it++)
    // {
    //     std::cout << "first: '" << it->first << "' second: '" << it->second << "'" << std::endl;
    // }
    // exit(1);
    
}

/*

5\r\n
rredf\r\n
0\r\n
\r\n

*/