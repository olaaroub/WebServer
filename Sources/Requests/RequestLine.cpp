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
    std::stringstream str(line);
    str >> method;
    str >> _url;
    str >> HttpVerction;
    std::cout << "line: "<< line << "' " << method << " " << _url << " " << HttpVerction << std::endl;
    // size_t cont = line.find(" ");
    // if (cont == std::string::npos)
    //     throw std::runtime_error("RequestLine ERROR: method not found");
    // method = line.substr(0, cont);
    // line = line.substr(++cont);
    // if (method != "POST" && method != "GET" && method != "DELETE")
    //     throw std::runtime_error("RequestLine ERROR: this method not allowed");
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

void RequestLine:: SeparateUrlAndQuerys(std::string buff)
{
    size_t FindQuestionMark = buff.find("?");
    if (FindQuestionMark == std::string::npos)
    {
        _url = buff;
        return ;
    }
    _url = buff.substr(0, FindQuestionMark);
    Query_lien = buff.erase(0, ++FindQuestionMark);

}

void RequestLine:: ParsRequestLine()
{
    std::string buff;
    std::stringstream str(line);
    str >> method;
    str >> buff;
    SeparateUrlAndQuerys(buff);
    str >> HttpVerction;
    std::cout << "method " << method << " HttpVerction: " << HttpVerction << " URL: "<< _url << std::endl;
    if (HttpVerction != "HTTP/1.1")
        throw std::runtime_error("RequestLine Error: verstion of HTTP not seported!");
}

std::string RequestLine:: get_method()
{
    return method;
}