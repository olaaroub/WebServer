#include "Post.hpp"
#include "Utils.hpp"

std::string Post::extractfileName(std::string query)
{
    unsigned long pos = query.find("=");
    if (pos != std::string::npos)
    {
        return query.substr(pos + 1);
    }
    return "error";
}

std::string Post::get_locationFiles()
{
    return location_savedFiles;
}

int Post::post_multipartFormData(std::string content_type, std::string body_content)
{
    std::string boundary = "--" + get_boundary(content_type);

    // start parsing !
    // --------------------------------- //
    unsigned long start_pos = 0;
    unsigned long end_pos = 0;

    start_pos = body_content.find(boundary);
    if (start_pos == std::string::npos)
        return 400;
    start_pos += boundary.length();
    while (start_pos < body_content.length())
    {

        end_pos = body_content.find(boundary, start_pos);
        if (end_pos == std::string::npos)
            break;
        // extracting part
        std::string part = body_content.substr(start_pos, end_pos - start_pos);

        // extracting header & body
        unsigned long header_end = part.find("\r\n\r\n");
        if (header_end == std::string::npos)
        {
            start_pos = end_pos + boundary.length();
            continue;
        }
        std::string header = part.substr(0, header_end);
        std::string content = part.substr(header_end + 4, part.length() - (header_end + 4) - 2);

        unsigned long contentDis_pos = header.find("Content-Disposition");
        if (contentDis_pos == std::string::npos)
        {
            return 400;
        }
        std::string file_name = extract_nameFile(header);

        std::ofstream savefile1(joinPaths(get_locationFiles(), file_name).c_str(), std::ios::binary);
        savefile1.write(content.c_str(), content.length());
        savefile1.close();

        start_pos = end_pos + boundary.length();
    }
    return 1;
}

void Post::post_Query(std::string queryLine, std::string body_content)
{
    if (queryLine.empty())
        path_savedFile = joinPaths(get_locationFiles(), generateUniqueFilename());
    else
        path_savedFile = joinPaths(get_locationFiles(), extractfileName(queryLine));

    std::ofstream savefile(path_savedFile.c_str(), std::ios::binary);
    savefile.write(body_content.c_str(), body_content.length());
    savefile.close();
}

std::string Post::get_boundary(std::string content_type)
{
    unsigned long pos = content_type.find("multipart/form-data");
    if (pos != std::string::npos)
    {
        std::string boundary = "boundary=";
        unsigned long pos_bound = content_type.find("boundary=");
        if (pos_bound != std::string::npos)
        {
            return content_type.substr(pos_bound + boundary.length());
        }
    }
    return "";
}

std::string Post::extract_nameFile(std::string header)
{
    std::string str_searching = "filename=\"";
    unsigned long start1_pos = header.find(str_searching);
    start1_pos += str_searching.length();
    unsigned long end1_pos = header.find("\"", start1_pos);
    std::string nameFile = header.substr(start1_pos, end1_pos - start1_pos);
    // std::cout << RED << nameFile << RESET << std::endl;
    return nameFile;
}

Post::Post(std::string location_savedFiles)
{
    this->location_savedFiles = location_savedFiles;
}
Post::~Post() {}
