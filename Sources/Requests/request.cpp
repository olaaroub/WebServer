#include "request.hpp"

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
        // std::cout << "is here\n";
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

        // std::cout << buffer << std::endl;
        // exit(1);
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

void Request:: ChunkReaContent(std::fstream &body, int socket_fd)
{
    std::cout << "chunked" << std::endl;
    int is_content = 0;
    int readlen;
    unsigned int len;
    bool read_ended = false;
    while (true)
    {
        size_t findNewLine = buffer.find("\r\n");
        while (findNewLine == std::string::npos)
        {
            char buf[1024];
            readlen = read(socket_fd, &buf, 1023);
            if (!readlen)
            {
                read_ended = true;
                break;
            }
            if (readlen < 0)
                throw std::string("connection ended");
            buffer.append(buf, readlen);
            findNewLine = buffer.find("\r\n");
        }
        if (findNewLine == std::string::npos)
            throw std::string("ERROR: structur of chunked POST not correct");
        std::string line = buffer.substr(0, findNewLine);
        if (is_content % 2 == 0)
        {
            std::istringstream ff(line);
            ff >> std::hex >> len;
            buffer = buffer.substr(findNewLine + 2);
            if (!len)
            {
                request_ended = true;
                return ;
            }
            is_content++;
        }
        else
        {
            line = buffer.substr(0, len);
            buffer = buffer.substr(len + 2);
            body << line;
            is_content++;
        }
        if (read_ended)
            break;
    }
}

void Request:: is_number(std::string string)
{
    for (size_t i = 0; i < string.length(); i++)
    {
        if (!isdigit(string[i]))
            throw std::string("ERROR: length not number");
    }
}

void Request:: ContentLenghtRead(std::fstream &body, int socket_fd)
{
    int cont;
    std::string number;
    if (!Headers.map["Content-Length"].empty())
        number = Headers.map["Content-Length"];
    else
        number = Headers.map["content-length"];
    is_number(number);
    cont = atoi(number.c_str());
    cont -= buffer.size();
    if (cont < 0)
        throw std::string("ERROR: !");
    else if (cont > 0)
    {
        char buf[cont];
        int read_cont = read(socket_fd, &buf, cont - 1);
        if (read_cont < 0)
            throw std::string("ERROR: read failed");
        buffer.append(buf, read_cont);

    }
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
    if (!Headers.map["Content-Length"].empty() || !Headers.map["content-length"].empty())
        ContentLenghtRead(body, socket_fd);
    else if (!Headers.map["Transfer-Encoding"].empty() && Headers.map["Transfer-Encoding"] == "chunked")
        ChunkReaContent(body, socket_fd);
    else
        throw std::string("ERROR");
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

    int cont = read(socket_fd, &bfr, 1023);
    if (cont <= 0)
    {
        close(socket_fd);
        throw std::string("ERROR: read failed");
    }
    baff.append(bfr, cont);
    buffer = baff;
    std::cout << "'" << buffer << "'" << std::endl;
    StateOFParser(socket_fd);
    return request_ended;
}