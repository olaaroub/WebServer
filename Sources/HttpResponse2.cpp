/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse2.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 19:02:05 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/08 19:34:17 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse2.hpp"

HttpResponse::HttpResponse() : _http_version("HTTP/1.1"), _status_code(200) {
    this->_reason_phrase = getReasonPhrase(200);
}

HttpResponse::~HttpResponse() {}

// const char* HttpResponse::getReasonPhrase(int code) {
//     switch (code) {
//         case 200: return "OK";
//         case 403: return "Forbidden";
//         case 404: return "Not Found";
//         case 405: return "Method Not Allowed";
//         case 500: return "Internal Server Error";
//         case 501: return "Not Implemented";
//         case 502: return "Bad Gateway";
//         case 504: return "Gateway Timeout";
//         default: return "Unknown Status";
//     }
// }

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
