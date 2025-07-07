/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ohammou- <ohammou-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 14:21:00 by olaaroub          #+#    #+#             */
/*   Updated: 2025/07/06 15:34:57 by ohammou-         ###   ########.fr       */
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
	void cleanContent();
	void parse();
	bool isWhitespaces(const std::string& str)const;
	bool isDirective(const std::string& str)const;
	bool isMethod(const std::string& str)const;

	ServerConfigs parseServerBlock(const std::string& serverBlock);
    LocationConfigs parseLocationBlock(std::stringstream& ss);
    void validateServers() const;

	ConfigParser(const ConfigParser &obj);
	ConfigParser& operator=(const ConfigParser& obj);
};