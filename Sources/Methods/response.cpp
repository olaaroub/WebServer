#include "response.hpp"
#include "Utils.hpp"
// #include "client.hpp"

response::response(int socket_fd, const ServerConfigs &save_server_config) : server_config(save_server_config)
{
    this->socket_fd = socket_fd;
}

// response::response(int socket_fd, std::string type_res, std::string final_path) // constructer for GET
// {
//     std::string response;
//     if (type_res == "success")
//     {
//         send_fullresponse(socket_fd , getFileSize(final_path), final_path, get_statusLine(type_res));
//     }
//     else
//     {
//         std::string path_error = create_path_error(type_res);
//         send_fullresponse(socket_fd , getFileSize(path_error), path_error, get_statusLine(type_res));
//     }
// }

// response::response(int socket_fd, c) // constructer for POST
// {
//     std::stringstream response;
//     std::string location_responsefile = "./Pages/response.html"; // the file i will sent !
//     // // Status Line
//     response << "HTTP/1.1 201 Created";
//     // // Headers
//     response << "Content-Length: " << getFileSize(location_responsefile) << "\r\n";
//     response << "Content-Type: " << "text/html" << "\r\n";
//     response << "Location: " << location_file << "\r\n";

//     // // Blank line separating headers from body
//     response << "\r\n";
//     send_string(socket_fd, response.str());
//     send_body(socket_fd, location_responsefile);

// }

// void response::send_response_sucess(int socket_fd)
// {
//     std::stringstream response;
//     // // Status Line
//     response << "HTTP/1.1 204 No Content";
//     // // Blank line separating headers from body
//     response << "\r\n";

//     send_string(socket_fd, response.str());

// }

// --- THE NEW FUNCTION :



void response::send_res_autoindexFile(std::string file_content)
{
    std::stringstream response;
    // Status Line
    response << "HTTP/1.1 200 OK\r\n";
    // Headers
    response << "Content-Length: " << file_content.size() << "\r\n";
    response << "Content-Type: " << "text/html" << "\r\n";

    // Blank line separating headers from body
    response << "\r\n";
    send_string(socket_fd, response.str());
    send_string(socket_fd, file_content);
}

void response::get_response(std::string path_file, bool autoindex)
{
    if (autoindex == true) // mean autoindex is on . 
        send_res_autoindexFile(path_file);
    else
        send_fullresponse(socket_fd, getFileSize(path_file), path_file, "HTTP/1.1 200 OK\r\n");
    throw std::runtime_error("Response Get sucess sent!");
}

void response::post_response()
{
    std::stringstream response;
    std::string location_responsefile = "./Pages/response.html"; // the file i will sent !
    // // Status Line
    response << "HTTP/1.1 201 Created";
    // // Headers
    response << "Content-Length: " << getFileSize(location_responsefile) << "\r\n";
    response << "Content-Type: " << "text/html" << "\r\n";

    // // Blank line separating headers from body
    response << "\r\n";
    send_string(socket_fd, response.str());
    send_body(socket_fd, location_responsefile);
    throw std::runtime_error("Response Post sucess sent!");
}

void response::delete_response()
{
    std::stringstream response;
    // // Status Line
    response << "HTTP/1.1 204 No Content";
    // // Blank line separating headers from body
    response << "\r\n";

    send_string(socket_fd, response.str());
    throw std::runtime_error("Response delete sucess sent!");
}

void response::error_response(int type_error)
{
    std::string path_file;

    if (this->server_config.error_pages.count(type_error))
    {
        std::string errorPageUri = this->server_config.error_pages.at(type_error);
        const LocationConfigs *errorLocation = findLocation(errorPageUri, this->server_config);
        if (errorLocation)
            path_file = joinPaths(errorLocation->root, errorPageUri);
        else
        {
            std::cout << red << "error in error_response function ! " << reset << std::endl;
        }
    }
    else
        path_file = "./Errors/default.html";
    std::stringstream type_res;
    type_res << type_error;
    send_fullresponse(socket_fd, getFileSize(path_file), path_file, get_statusLine(type_res.str()));
    throw std::runtime_error("Response " + type_res.str() + " sent!");
}

