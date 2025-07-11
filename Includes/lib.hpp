#ifndef LIB_HPP
#define LIB_HPP

#define ERROR_PAGES "error_page"
#define LISTEN "listen"
#define CLIENT_MAX_BODY_SIZE "client_max_body_size"
#define SERVER_NAME "server_name"
#define LOCATION "location"
#define GET "GET"
#define POST "POST"
#define DELETE "DELETE"

#define MAX_EPOLL 100

#define red  "\033[31m"
#define reset  "\033[0m"
#define green "\033[32m"

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <exception>
#include <vector>
#include <fcntl.h>
#include <sys/epoll.h>
#include <algorithm>
#include <map>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <set>
#include <cstdlib>
#include <sys/stat.h>


#endif