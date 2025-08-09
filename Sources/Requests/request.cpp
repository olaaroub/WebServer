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

    if (buffer.size() > MAX_REQUESTLINE_SIZE)
        throw std::runtime_error("Request parser Error: Request Line Size too Large!");
    if (cont != std::string::npos)
    {
        requestLine.set_line(buffer.substr(0, cont));
        requestLine.ParsRequestLine();
        buffer.erase(0, cont+2);
        state++;
    }

}

void Request:: ParsHeaders()
{
    size_t cont = buffer.find("\r\n\r\n");

    if (buffer.size() > MAX_HEADERS_SIZE)
        throw std::runtime_error("Request parser Error: Headers Size too Large!");
    if (cont != std::string::npos)
    {

        headers.set_buffer(buffer.substr(0, cont + 2));
        buffer = buffer.substr(cont + 4);
        headers.HeadersParser();
        if (requestLine.get_method() != "POST")
            request_ended = true;
        else
            request_ended = false;

        state++;
    }

}

void Request:: ChunkReaContent()
{
    unsigned int len;
    while (true)
    {
        if (buffer.empty())
            return ;
        if (_waiting_for_new_chunk)
        {
            size_t findNewLine = buffer.find("\r\n");
            if (findNewLine == std::string::npos)
                throw std::runtime_error("Request parser Error: format of chunked POST not correct");
            std::string line = buffer.substr(0, findNewLine);
            is_number(line);
            std::istringstream ff(line);
            ff >> std::hex >> len;
            _chunkSize = len;
            buffer.erase(0, findNewLine + 2);
            if (!len)
            {
                request_ended = true;
                return ;
            }
            _waiting_for_new_chunk = false;
        }
        if (buffer.length() < _chunkSize + 2)
            return;
        std::string chunkData = buffer.substr(0, _chunkSize);
        if (_chunkSize > max_body_size)
            throw std::runtime_error("ERROR: Request: Body too large!");
        max_body_size -= _chunkSize;
        body_content.write(chunkData.c_str(), _chunkSize);
        buffer.erase(0, _chunkSize + 2);
        _waiting_for_new_chunk = true;
        _chunkSize = 0;
    }
}

void Request:: is_number(std::string string)
{
    for (size_t i = 0; i < string.length(); i++)
    {
        if (!std::isxdigit(string[i]) || string[i] == '-')
            throw std::runtime_error("Request parser Error: size of data passed does not  number!");
    }
}

void Request:: ContentLenghtRead()
{
    // static long long ContentSize = 0;
    std::string number;

    if (buffer.empty())
        return;
    if (!_chunkSize)
    {
        number = headers.map["content-length"].at(0);
        is_number(number);
        _chunkSize = atol(number.c_str());
        // if (_chunkSize < 0)
            // throw std::runtime_error("Request parser Error:bad request!"); // ach had l9lawwi wach tatsna unsigned long ikon 9el mn 0 ?????
                                                                            // walayni rak 7mar
        if (_chunkSize > max_body_size)
            throw std::runtime_error("Request parser Error: the content lenght too large!");
    }
    if (_contentSize < _chunkSize)
        _contentSize += buffer.size();
    if (_contentSize > _chunkSize)
        throw std::runtime_error("Request Error: ContentLenghtRead Error: size of content readed large of content-lenght");
    if (_contentSize == _chunkSize)
    {
        std::cout << "request ended" << std::endl;
        request_ended = true;
    }
    body_content.write(buffer.c_str(), buffer.length());
    buffer.clear();
}

void Request:: ParsBody()
{
    if (!headers.map["content-length"].empty() && headers.map["transfer-encoding"].empty())
        ContentLenghtRead();
    else if (!headers.map["transfer-encoding"].empty() && headers.map["transfer-encoding"].at(0) == "chunked" && headers.map["content-length"].empty())
        ChunkReaContent();
    else
        throw std::runtime_error("Request parser Error: this method to transfer data not allowed!");
}

void Request:: StateOFParser()
{
    if (state == 0)
        ParsRequstLine();
    if (state == 1)
        ParsHeaders();
    if (state == 2 && requestLine.get_method() == "POST")
        ParsBody();

}

bool Request:: run_parser(int socket_fd)
{
    char bfr[1024];

    int cont = read(socket_fd, &bfr, 1023);
    if (cont < 0)
        throw std::runtime_error("Request parser Error: read failed!");
    if (cont == 0)
        throw std::runtime_error("Request parser: Client closed connection unexpectedly");
    buffer.append(bfr, cont);
    StateOFParser();
    return request_ended;
}