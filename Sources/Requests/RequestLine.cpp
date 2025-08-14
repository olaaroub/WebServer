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



void RequestLine:: SeparateUrlAndQuerys(std::string buff)
{
    size_t FindQuestionMark = buff.find("?");
    if (FindQuestionMark == std::string::npos)
    {
        _url = buff;
        return ;
    }
    _url = buff.substr(0, FindQuestionMark);
    queryLine = buff.erase(0, ++FindQuestionMark);

}

void RequestLine:: ParsRequestLine()
{
    std::string buff;
    std::stringstream str(line);
    str >> method;
    str >> buff;
    SeparateUrlAndQuerys(buff);
    str >> HttpVerction;
    if (HttpVerction != "HTTP/1.1")
        throw std::runtime_error("RequestLine Error: verstion of HTTP not seported!");
    if (method != "POST" && method != "GET" && method != "DELETE")
        throw std::runtime_error("RequestLine ERROR: this method not allowed");
}

const std::string& RequestLine:: get_method() const
{
    return method;
}