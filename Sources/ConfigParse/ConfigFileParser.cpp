/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 17:21:57 by olaaroub          #+#    #+#             */
/*   Updated: 2025/06/28 20:16:06 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFileParser.hpp"

ConfigParser::ConfigParser(const std::string &rawContent) : _rawContent(rawContent) { this->parse(); }

ConfigParser::~ConfigParser() { ; }

const std::vector<std::string>& ConfigParser::getRawServerBlocks()const { return this->_rawServerBlocks; }

void ConfigParser::checkBraces() const
{
	int braces = 0;
	for (size_t i = 0; i < _rawContent.length(); i++){
		if (_rawContent[i] == '{')
			braces++;
		else if (_rawContent[i] == '}'){
			if (braces == 0)
				throw std::runtime_error("Config Error: Unmatched closing brace '}' found.");
			braces--;
		}
	}
	if (braces != 0)
		throw std::runtime_error("Config Error: Unmatched opening brace '{' at end of file.");
}

bool ConfigParser::isWhitespaces(const std::string &str) const
{
	for (size_t i = 0; i < str.length(); ++i){
		if (!std::isspace(static_cast<unsigned char>(str[i])))
			return false;
	}
	return true;
}

void ConfigParser::parse()
{
	this->checkBraces();

	size_t pos = 0;
	while ((pos = this->_rawContent.find("server", pos)) != std::string::npos){
		// std::cout << "DEBUFGGGG\n";
		size_t openingBrace = this->_rawContent.find('{', pos);
		if (openingBrace == std::string::npos)
			throw std::runtime_error("Config Error: Missing opening brace '{' after 'server'.");

		size_t closingBrace = openingBrace + 1;
		int depth = 1;
		while (closingBrace < this->_rawContent.length() && depth > 0){
			if (this->_rawContent[closingBrace] == '{')
				depth++;
			else if (this->_rawContent[closingBrace] == '}')
				depth--;
			closingBrace++;
		}
		if (depth != 0)
			throw std::runtime_error("Config Error: Unmatched braces in server block.");

		std::string blockContent = this->_rawContent.substr(openingBrace + 1, closingBrace - openingBrace - 2);
		// std::cout << "helloooee 22" << blockContent << std::endl;
		if (isWhitespaces(blockContent))
			throw std::runtime_error("Config Error: Empty 'server' block found.");
		this->_rawServerBlocks.push_back(blockContent);
		// std::cout << "tfo 3la mk" << _rawServerBlocks[0] << std::endl;
		// std::cout << "tfo 3la mk" << _rawServerBlocks[1] << std::endl;
		pos = closingBrace;
	}
	if( this->_rawServerBlocks.empty())
		throw std::runtime_error("Config Error: No 'server' blocks found in config file.");
}