/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   oldHttpResponse.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 18:26:17 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/08 21:45:03 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "oldHttpResponse.hpp"
#include "Utils.hpp"

HttpResponse::HttpResponse() : _http_version("HTTP/1.1"), _status_code(200), _reason_phrase("OK") {}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatus(int code, const std::string &phrase)
{
	_status_code = code;
	_reason_phrase = phrase;
}

void HttpResponse::addHeader(const std::string &key, const std::string &value)
{
	_headers[key] = value;
}

void HttpResponse::setBody(const std::string &body)
{
	_body = body;
}

void HttpResponse::setFromCgiOutput(const std::string &cgiOutput)
{
	std::string headers_str;
	std::string body_str;

	size_t separator = cgiOutput.find("\r\n\r\n");

	// std::cout << "ALOOOOOOOO 2222222: " << separator << std::endl;

	if (separator != std::string::npos)
	{
		headers_str = cgiOutput.substr(0, separator);
		body_str = cgiOutput.substr(separator + 4);
		// std::cout << "ALOOOOO 333333333 headers_str: " << headers_str << std::endl;
		// std::cout << "ALOOOOO 44444 body_str: " << body_str << std::endl;
	}
	else
	{
		separator = cgiOutput.find("\n\n");
		if (separator != std::string::npos)
		{
			headers_str = cgiOutput.substr(0, separator);
			body_str = cgiOutput.substr(separator + 2);
		}
		else
		{
			body_str = cgiOutput;
		}
	}

	setBody(body_str);

	std::stringstream cgi_headers_ss(headers_str);
	std::string header_line;

	while (std::getline(cgi_headers_ss, header_line))
	{
		std::string trimmed_line = trimWhitespace(header_line);
		if (trimmed_line.empty())
		{
			continue;
		}

		if (strncasecmp(header_line.c_str(), "Status:", 7) == 0)
		{
			std::stringstream ss(header_line.substr(8));
			int code;
			std::string phrase;
			ss >> code;
			std::getline(ss, phrase);
			if (!phrase.empty() && phrase[0] == ' ')
			{
				phrase.erase(0, 1);
			}
			setStatus(code, phrase);
		}
		else
		{
			size_t colon_pos = header_line.find(":");
			if (colon_pos != std::string::npos)
			{
				std::string key = header_line.substr(0, colon_pos);
				std::string value = header_line.substr(colon_pos + 1);
				if (!value.empty() && value[0] == ' ')
				{
					value.erase(0, 1);
				}
				addHeader(key, value);
			}
		}
	}
}

std::string HttpResponse::toString() const
{
	std::stringstream response_ss;

	response_ss << _http_version << " " << _status_code << " " << _reason_phrase << "\r\n";

	bool has_content_length = false;
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		response_ss << it->first << ": " << it->second << "\r\n";
		if (strncasecmp(it->first.c_str(), "Content-Length", 14) == 0)
		{
			has_content_length = true;
		}
	}

	if (!has_content_length)
	{
		response_ss << "Content-Length: " << _body.length() << "\r\n";
	}


	response_ss << "\r\n";

	response_ss << _body;

	return response_ss.str();
}

void HttpResponse::sendResponse(int socket_fd) const
{
	std::string response_str = toString();
	// std::cout << "Sending response:\n" << response_str << std::endl;
	ssize_t total_sent = 0;
	while (total_sent < (ssize_t)response_str.length())
	{
		ssize_t bytes_sent = send(socket_fd, response_str.c_str() + total_sent, response_str.length() - total_sent, 0);
		if (bytes_sent <= 0)
		{
			std::cerr << "Failed to send response or connection closed." << std::endl;
			break;
		}
		total_sent += bytes_sent;
	}
}
