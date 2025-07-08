#include "client.hpp"
#include "WebServer.hpp"

#include <sys/stat.h> 

client:: client(const ServerConfigs& server_config) : network(server_config, false)
{
    // is_server = false;
    request.state = 0;
}

// client:: client() : network(false)
// {
//     request.state = 0;
// }

void client:: epoll_modify()
{
    ev.events = EPOLLOUT | EPOLLRDHUP;
    ev.data.fd = socket_fd;
    if (epoll_ctl(WebServer:: kernel_identifier, EPOLL_CTL_MOD, socket_fd, &ev) < 0)
    {
        perror("epoll_modify");
        throw std::string("");
    }
}


// ------------- test ---------------- //


int check_location(ServerConfigs serverconfigs, std::string uri)
{
    unsigned long max_length = -1;
    int right_index = -1;
    for (unsigned long i = 0; i < serverconfigs.locations.size() ; i++)
    {
        std::string path = serverconfigs.locations.at(i).path;
        if (uri.find(path) == 0)
        {
            if (path.length() > max_length)
            {
                max_length = path.length();
                right_index = i;
            }

        }
    }
    return right_index;
}

int check_method(std::string method , std::vector<std::string> allowed_methods)
{
    for (unsigned long i = 0; i < allowed_methods.size(); i++)
    {
        if (method == allowed_methods.at(i))
            return 1;
    }
    return 0;
}

std::string get_full_path(std::string uri, std::string root_path)
{
    // We know 'uri' always starts with '/'.
    // We only need to check if 'root' ends with '/' to avoid a double slash.
    if (!root_path.empty() && root_path[root_path.length() - 1] == '/') {
        // Use the part of the uri *after* its leading slash.
        return root_path + uri.substr(1);
    }
    
    // Otherwise, they fit together perfectly.
    return root_path + uri;

}

// Gets the MIME type from a file path based on its extension
std::string getMimeType(const std::string& filePath) {
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

std::string get_response(struct stat file_info, std::string file_path, std::string body)
{
    std::stringstream response;

    // Status Line
    response << "HTTP/1.1 200 OK\r\n";
    // Headers
    response << "Content-Length: " << file_info.st_size << "\r\n";
    response << "Content-Type: " << getMimeType(file_path) << "\r_n";

    // Blank line separating headers from body
    response << "\r\n";

    // body
    response << body;

    return response.str();




}

std::string get_body(std::string file_path)
{
    std::ifstream file_stream(file_path.c_str(), std::ios::binary);

    if (!file_stream) {
        // Handle error: could not open file
        return "error";
    }



    return "ss";

}


void client:: onEvent()
{
    if (event & (EPOLLERR | EPOLLHUP))
        perror("ERROR: ");
    else if (event & EPOLLIN)
    {
        std::cout << "in input" << std::endl;
        int is_finish = request.run_parser(socket_fd);
        if (is_finish)
            epoll_modify();
    }
    else if (event & EPOLLOUT)
    {
        std::cout << "send output" << std::endl;
        std::cout << "------------------Part of response begin !!!--------------" << std::endl;

        // -(My part ) ==> GET POST DELETE - -------------------//

        int location = check_location(server_config, request.RequestLine.get_url());
        if (location == -1)
            std::cout << "error , there is not a location like this !! not found " << std::endl;

        int method = check_method(request.RequestLine.get_method(), server_config.locations.at(location).allowed_methods);
        if (method == 0)
            std::cout << "error , method not found !!" << std::endl;

        // start building GET method !!
        if (request.RequestLine.get_method() == "GET")
        {
            // uri  | index_file  | root_path
            //  build the full path to the file . 
            std::string full_path =  get_full_path(request.RequestLine.get_url(), server_config.locations.at(location).root);
            // check is this path exsist !
            struct stat file_info;

            if (stat(full_path.c_str(), &file_info) != 0)
            {
                std::cout << "error , not found !" << std::endl;
            }
            bool is_a_file = S_ISREG(file_info.st_mode);
            bool is_a_dir = S_ISDIR(file_info.st_mode);

            if (!is_a_file)
            {
                // check  permession !
                if (file_info.st_mode & S_IRUSR)
                {
                    std::cout << "error, 403 Forbiden ! (there is no permession for read)" << std::endl;
                }
                else 
                {
                    // sending the response
                    
                    std::string response = get_response(file_info, full_path , get_body(full_path));

                }
            }
            else if (!is_a_dir)
            {
                // check  permession !
                if (file_info.st_mode & S_IXUSR)
                {
                    std::cout << "error, 403 Forbiden ! (there is no permession for execute the dir)" << std::endl;
                }
                else
                {
                    // sending the response


                }

            }

        
        }
        
        
        


        // send(socket_fd, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 21\r\n\r\nChunked data received!", 87, 0);
            // This is where you'll implement the GET checklist from above.
            // 1. Get URI from request.
            // 2. Find matching config location.
            // 3. Check if GET is an allowed method in that config.
            // 4. Build the full path to the file.
            // 5. Check if the file exists and you have permission.
            // 6. If all checks pass:
            //      - Read the file content into a string (the body).
            //      - Build a "200 OK" response string with the correct Content-Type and Content-Length headers.
            //      - Send that response string.
            // 7. If any check fails:
            //      - Build the appropriate error response (404, 403, 405).
            //      - Send that error response.

    }
}






client:: ~client()
{

}