// _________________________________________ //

void response::send_fullresponse(int socket_fd, long size, std::string file_path, std::string status_line)
{
    send_header(socket_fd, size, file_path, status_line);
    send_body(socket_fd, file_path);
}

void response::send_body(int socket_fd, std::string file_path)
{
    std::ifstream file_stream(file_path.c_str(), std::ios::binary);
    if (!file_stream)
    {
        return;
    }

    char buffer[4096]; // 4KB buffer
    while (file_stream.read(buffer, sizeof(buffer)))
    {
        send_chunk(socket_fd, buffer, sizeof(buffer));
    }
    //  check if there is a final, smaller chunk left to send
    if (file_stream.gcount() > 0)
    {
        send_chunk(socket_fd, buffer, file_stream.gcount());
    }
}

void response::send_header(int socket_fd, long size, std::string file_path, std::string status_line)
{
    std::stringstream response;

    // Status Line
    response << status_line;
    // Headers
    response << "Content-Length: " << size << "\r\n";
    response << "Content-Type: " << getMimeType(file_path) << "\r\n";

    // Blank line separating headers from body
    response << "\r\n";

    send_string(socket_fd, response.str());
}

// ------------------------ HELPER FUNCTIONS ------------------ //

std::string response::create_path_error(std::string type_error)
{
    return "./Pages/Errors/" + type_error + ".html";
}

std::string response::get_statusLine(std::string type_res)
{
    if (type_res == "403")
        return "HTTP/1.1 403 Forbidden";
    else if (type_res == "404")
        return "HTTP/1.1 404 Not Found";
    else if (type_res == "405")
        return "HTTP/1.1 405 Method Not Allowed";
    else if (type_res == "500")
        return "Internal Server Error";
    else if (type_res == "502")
        return "Bad Gateway";
    else if (type_res == "504")
        return "Gateway Timeout";
    else if (type_res == "400")
        return "Bad Request";
    return "HTTP/1.1 200 OK\r\n";
}

// Gets the MIME type from a file path based on its extension
std::string response::getMimeType(const std::string &filePath)
{
    // Find the position of the last dot
    size_t dot_pos = filePath.rfind('.');
    if (dot_pos == std::string::npos)
    {
        // No extension found, return the default
        return "application/octet-stream";
    }

    // Get the extension substring
    std::string extension = filePath.substr(dot_pos);

    // Look up the extension
    if (extension == ".html" || extension == ".htm")
        return "text/html";
    if (extension == ".css")
        return "text/css";
    if (extension == ".js")
        return "application/javascript";
    if (extension == ".jpg" || extension == ".jpeg")
        return "image/jpeg";
    if (extension == ".png")
        return "image/png";
    if (extension == ".gif")
        return "image/gif";
    if (extension == ".txt")
        return "text/plain";

    // Return the default for unknown extensions
    return "application/octet-stream";
}

void response::send_chunk(int socket_fd, const char *data, size_t length)
{
    size_t total_bytes_sent = 0;
    while (total_bytes_sent < length)
    {
        ssize_t bytes_sent = send(socket_fd, data + total_bytes_sent, length - total_bytes_sent, 0);
        if (bytes_sent == -1)
        {
            std::cerr << "Error sending chunk!" << std::endl;
            return;
        }
        total_bytes_sent += bytes_sent;
    }
}

void response::send_string(int socket_fd, std::string response)
{

    int length = response.length();
    int byte_sended = 0;
    while (byte_sended < length)
    {
        ssize_t byte_send = send(socket_fd, response.c_str() + byte_sended, length - byte_sended, 0);
        if (byte_send == -1)
        {

            std::cout << "error !" << std::endl;
            return;
        }
        byte_sended += byte_send;
    }
}

long response::getFileSize(const std::string &path)
{
    struct stat file_info;

    if (stat(path.c_str(), &file_info) != 0)
    {
        return -1; // Return -1 to signal an error
    }

    return file_info.st_size;
}

response::~response() {}