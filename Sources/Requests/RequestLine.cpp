#include "RequestLine.hpp"
#include "Utils.hpp"


std::string RequestLine:: getHttpVerction() {return _HttpVerction;}
void RequestLine:: set_line(std::string line){this->_line += line;}
std::string& RequestLine:: get_line(){return _line;}
const std::string &RequestLine::getUrl()const{ return _url; }
const std::string& RequestLine:: get_method() const{return _method;}


void RequestLine:: SeparateUrlAndQuerys(std::string buff)
{
    if (buff.size() > MAX_REQUESTLINE_SIZE)
        throw ParseError("Request Error: Request Line Size too Large!", uRLTooLarge);
    size_t FindQuestionMark = buff.find("?");
    if (FindQuestionMark == std::string::npos)
    {
        _url = buff;
        return ;
    }
    _url = buff.substr(0, FindQuestionMark);
    queryLine = buff.erase(0, ++FindQuestionMark);
    queryLine = uRLEncoding(queryLine);

}

void RequestLine:: ParsRequestLine()
{
    std::string buff;
    std::string forbidenChars = " \t{}|\\^[]`";
    std::stringstream str(_line);
    str >> _method;
    str >> buff;
    SeparateUrlAndQuerys(buff);
    str >> _HttpVerction;
    // std::cout << "'" << _url << "'" << std::endl;
    for (size_t i = 0; i < _url.size(); i++)
    {
        size_t index = forbidenChars.find(_url[i]);
        if (!isprint(_url[i]) || index != std::string::npos)
            throw ParseError("invalid characters in uri", badRequest);
    }
    _url = uRLEncoding(_url);
}
