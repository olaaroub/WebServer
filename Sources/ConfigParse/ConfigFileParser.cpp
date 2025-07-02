/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 17:21:57 by olaaroub          #+#    #+#             */
/*   Updated: 2025/07/02 01:02:21 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFileParser.hpp"

ConfigParser::ConfigParser(const std::string &rawContent) : _content(rawContent) { this->parse(); }

ConfigParser::~ConfigParser() { ; }

const std::vector<ServerConfigs> &ConfigParser::getServers() const { return this->_servers; }

void ConfigParser::checkBraces() const
{
	int braces = 0;
	for (size_t i = 0; i < _content.length(); i++)
	{
		if (_content[i] == '{')
			braces++;
		else if (_content[i] == '}')
		{
			if (braces == 0)
				throw std::runtime_error("Config Error: Unmatched closing brace '}' found.");
			braces--;
		}
	}
	if (braces != 0)
		throw std::runtime_error("Config Error: Unmatched opening brace '{' at end of file.");
}

void ConfigParser::cleanContent(const std::string &content)
{
	std::string cleaned;
	cleaned.reserve(content.length());

	for (size_t i = 0; i < content.length(); ++i)
	{
		char c = content[i];
		if (c == '#')
		{
			while (i < content.length() && content[i] != '\n')
				i++;
			continue;
		}
		if (c == '{' || c == '}' || c == ';')
		{
			if (!cleaned.empty() && cleaned[cleaned.length() - 1] != ' ')
				cleaned += ' ';
			cleaned += c;
			cleaned += ' ';
		}
		else if (std::isspace(static_cast<unsigned char>(c)))
		{
			if (!cleaned.empty() && cleaned[cleaned.length() - 1] != ' ')
				cleaned += ' ';
		}
		else
		{
			cleaned += c;
		}
	}
	this->_content = cleaned;
}

bool ConfigParser::isWhitespaces(const std::string &str) const
{
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!std::isspace(static_cast<unsigned char>(str[i])))
			return false;
	}
	return true;
}

void ConfigParser::parse()
{
	this->checkBraces();
	this->cleanContent(_content);

	size_t pos = 0;
	while ((pos = this->_content.find("server", pos)) != std::string::npos)
	{
		// std::cout << "DEBUFGGGG\n";
		size_t openingBrace = pos + 6;
		while (openingBrace < _content.length() && std::isspace(static_cast<unsigned char>(_content[openingBrace])))
			openingBrace++;
		if (openingBrace >= _content.length() || _content[openingBrace] != '{')
			throw std::runtime_error("Config Error: Missing opening brace '{' after 'server'.");

		size_t closingBrace = openingBrace + 1;
		int depth = 1;
		while (closingBrace < this->_content.length() && depth > 0)
		{
			if (this->_content[closingBrace] == '{')
				depth++;
			else if (this->_content[closingBrace] == '}')
				depth--;
			closingBrace++;
		}
		if (depth != 0)
			throw std::runtime_error("Config Error: Unmatched braces in server block.");

		std::string blockContent = this->_content.substr(openingBrace + 1, closingBrace - openingBrace - 2);
		// std::cout << "helloooee 22" << blockContent << std::endl;
		if (isWhitespaces(blockContent))
			throw std::runtime_error("Config Error: Empty 'server' block found.");
		this->_servers.push_back(this->parseServerBlock(blockContent));
		// std::cout << "tfo 3la mk" << _rawServerBlocks[0] << std::endl;
		// std::cout << "tfo 3la mk" << _rawServerBlocks[1] << std::endl;
		pos = closingBrace;
	}
	if (this->_servers.empty())
		throw std::runtime_error("Config Error: No 'server' blocks found in config file.");
	this->validateServers();
}

ServerConfigs ConfigParser::parseServerBlock(const std::string &block)
{
	ServerConfigs server_conf;
	std::stringstream ss(block);
	std::string token;

	while (ss >> token)
	{
		if (token == "listen")
		{
			if (server_conf.listen_set)
				throw std::runtime_error("Config Error: Duplicate 'listen' directive.");
			std::string listen_val;
			ss >> listen_val;
			size_t colon_pos = listen_val.find(':');
			if (colon_pos != std::string::npos)
			{
				server_conf.host = listen_val.substr(0, colon_pos);
				server_conf.port = std::atoi(listen_val.substr(colon_pos + 1).c_str());
			}
			else
			{
				server_conf.port = std::atoi(listen_val.c_str());
				server_conf.host = "0.0.0.0";
			}
			if (!(ss >> token) || token != ";")
				throw std::runtime_error("Config Error: Missing ';' after 'listen' directive.");
			server_conf.listen_set = true;
		}
		else if (token == "client_max_body_size")
		{
			if (server_conf.client_max_body_size_set)
				throw std::runtime_error("Config Error: Duplicate 'client_max_body_size' directive.");
			std::string size_str;
			ss >> size_str;
			server_conf.client_max_body_size = std::strtol(size_str.c_str(), NULL, 10);
			if (!(ss >> token) || token != ";")
				throw std::runtime_error("Config Error: Missing ';' after 'client_max_body_size' directive.");
			server_conf.client_max_body_size_set = true;
		}
		else if (token == "server_name")
		{
			while (ss >> token && token != ";")
			{
				server_conf.server_names.push_back(token);
			}
            if (token != ";")
                throw std::runtime_error("Config Error: Missing ';' after 'server_name' directive.");
		}
		else if (token == "error_page")
		{
			int code;
			std::string path;
			ss >> code >> path;
			server_conf.error_pages[code] = path;
			if (!(ss >> token) || token != ";")
				throw std::runtime_error("Config Error: Missing ';' after 'error_page' directive.");
		}
		else if (token == "location")
		{
			server_conf.locations.push_back(parseLocationBlock(ss));
		}
		else
		{
			throw std::runtime_error("Config Error: Unknown directive '" + token + "' in server block.");
		}
	}
	return server_conf;
}

LocationConfigs ConfigParser::parseLocationBlock(std::stringstream &ss)
{
	LocationConfigs location_conf;
	ss >> location_conf.path;
	std::string token;
	ss >> token;
	if (token != "{")
		throw std::runtime_error("Config Error: Expected '{' after location path.");

	while (ss >> token && token != "}")
	{
		if (token == "root")
		{
			if (location_conf.root_set)
				throw std::runtime_error("Config Error: Duplicate 'root' directive in location.");
			ss >> location_conf.root;
			if (!(ss >> token) || token != ";")
				throw std::runtime_error("Config Error: Missing ';' after 'root' directive.");
			location_conf.root_set = true;
		}
		else if (token == "autoindex")
		{
			if (location_conf.autoindex_set)
				throw std::runtime_error("Config Error: Duplicate 'autoindex' directive in location.");
			std::string val;
			ss >> val;
			location_conf.autoindex = (val == "on");
			if (!(ss >> token) || token != ";")
				throw std::runtime_error("Config Error: Missing ';' after 'autoindex' directive.");
			location_conf.autoindex_set = true;
		}
		else if (token == "index")
		{
            if(location_conf.index_file_set)
                throw std::runtime_error("Config Error: Duplicate 'index' directive in location.");
			ss >> location_conf.index_file;
			if (!(ss >> token) || token != ";")
				throw std::runtime_error("Config Error: Missing ';' after 'index' directive.");
            location_conf.index_file_set = true;
		}
		else if (token == "allow_methods")
		{
			while (ss >> token && token != ";")
			{
				location_conf.allowed_methods.push_back(token);
			}
			if (token != ";")
				throw std::runtime_error("Config Error: Missing ';' after 'allow_methods' directive.");
		}
		else if (token == "return")
		{
            if(location_conf.redirection_set)
                throw std::runtime_error("Config Error: Duplicate 'return' directive in location.");
			ss >> location_conf.redirection_code >> location_conf.redirection_url;
			if (!(ss >> token) || token != ";")
				throw std::runtime_error("Config Error: Missing ';' after 'return' directive.");
            location_conf.redirection_set = true;
		}
		else if (token == "cgi_pass")
        {
            if (location_conf.cgi_set)
                throw std::runtime_error("Config Error: Duplicate 'cgi_pass' directive in location.");
            std::string ext, path;
            ss >> ext >> path;
            location_conf.cgi_handlers[ext] = path;
            if (!(ss >> token) || token != ";")
                throw std::runtime_error("Config Error: Missing ';' after 'cgi_pass' directive.");
            location_conf.cgi_set = true;
        }
		else if (token == "upload_path")
		{
            if(location_conf.upload_path_set)
                throw std::runtime_error("Config Error: Duplicate 'upload_path' directive in location.");
			ss >> location_conf.upload_path;
			if (!(ss >> token) || token != ";")
				throw std::runtime_error("Config Error: Missing ';' after 'upload_path' directive.");
            location_conf.upload_path_set = true;
		}
		else
		{
			throw std::runtime_error("Config Error: Unknown directive '" + token + "' in location block.");
		}
	}
	if (token != "}")
		throw std::runtime_error("Config Error: Location block not closed properly.");
	return location_conf;
}

void ConfigParser::validateServers() const
{
    std::map<std::string, std::set<std::string> > serverNameMap;
    std::set<std::string> defaultServers;

    for (size_t i = 0; i < this->_servers.size(); ++i)
    {
        const ServerConfigs& server = this->_servers[i];
        std::stringstream ss;
        ss << server.host << ":" << server.port;
        std::string listenKey = ss.str();

        std::set<std::string> locationPaths;
        for (size_t j = 0; j < server.locations.size(); ++j)
        {
            if (!locationPaths.insert(server.locations[j].path).second)
            {
                throw std::runtime_error("Config Error: Duplicate location path '" + server.locations[j].path + "' in a server block.");
            }
        }

        if (server.server_names.empty())
        {
            if (defaultServers.count(listenKey))
            {
                throw std::runtime_error("Config Error: Multiple default servers defined for " + listenKey);
            }
            defaultServers.insert(listenKey);
        }
        else
        {
            for (size_t j = 0; j < server.server_names.size(); ++j)
            {
                if (!serverNameMap[listenKey].insert(server.server_names[j]).second)
                {
                    throw std::runtime_error("Config Error: Duplicate server_name '" + server.server_names[j] + "' for " + listenKey);
                }
            }
        }
    }
}