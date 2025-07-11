#include "response.hpp"


response::response(int socket_fd, std::string path_file, std::string status_line) // constructer for GET
{
    std::string response = get_response(getFileSize(path_file), path_file , get_body(path_file), status_line);
    send_response(socket_fd, response);
}



// ------------------------ HELPER FUNCTIONS ------------------ // 


// Gets the MIME type from a file path based on its extension
std::string response::getMimeType(const std::string& filePath) {
    // Find the position of the last dot
    size_t dot_pos = filePath.rfind('.');
    if (dot_pos == std::string::npos) {
        // No extension found, return the default
        return "application/octet-stream";
    }

    // Get the extension substring
    std::string extension = filePath.substr(dot_pos);

    // Look up the extension
    if (extension == ".html" || extension == ".htm") return "text/html";
    if (extension == ".css") return "text/css";
    if (extension == ".js") return "application/javascript";
    if (extension == ".jpg" || extension == ".jpeg") return "image/jpeg";
    if (extension == ".png") return "image/png";
    if (extension == ".gif") return "image/gif";
    if (extension == ".txt") return "text/plain";
    
    // Return the default for unknown extensions
    return "application/octet-stream";
}

std::string response::get_response(long size, std::string file_path, std::string body, std::string status_line)
{
    std::stringstream response;

    // Status Line
    response << status_line;
    // Headers
    response << "Content-Length: " << size << "\r\n";
    response << "Content-Type: " << getMimeType(file_path) << "\r\n";

    // Blank line separating headers from body
    response << "\r\n";

    // body
    response << body;

    return response.str();
}

std::string response::get_body(std::string file_path)
{
    std::ifstream file_stream(file_path.c_str(), std::ios::binary);

    if (!file_stream) {
        // Handle error: could not open file
        return "error";
    }

    std::string str;
    std::istreambuf_iterator<char> it(file_stream);
    std::istreambuf_iterator<char> end;

    while (it != end) {
    str += *it;
    ++it; // Move to the next one
    }

    return str;

}


void response::send_response(int socket_fd, std::string response)
{

    int length = response.length();
    int byte_sended = 0;
    while (byte_sended < length)
    {
        ssize_t byte_send = send(socket_fd, response.c_str() + byte_sended, length - byte_sended, 0);
        if (byte_send == -1)
        {

            std::cout << "error !" << std::endl;
            return ;
        }
        byte_sended += byte_send;
    }
    


}

long response::getFileSize(const std::string& path) {
    struct stat file_info;

    if (stat(path.c_str(), &file_info) != 0) {
        return -1; // Return -1 to signal an error
    }

    return file_info.st_size;
}

response::~response(){}