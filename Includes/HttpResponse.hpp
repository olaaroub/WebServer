/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/08 19:00:40 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/29 21:19:58 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib.hpp"
#include "Configs.hpp"
#include "Utils.hpp"

class HttpResponse
{
private:
	std::string 								_http_version;
	int											_status_code;
	std::string 								_reason_phrase;
	std::map<std::string, std::string> 			_headers;
	std::string 								_body;

public:
	HttpResponse();
	~HttpResponse();

	void 										setFromCgiOutput(const std::string &cgiOutput);
	void 										setStatus(int code);
	void 										addHeader(const std::string &key, const std::string &value);
	void 										setBody(const std::string &body);
	std::string 								getHeader(const std::string &key) const;
	std::string 								toString() const;
};