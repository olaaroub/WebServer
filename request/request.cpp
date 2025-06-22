#include "request.hpp"


void Request:: is_finished()
{
    // if (buffer.find("\r\n\r\n") != std::string::npos)
    //     request_ended = true;
    // else
    //     request_ended = false;
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

void Request:: ParsBody(int socket_fd)
{
    std::fstream body("body.bin", std::ios::binary | std::ios::out);
    if (!body.is_open())
        throw std::string("ERROR: file not open!");
    body << buffer;
    if (!Headers.map["Content-Length"].empty())
    {
        // std::cout << "Dddd\n";
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
    std::cout << "is here\n";
    // std::cout << buffer << std::endl;
    StateOFParser(socket_fd);
    is_finished();
    return request_ended;
}