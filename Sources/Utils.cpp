/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 14:55:27 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/21 13:48:22 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

ResponseSentException::ResponseSentException(const std::string& message): messageSent(message){ ; }
const char* ResponseSentException::what() const throw(){
    return messageSent.c_str();
}


ParseError:: ParseError(std::string Error, short stute) : _Error(Error), ErrorStute(stute) {}
short ParseError:: getStutError() const {return ErrorStute;}
const char* ParseError:: what() const throw()
{
    return _Error.c_str();
}
ParseError:: ~ParseError() throw() {}

std::string joinPaths(const std::string& p1, const std::string& p2) {

    if (p1.empty()) {
        return p2;
    }
    if (p2.empty()) {
        return p1;
    }

    std::string path1 = p1;
    std::string path2 = p2;
    while (!path1.empty() && path1[path1.length() - 1] == '/') {
        path1 = path1.substr(0, path1.length() - 1);
    }

    while (!path2.empty() && path2[0] == '/') {
        path2 = path2.substr(1);
    }
    return path1 + "/" + path2;
}

std::string normalizePath(const std::string& uri) {
    if (uri.empty()) {
        return "/";
    }

    std::string result;
    result.reserve(uri.length());
    result += '/';

    for (size_t i = 0; i < uri.length(); ++i) {
        if (uri[i] == '/') {
            if (result[result.length() - 1] != '/') {
                result += '/';
            }
        } else {
            result += uri[i];
        }
    }
    return result;
}


std::string getExtension(const std::string& path)
{
    size_t dot_pos = path.rfind('.');

    if (dot_pos != std::string::npos)
    {
        size_t slash_pos = path.rfind('/');
        if (slash_pos != std::string::npos && dot_pos < slash_pos)
            return "";
        return path.substr(dot_pos);
    }
    return "";
}


std::string getFileContents(const std::string& filePath) {
    std::ifstream fileStream(filePath.c_str());
    if (!fileStream.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}

 std::string trimWhitespace(const std::string& s)
{
    const std::string WHITESPACE = " \t\r\n";
    size_t first = s.find_first_not_of(WHITESPACE);
    if (std::string::npos == first) {
        return s;
    }
    size_t last = s.find_last_not_of(WHITESPACE);
    return s.substr(first, (last - first + 1));
}
std::string generateUniqueFilename() {
    // Get the current time as a number (seconds since 1970).
    time_t currentTime = time(NULL);

    std::stringstream ss;
    ss << currentTime;

    // 3. Add a file extension.
    return ss.str() + ".ser";
}

long parseSizeToBytes(const std::string& size_str) {
    if (size_str.empty()) {
        throw std::runtime_error("Config Error: client_max_body_size value is empty.");
    }

    long number;
    long multiplier = 1;
    std::string num_part = size_str;

    char last_char = toupper(size_str[size_str.length() - 1]);
    if (last_char == 'M' || last_char == 'K') {
        multiplier = (last_char == 'M') ? (1024 * 1024) : 1024;
        num_part = size_str.substr(0, size_str.length() - 1);
    }

    if (num_part.empty() || num_part.find_first_not_of("0123456789") != std::string::npos) {
        throw std::runtime_error("Config Error: Invalid number format for client_max_body_size '" + size_str + "'");
    }

    char* end = NULL;
    number = std::strtol(num_part.c_str(), &end, 10);

    if (*end != '\0') {
        throw std::runtime_error("Config Error: Invalid client_max_body_size value '" + size_str + "'");
    }

    if (number < 0 || (multiplier > 1 && number > LONG_MAX / (multiplier))) {
        throw std::runtime_error("Config Error: client_max_body_size value '" + size_str + "' is too large and would cause an overflow.");
    }
    return number * multiplier;
}


const char* getReasonPhrase(int code) {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 202: return "Accepted";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Payload Too Large";
        case 415: return "Unsupported Media Type";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 504: return "Gateway Timeout";
        default: return "Unknown Status";
    }
}

std::string getMimeType(const std::string &filePath)
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
    std::cout << MAGENTA << "MIME type lookup for extension: " << extension << RESET << std::endl;

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
    if (extension == ".ico")
        return "image/x-icon";
    if (extension == ".txt")
        return "text/plain";
    if( extension == ".mp4")
        return "video/mp4";

    return "application/octet-stream";
}

std::string generate_body_FromFile(std::string pathFIle)
{
    std::ifstream file_stream(pathFIle.c_str(), std::ios::binary);
    if (!file_stream)
        throw std::runtime_error("GenerateBodyFromFile Error: open file failed !");
    std::stringstream body;

    body << file_stream.rdbuf();

    return body.str();
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    // std::cout << GREEN << result << RESET << std::endl;
        return result;
}