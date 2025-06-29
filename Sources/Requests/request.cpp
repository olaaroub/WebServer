#include "request.hpp"
#include <stdlib.h>
#include <unistd.h>

void Request:: is_finished()
{
    if (state == 2)
        request_ended = true;
    else
        request_ended = false;
}

void Request:: ParsRequstLine()
{
    size_t cont = buffer.find("\r\n");
    if (cont != std::string::npos)
    {
        RequestLine.set_line(buffer.substr(0, cont));
        RequestLine.ParsRequestLine();
        buffer = buffer.substr(cont+2);
        state++;
    }
    else
        RequestLine.set_line(buffer);
    
}

void Request:: ParsHeaders()
{
    size_t cont = buffer.find("\r\n\r\n");
    if (cont != std::string::npos)
    {
        Headers.set_buffer(buffer.substr(0, cont + 2));
        buffer = buffer.substr(cont + 4);
        Headers.HeadersParser();
        if (RequestLine.get_method() != "POST")
            request_ended = true;
        else
            request_ended = false;
        state++;
    }
    else
        Headers.set_buffer(buffer);
}
#include <sstream>

void Request:: ChunkReaContent(std::fstream &body, int socket_fd)
{
    while (!buffer.empty())
    {
        unsigned int len;
        size_t findNewLine = buffer.find("\r\n");
        std::cout << "'" << buffer << "'" << std::endl;
        if (findNewLine == std::string::npos)
            throw std::string("ERROR: structur of chunked POST not correct");
        std::string len_str = buffer.substr(0, findNewLine);
        std::istringstream ff(len_str);
        ff >> std::hex >> len;
        std::cout << "'" << len << "'" << std::endl;
        if (!len)
        {
            request_ended = true;
            return ;
        }
        buffer = buffer.substr(findNewLine + 2);
        if (buffer.length() < len)
        {
            int cont = len - buffer.length();
            char read_buffer[cont];
            int readlen = read(socket_fd, &read_buffer, cont);
            if (readlen < 0)
                throw std::string("ERROR");
            buffer.append(read_buffer, cont - 1);
            body << buffer;
            buffer = "";
        }
        else
        {
            findNewLine = buffer.find("\r\n");
            if (findNewLine == std::string::npos)
                throw std::string("ERROR: structur of chunked POST not correct");
            std::string content = buffer.substr(0, findNewLine);
            buffer = buffer.substr(findNewLine + 2);
            std::cout << "len: " << len << " " << content.length() << std::endl;
            if (static_cast<size_t>(len) != content.length())
                throw std::string("Bad Request");
            body << content;
        }
        
    }
}

void Request:: ContentLenghtRead(std::fstream &body, int socket_fd)
{
    body << buffer;
    int cont = atoi(Headers.map["Content-Length"].c_str());
    cont -= buffer.size() - 1;
    if (cont - 1 == 0)
    {
        request_ended = true;
        return;
    }
    if (cont < 0)
        throw std::string("ERROR: !");
    char buf[cont];
    int read_cont = read(socket_fd, &buf, cont - 1);
    if (cont <= 0)
    {
        close(socket_fd);
        throw std::string("ERROR: read failed");
    }
    buffer.append(buf, read_cont);
    body << buffer;
    request_ended = true;
}

void Request:: ParsBody(int socket_fd)
{
    std::stringstream ss;
    ss << socket_fd;
    ss >> file_name;
    file_name += "_POST_FILE";
    std::fstream body(file_name.c_str(), std::ios::binary | std::ios::out);
    if (!body.is_open())
        throw std::string("ERROR: file not open!");
    file = &body;
    if (!Headers.map["Content-Length"].empty())
        ContentLenghtRead(body, socket_fd);
    else if (!Headers.map["Transfer-Encoding"].empty() && Headers.map["Transfer-Encoding"] == "chunked")
        ChunkReaContent(body, socket_fd);
}

void Request:: StateOFParser(int socket_fd)
{
    if (state == 0)
        ParsRequstLine();
    if (state == 1)
        ParsHeaders();
    if (state == 2 && RequestLine.get_method() == "POST")
        ParsBody(socket_fd);

}

bool Request:: run_parser(int socket_fd)
{
    char bfr[1024];
    std::string baff;
    state = 0;

    int cont = read(socket_fd, &bfr, 1023);
    if (cont <= 0)
    {
        close(socket_fd);
        throw std::string("ERROR: read failed");
    }
    baff.append(bfr, cont);
    buffer = baff;
    // std::cout << "'" << buffer << "'" << std::endl;
    StateOFParser(socket_fd);
    return request_ended;
}