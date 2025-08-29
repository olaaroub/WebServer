/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 19:02:05 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/29 21:41:51 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

HttpResponse::HttpResponse() : _http_version("HTTP/1.1"), _status_code(200) {
    this->_reason_phrase = getReasonPhrase(200);
}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatus(int code) {
    this->_status_code = code;
    this->_reason_phrase = getReasonPhrase(code);
}

void HttpResponse::addHeader(const std::string& key, const std::string& value) {
    this->_headers[key] = value;
}

void HttpResponse::setBody(const std::string& body) {
    this->_body = body;
}

std::string HttpResponse::getHeader(const std::string& key) const {
    std::string lname = toLower(key);
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
         it != _headers.end(); ++it) {
        if (toLower(it->first) == lname) {
            return it->second;
        }
    }
    return "";
}
std::string HttpResponse::toString() const {
    std::stringstream response_ss;

    response_ss << _http_version << " " << _status_code << " " << _reason_phrase << "\r\n";

    std::map<std::string, std::string> final_headers = _headers;

    if (final_headers.find("Content-Length") == final_headers.end()) {
        std::stringstream ss;
        ss << _body.length();
        final_headers["Content-Length"] = ss.str();
    }

    for (std::map<std::string, std::string>::const_iterator it = final_headers.begin(); it != final_headers.end(); ++it) {
        response_ss << it->first << ": " << it->second << "\r\n";
    }

    response_ss << "\r\n";

    response_ss << _body;
    return response_ss.str();
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
			body_str = cgiOutput;
	}

	setBody(body_str);

	std::stringstream cgi_headers_ss(headers_str);
	std::string header_line;

	while (std::getline(cgi_headers_ss, header_line))
	{
		std::string trimmed_line = trimWhitespace(header_line);
		if (trimmed_line.empty())
			continue;

		if (strncasecmp(header_line.c_str(), "Status:", 7) == 0)
		{
			std::stringstream ss(header_line.substr(8));
			int code;
			ss >> code;
			setStatus(code);
		}
		else
		{
			size_t colon_pos = header_line.find(":");
			if (colon_pos != std::string::npos)
			{
				std::string key = header_line.substr(0, colon_pos);
				std::string value = header_line.substr(colon_pos + 1);
				if (!value.empty() && value[0] == ' ')
					value.erase(0, 1);
				addHeader(key, value);
			}
		}
	}
}
