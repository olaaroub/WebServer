/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileReader.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 13:42:33 by olaaroub          #+#    #+#             */
/*   Updated: 2025/06/25 13:58:54 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileReader.hpp"

FileReader::FileReader(const std::string &path) : _confPath(path)
{
	validateFile();
	readFile();
}

FileReader::~FileReader() { ; }

const std::string &FileReader::getContent() const { return _confContent; }

void FileReader::validateFile() const
{
	if (access(_confPath.c_str(), R_OK) != 0)
		throw std::runtime_error("Configuration file inaccessible or does not exist: " + _confPath);
}

void FileReader::readFile()
{
	std::ifstream fileStream(_confPath.c_str());

	if (!fileStream.is_open())
		throw std::runtime_error("Failed to open configuration file: " + _confPath);

	std::stringstream holder;

	holder << fileStream.rdbuf();
	_confContent = holder.str();
	fileStream.close();
	if (_confContent.empty())
		throw std::runtime_error("Configuration file is empty: " + _confPath);
}
