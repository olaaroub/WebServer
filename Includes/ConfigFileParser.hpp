/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 14:21:00 by olaaroub          #+#    #+#             */
/*   Updated: 2025/07/10 12:02:17 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib.hpp"
#include "Configs.hpp"

class ConfigParser
{
public:
	ConfigParser(const std::string &rawContent);
	~ConfigParser();

	const std::vector<ServerConfigs>& 				getServers()const;

private:
	std::string  									_content;
	std::vector<ServerConfigs> 						_servers;

	ServerConfigs 									parseServerBlock(const std::string& serverBlock);
    LocationConfigs 								parseLocationBlock(std::stringstream& ss);

	void 											checkBraces()const;
	void 											cleanContent();
	void 											parse();
    void 											validateServers() const;
	bool 											isWhitespaces(const std::string& str)const;
	bool 											isDirective(const std::string& str)const;
	bool 											isMethod(const std::string& str)const;


	ConfigParser(const ConfigParser &obj);
	ConfigParser& operator=(const ConfigParser& obj);
};