#include "request.hpp"

void Request:: run_parser(int socket_fd)
{
    int cont;
    char bfr[1024];

    int cont = read(socket_fd, &bfr, 1023);
    if (cont < 0)
        throw std::string("ERROR: read failed");
    baffer.append(bfr, cont);
    
}