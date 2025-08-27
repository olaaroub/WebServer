/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 14:55:18 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/27 18:28:08 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib.hpp"
#include "Configs.hpp"

class ResponseSentException : public std::exception
{
	const std::string messageSent;

public:
	const char *what() const throw();
	ResponseSentException(const std::string &message);
	~ResponseSentException() throw() {}
};

class ParseError : public std::exception
{
private:
	std::string _Error;

public:
	short ErrorStute;
	ParseError(std::string Error, short stute);
	short getStutError() const;
	const char *what() const throw();
	~ParseError() throw();
};

enum ErrorCode
{
	noError = 0,
	closeConnection = 1,
	badRequest = 400,
	timeout = 408,
	payloadTooLarge = 413,
	uRLTooLarge = 414,
	requestHeaderTooLarge = 431,
	ServerError = 500,
	methodNotImplemented = 501
};

std::string joinPaths(const std::string &p1, const std::string &p2);
std::string normalizePath(const std::string &uri);
std::string getExtension(const std::string &path);
std::string getFileContents(const std::string &filePath);
std::string trimWhitespace(const std::string &s);
std::string generateUniqueFilename();
const char *getReasonPhrase(int code);
std::string getMimeType(const std::string &filePath);
std::string generate_body_FromFile(std::string pathFIle);
std::string toLower(const std::string &str);
long parseSizeToBytes(const std::string &size_str);
bool pathChecker(std::string Uri);
std::string uRLEncoding(std::string url);