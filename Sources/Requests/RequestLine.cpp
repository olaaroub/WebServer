#include "RequestLine.hpp"

void RequestLine:: set_line(std::string line)
{
    this->line += line;
}

std::string& RequestLine:: get_line()
{
    return line;
}

const std::string &RequestLine::getUrl()const{ return _url; }

void RequestLine:: SeparateMethod()
{
    size_t cont = line.find(" ");
    if (cont == std::string::npos)
        throw std::runtime_error("RequestLine ERROR: method not found");
    method = line.substr(0, cont);
    line = line.substr(++cont);
    if (method != "POST" && method != "GET" && method != "DELETE")
        throw std::runtime_error("RequestLine ERROR: this method not allowed");
}

void RequestLine:: SeparateQuerys()
{
    size_t FindIndex;

    FindIndex = line.find(" ");
    if (FindIndex == std::string::npos)
        throw std::runtime_error("RequestLine Error: Format not correct!");
    Query_lien = line.substr(0, FindIndex);
    line.erase(0, ++FindIndex);
}

void RequestLine:: SeparateUrl()
{
    size_t cont = line.find("?");
    size_t cont1 = line.find(" ");
    if (cont1 == std::string::npos)
        throw std::runtime_error("RequestLine ERROR: url error");
    if (cont != std::string::npos)
    {
        _url = line.substr(0, cont);
        line = line.substr(++cont);
        SeparateQuerys();
    }
    else
    {
        _url = line.substr(0, cont1);
        line = line.substr(++cont1);
    }
    HttpVerction = line;
    if (HttpVerction != "HTTP/1.1")
        throw std::runtime_error("RequestLine Error: verstion of HTTP not seported!");
}

void RequestLine:: ParsRequestLine()
{
    SeparateMethod();
    SeparateUrl();
    // std::cout << " method: '" << method << "' url: '" << url << "' httpv: '" << HttpVerction << "'" << std::endl;
}

std::string RequestLine:: get_method()
{
    return method;
}