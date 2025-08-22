#include "RequestLine.hpp"
#include "Utils.hpp"

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
    if (method != "POST" && method != "GET" && method != "DELETE")
        throw ParseError("RequestLine Error: method not implemented", methodNotImplemented);
}

const std::string& RequestLine:: get_method() const
{
    return method;
}