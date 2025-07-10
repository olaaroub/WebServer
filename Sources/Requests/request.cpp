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


void Request:: ChunkReaContent()
{

    static unsigned int current_chunk_size = 0;
    static bool waiting_for_new_chunk = true;
    unsigned int len;
    while (true)
    {
        if (waiting_for_new_chunk)
        {
            size_t findNewLine = buffer.find("\r\n");
            if (findNewLine == std::string::npos)
                throw std::string("ERROR: structur of chunked POST not correct");
            std::string line = buffer.substr(0, findNewLine);
            is_number(line);
            std::istringstream ff(line);
            ff >> std::hex >> len;
            current_chunk_size = len;
            buffer.erase(0, findNewLine + 2);
            if (!len)
            {
                request_ended = true;
                return ;
            }
            waiting_for_new_chunk = false;
        }
        if (buffer.length() < current_chunk_size + 2)
            return;
        std::string chunkData = buffer.substr(0, current_chunk_size);
        body_content.write(chunkData.c_str(), current_chunk_size);
        buffer.erase(0, current_chunk_size + 2);
        waiting_for_new_chunk = true;
        current_chunk_size = 0;
    }
}

void Request:: is_number(std::string string)
{
    for (size_t i = 0; i < string.length(); i++)
    {
        if (!std::isxdigit(string[i]))
            throw std::string("ERROR: length not number");
    }
}

void Request:: ContentLenghtRead(int socket_fd)
{
    long cont;
    std::string number;

    number = Headers.map["content-length"].at(0);
    is_number(number);
    cont = atol(number.c_str());
    cont -= buffer.size();
    if (cont < 0)
        throw std::string("ERROR: bad request");
    else if (cont > 0)
    {
        char buf[cont];
        int read_cont = read(socket_fd, &buf, cont - 1);
        if (read_cont < 0)
            throw std::string("ERROR: read failed");
        buffer.append(buf, read_cont);

    }
    body_content << buffer;
    request_ended = true;
}

void Request:: ParsBody(int socket_fd)
{
    std::stringstream ss;
    ss << socket_fd;
    ss >> file_name;
    file_name += "_POST_FILE";
    if (!Headers.map["content-length"].empty() && Headers.map["transfer-encoding"].empty())
        ContentLenghtRead(socket_fd);
    else if (!Headers.map["transfer-encoding"].empty() && Headers.map["transfer-encoding"].at(0) == "chunked" && Headers.map["content-length"].empty())
        ChunkReaContent();
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
    // std::string baff;
    int cont = read(socket_fd, &bfr, 1023);
    if (cont <= 0)
    {
        close(socket_fd);
        throw std::runtime_error("Request parser Error: read failed!");
    }
    buffer.append(bfr, cont);
    // buffer = baff;
    std::cout << "'" << buffer << "'" << std::endl;
    StateOFParser(socket_fd);
    // std::cout << RequestLine.url << std::endl;
    return request_ended;
}