#include "Headers.hpp"
#include "Utils.hpp"

void Headers:: set_buffer(std::string buffer)
{
    this->buffer += buffer;
}
std::string Headers:: get_buffer()
{
    return buffer;
}

// std::string Headers::toLower(const std::string& str) const {
//     std::string result = str;
//     std::transform(result.begin(), result.end(), result.begin(), ::tolower);
//     // std::cout << GREEN << result << RESET << std::endl;
//         return result;
// }

std::string Headers::getCookie(const std::string& key) const {
    std::string lname = toLower(key);
    for (std::map<std::string, std::string>::const_iterator it = cookieInfo.begin();
         it != cookieInfo.end(); ++it) {
        if (toLower(it->first) == lname)
            return it->second;
    }
    return "";
}

bool Headers:: _isValidHeaderKey(const std::string& key) {
    if (key.empty())
        return false;

    std::string invalidHeaderKeyPrintable = "()<>@,;:\\\"/[]?={} \t";

    for (size_t i = 0; i < key.length(); ++i)
    {
        if (iscntrl(static_cast<unsigned char>(key[i])) || invalidHeaderKeyPrintable.find(key[i]) != std::string::npos)
            return false;
    }

    return true;
}

void Headers:: AddToMap(std::string line)
{
    size_t cont = line.find(":");
    if (cont == std::string::npos)
        throw ParseError("Headers Error: Invalid Header format", badRequest);
    std::string key = line.substr(0, cont);
    std::string value = line.substr(cont + 2);
    if (!_isValidHeaderKey(key))
        throw ParseError("Headers Error: Invalid Key", badRequest);
    for (size_t i = 0; i < value.size(); i++)
        if (!isprint(value[i]) || iscntrl(static_cast<unsigned char>(value[i])))
            throw ParseError("Headers Error: Invalid Header value", badRequest);
    key = toLower(key);
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
        throw ParseError("Headers Error: duplicate headers", badRequest);
    if (map["host"].empty())
        throw ParseError("Headers Error: host not found", badRequest);
    if (!map["content-length"].empty() && !map["transfer-encoding"].empty())
        throw ParseError("Headers Error: content-length and transfer-encoding not allowed together", badRequest);

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
        throw ParseError("Header Parser: cookieParser: wrong format", badRequest);
    std::string key = cookie.substr(0, findIndex++);
    std::string value = cookie.substr(findIndex, cookie.size() - findIndex);
    cookieInfo[key] = _percentEncoding(value);
    std::cout << MAGENTA << "Parsed Cookie: key='" << key << "', value='" << cookieInfo[key] << "'" << RESET << std::endl;
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