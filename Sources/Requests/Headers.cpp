#include "Headers.hpp"

void Headers:: set_buffer(std::string buffer)
{
    this->buffer += buffer;
}
std::string Headers:: get_buffer()
{
    return buffer;
}

void Headers:: to_lower(std::string &string)
{
    std::string buf = string;
    for(size_t i = 0;i < buf.length();i++)
        string[i] = tolower(buf[i]);
}

void Headers:: AddToMap(std::string line)
{
    size_t cont = line.find(":");
    if (cont == std::string::npos)
        throw std::runtime_error("Headers Error: format not exist");
    std::string key = line.substr(0, cont);
    std::string value = line.substr(cont + 2);
    if (key.empty() || value.empty())
        throw std::runtime_error("Headers Error: format not exist");
    for (size_t i = 0; i < key.length(); i++)
        if (key[i] == ' ' || !isprint(key[i]) || !isascii(key[i]))
            throw std::runtime_error("Headers Error: Bad request: space in key");
    to_lower(key);
    // std::cout<< key << " " << value << std::endl;
    // exit(0);
    map[key].push_back(value);
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

    if (map["host"].size() > 1 || map["content-length"].size() > 1
        || map["content-type"].size() > 1 || map["authorization"].size() > 1
            || map["transfer-encoding"].size() > 1)
        throw std::runtime_error("Headers Error: duplicate headers");
    if (map["host"].empty())
        throw std::runtime_error("Headers Error: host not found");
    if (!map["content-length"].empty() && !map["transfer-encoding"].empty())
        throw std::runtime_error("Headers Error: content-length and transfer-encoding not allowed together");
    
    // for (std::map<std::string, std::vector<std::string> >::iterator it = map.begin(); it != map.end(); it++)
    // {
    //     std::cout << "first: '" << it->first << "' second: '" << it->second.at(0) << "'" << std::endl;
    // }
    // exit(1);

}

/*

5\r\n
rredf\r\n
0\r\n
\r\n

*/