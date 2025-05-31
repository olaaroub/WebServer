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

void Request:: ParsBody()
{
    std::fstream body("body.bin", std::ios::binary | std::ios::out);
    if (!body)
        throw std::string("ERROR: file not open!");
    body << buffer;
}

void Request:: StateOFParser()
{
    std::cout << state << std::endl;
    if (state == 0)
        ParsRequstLine();
    if (state == 1)
        ParsHeaders();
    if (state == 2 && RequestLine.get_method() == "POST")
        ParsBody();

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
    StateOFParser();
    // is_finished();
    // std::cout << "request line: '" << RequestLine.get_line() << "' Headers: '" << Headers.get_buffer() << "'\n";
    return request_ended;
}