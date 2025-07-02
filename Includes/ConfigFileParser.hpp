/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 14:21:00 by olaaroub          #+#    #+#             */
/*   Updated: 2025/07/01 19:35:08 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib.hpp"
#include "Configs.hpp"
class ConfigParser
{
public:
	ConfigParser(const std::string &rawContent);
	const std::vector<ServerConfigs>& getServers()const;
	~ConfigParser();

private:
	std::string _content;
	std::vector<ServerConfigs> _servers;

	void checkBraces()const;
	void cleanContent(const std::string& content);
	void parse();
	bool isWhitespaces(const std::string& str)const;

	ServerConfigs parseServerBlock(const std::string& serverBlock);
    LocationConfigs parseLocationBlock(std::stringstream& ss);
    void validateServers() const;

	ConfigParser(const ConfigParser &obj);
	ConfigParser& operator=(const ConfigParser& obj);
};