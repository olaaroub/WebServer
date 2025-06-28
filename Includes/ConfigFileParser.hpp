/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 14:21:00 by olaaroub          #+#    #+#             */
/*   Updated: 2025/06/28 18:52:37 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib.hpp"
class ConfigParser
{
public:
	ConfigParser(const std::string &rawContent);
	const std::vector<std::string>& getRawServerBlocks()const;
	~ConfigParser();

private:
	std::string _rawContent;
	std::vector<std::string> _rawServerBlocks;

	void parse();
	void checkBraces()const;
	bool isWhitespaces(const std::string& str)const;
	ConfigParser(const ConfigParser &obj);
	ConfigParser& operator=(const ConfigParser& obj);
};