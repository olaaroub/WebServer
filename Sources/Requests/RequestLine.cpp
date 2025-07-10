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
        throw std::string("REQUEST ERROR: method not found");
    method = line.substr(0, cont);
    line = line.substr(++cont);
    if (method != "POST" && method != "GET" && method != "DELETE")
        throw std::string("ERROR: this method not allowed");
}

void RequestLine:: SeparateQuerys()
{
    size_t cont;
    while (true)
    {
        cont = line.find("&");
        if (cont == std::string::npos)
            break;
        Query_parameters.push_back(line.substr(0, cont));
        line = line.substr(++cont);
    }
    cont = line.find(" ");
    Query_parameters.push_back(line.substr(0, cont));
    line = line.substr(++cont);
    // for(int i = 0;i < Query_parameters.size(); i++)
    //     std::cout << Query_parameters.at(i) << std::endl;
}

void RequestLine:: SeparateUrl()
{
    size_t cont = line.find("?");
    size_t cont1 = line.find(" ");
    if (cont1 == std::string::npos)
        throw std::string("REQUEST ERROR: url error");
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
        throw std::string("ERROR: verstion of HTTP not seported");
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