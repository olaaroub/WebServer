/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 14:55:18 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/20 01:59:52 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib.hpp"
#include "Configs.hpp"


class ResponseSentException : public std::exception
{
    const std::string messageSent;
    public:
        const char* what() const throw();
        ResponseSentException(const std::string& message);
        ~ResponseSentException() throw() {}
};

std::string joinPaths(const std::string& p1, const std::string& p2);
std::string normalizePath(const std::string& uri);
std::string getExtension(const std::string& path);
std::string getFileContents(const std::string& filePath);
 std::string trimWhitespace(const std::string& s);
 std::string generateUniqueFilename();
//  const LocationConfigs *findLocation(const std::string &uri, const ServerConfigs &server_config);
    const char* getReasonPhrase(int code);
   std::string getMimeType(const std::string &filePath);
std::string generate_body_FromFile(std::string pathFIle);
// std::string toLower(const std::string& s) ;
std::string toLower(const std::string& str);
long parseSizeToBytes(const std::string& size_str);
