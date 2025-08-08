/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 18:25:45 by olaaroub          #+#    #+#             */
/*   Updated: 2025/07/25 18:44:10 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "lib.hpp"

class HttpResponse
{
private:
    std::string _http_version;
    int _status_code;
    std::string _reason_phrase;
    std::map<std::string, std::string> _headers;
    std::string _body;

public:
    HttpResponse();
    ~HttpResponse();

    void setStatus(int code, const std::string& phrase);
    void addHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& body);

    void setFromCgiOutput(const std::string& cgiOutput);

    std::string toString() const;

    void sendResponse(int socket_fd) const;
};

#endif
