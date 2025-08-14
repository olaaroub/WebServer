/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 19:00:40 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/14 22:50:54 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib.hpp"
#include "Configs.hpp"
#include "Utils.hpp"


class HttpResponse
{
private:

    std::string _http_version;
    int _status_code;
    std::string _reason_phrase;
    std::map<std::string, std::string> _headers;
    std::string _body;

    // const char* getReasonPhrase(int code);

public:
    HttpResponse();
    ~HttpResponse();

    void setFromCgiOutput(const std::string& cgiOutput); // hadi kat parsi lia response dial script cgi


    void setStatus(int code);
    void addHeader(const std::string& key, const std::string& value);
    std::string getHeader(const std::string& key) const;
    void setBody(const std::string& body);
    std::string toString() const;
};