#include "RequestLine.hpp"

void RequestLine:: set_line(std::string line)
{
    this->line += line; 
}

std::string& RequestLine:: get_line()
{
    return line;
}

void RequestLine:: SeparateMethod()
{
    int cont = line.find(" ");
    if (cont == std::string::npos)
        throw std::string("REQUEST ERROR: method not found");
    method = line.substr(0, cont);
    line = line.substr(++cont);
}

void RequestLine:: SeparateQuerys()
{
    int cont;
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
    int cont = line.find("?");
    int cont1 = line.find(" ");
    if (cont1 == std::string::npos)
        throw std::string("REQUEST ERROR: url error");
    if (cont != std::string::npos)
    {
        url = line.substr(0, cont);
        line = line.substr(++cont);
        SeparateQuerys();
    }
    else
    {
        url = line.substr(0, cont1);
        line = line.substr(++cont1);
    }
    HttpVerction = line;
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