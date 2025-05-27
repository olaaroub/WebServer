#include "request.hpp"

void Request:: is_finished(std::string baffer)
{
    if (baffer.find("\r\n\r\n") != std::string::npos)
        request_ended = true;
    else
        request_ended = false;

}

bool Request:: run_parser(int socket_fd)
{
    char bfr[1024];
    std::string baffer;

    int cont = read(socket_fd, &bfr, 1023);
    if (cont <= 0)
    {
        close(socket_fd);
        throw std::string("ERROR: read failed");
    }
    baffer.append(bfr, cont);
    is_finished(baffer);
    std::cout << baffer  << "request ended : " << request_ended << std::endl;
    return request_ended;
}