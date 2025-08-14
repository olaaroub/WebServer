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
    // std::string buf = string;
    // for(size_t i = 0;i < buf.length();i++)
    //     string[i] = tolower(buf[i]);
    std::transform(string.begin(), string.end(), string.begin(), ::tolower);
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

int Headers:: _toDecimal(std::string::iterator begin, std::string::iterator end)
{
    int number = 0;
    std::string hexNb(begin, end);

    std::istringstream res(hexNb);
    res >> std::hex >> number;
    if (res.fail())
        return 0;
    return number;
}

std::string Headers:: _percentEncoding(std::string cookie)
{
    std::string value(cookie.begin(), cookie.end());

    for(size_t i = 0; i < value.size(); i++)
    {
        if (value[i] == '%' && i + 2 < value.size() && std::isxdigit(value[i + 1]) && std::isxdigit(value[i + 2]))
        {
            int res = _toDecimal(value.begin() + i + 1, value.begin() + i + 2 + 1);
            value[i] = static_cast<char>(res);
            value.erase(i + 1, 2);
        }
    }
    return value;
}

void Headers:: _splitCookie(std::string cookie)
{
    while (cookie[0] == ' ')
        cookie.erase(0, 1);
    while (cookie[cookie.size() - 1] == ' ')
        cookie.erase(cookie.size() - 1, 1);

    size_t findIndex = cookie.find("=");
    if (findIndex == std::string::npos)
        throw std::runtime_error("Header Parser: cookieParser: wrong format");
    std::string key = cookie.substr(0, findIndex++);
    std::string value = cookie.substr(findIndex, cookie.size() - findIndex);
    cookieInfo[key] = _percentEncoding(value);
    std::cout << "key: '" << key << "' value: '" << cookieInfo[key] << "'" << std::endl;
}

void Headers:: cookieParser()
{
    if (map["cookie"].empty())
        return ;
    std::string cookie = map["cookie"].at(0);
    while (!cookie.empty())
    {
        size_t findIndex = cookie.find(";");
        if (findIndex == std::string::npos)
        {
            _splitCookie(cookie);
            return ;
        }
        std::string firstCookie = cookie.substr(0, findIndex++);
        cookie.erase(0, findIndex);
        _splitCookie(firstCookie);
    }
}


/*

5\r\n
rredf\r\n
0\r\n
\r\n

*/