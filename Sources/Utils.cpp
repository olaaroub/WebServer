/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 14:55:27 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/02 16:37:32 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

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
