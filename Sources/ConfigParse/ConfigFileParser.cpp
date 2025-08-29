/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 17:21:57 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/29 21:42:31 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFileParser.hpp"
#include "Utils.hpp"

ConfigParser::ConfigParser(const std::string &rawContent) : _content(rawContent) { this->parse(); }

ConfigParser::~ConfigParser() { ; }

const std::vector<ServerConfigs> &ConfigParser::getServers() const { return this->_servers; }

void ConfigParser::checkBraces() const
{
	int braces = 0;
	for (size_t i = 0; i < _content.length(); i++){
		if (_content[i] == '{')
			braces++;
		else if (_content[i] == '}'){
			if (braces == 0)
				throw std::runtime_error("Config Error: Unmatched closing brace '}' found.");
			braces--;
		}
	}
	if (braces != 0)
		throw std::runtime_error("Config Error: Unmatched opening brace '{' at end of file.");
}

void ConfigParser::cleanContent()
{
	std::string cleaned;
	cleaned.reserve(_content.length());

	for (size_t i = 0; i < _content.length(); ++i){
		char c = _content[i];
		if (c == '#'){
			while (i < _content.length() && _content[i] != '\n')
				i++;
			continue;
		}
		if (c == '{' || c == '}' || c == ';'){
			if (!cleaned.empty() && cleaned[cleaned.length() - 1] != ' ')
				cleaned += ' ';
			cleaned += c;
			cleaned += ' ';
		}
		else if (std::isspace(static_cast<unsigned char>(c))){
			if (!cleaned.empty() && cleaned[cleaned.length() - 1] != ' ')
				cleaned += ' ';
		}
		else
			cleaned += c;
	}
	this->_content = cleaned;
}

bool ConfigParser::isWhitespaces(const std::string &str) const
{
	for (size_t i = 0; i < str.length(); ++i){
		if (!std::isspace(static_cast<unsigned char>(str[i])))
			return false;
	}
	return true;
}

bool ConfigParser::isDirective(const std::string &str)const{
	return( str == ERROR_PAGES || str == LISTEN || str == CLIENT_MAX_BODY_SIZE ||
			 str == SERVER_NAME || str== LOCATION);
}

bool ConfigParser::isMethod(const std::string &str)const{
	return(str == GET || str == POST || str == DELETE);
}

int ConfigParser::validateAndParsePort(const std::string& port_str)const {
    if (port_str.empty() || port_str.find_first_not_of("0123456789") != std::string::npos) {
        throw std::runtime_error("Config Error: Port '" + port_str + "' is not a valid number.");
    }

    long port = std::strtol(port_str.c_str(), NULL, 10);
    if (port < 1 || port > 65535) {
        throw std::runtime_error("Config Error: Port number '" + port_str + "' is out of range");
    }
    return static_cast<int>(port);
}

void ConfigParser::parse()
{
	this->checkBraces();
	this->cleanContent();

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
		if (token == LISTEN)
		{
			std::string listen_val;
			ss >> listen_val;

			std::string parsed_host;
			int parsed_port;
			size_t colon_pos = listen_val.find(':');

			if (colon_pos != std::string::npos) {
				parsed_host = listen_val.substr(0, colon_pos);
                if (parsed_host.empty())
                    parsed_host = "0.0.0.0";

				std::string port_str = listen_val.substr(colon_pos + 1);
				parsed_port = validateAndParsePort(port_str);

				if(!server_conf.host_set)
				{
					server_conf.host = parsed_host;
					server_conf.host_set = true;
				}
				else if(server_conf.host.compare(parsed_host) != 0)
					throw std::runtime_error("Config Error: Server block cannot have multiple host addresses.");
			}
			else {
                if (listen_val.find('.') != std::string::npos) {
                    throw std::runtime_error("Config Error: listen directive '" + listen_val + "' is missing a port.");
                }
				parsed_port = validateAndParsePort(listen_val);
				if(!server_conf.host_set)
				{
					server_conf.host = "0.0.0.0";
					server_conf.host_set = true;
				}
			}

			std::vector<int>::iterator index = std::find( server_conf.ports.begin(),
    											server_conf.ports.end(), parsed_port);
			if(index == server_conf.ports.end())
				server_conf.ports.push_back(parsed_port);


			if (!(ss >> token) || token != ";")
				throw std::runtime_error("Config Error: Missing ';' after 'listen' directive.");
		}
		else if (token == CLIENT_MAX_BODY_SIZE)
		{
			if (server_conf.client_max_body_size_set)
				throw std::runtime_error("Config Error: Duplicate 'client_max_body_size' directive.");

			std::string size_str;
			ss >> size_str;


			server_conf.client_max_body_size = parseSizeToBytes(size_str);

			if (!(ss >> token) || token != ";")
			throw std::runtime_error("Config Error: Missing ';' after 'client_max_body_size' directive.");
			server_conf.client_max_body_size_set = true;
			// std::cout << "client_max_body_size: " << server_conf.client_max_body_size << std::endl;
			// exit(0);
		}
		else if (token == SERVER_NAME)
		{
			while (ss >> token && !this->isDirective(token) &&token != ";" )
				server_conf.server_names.push_back(token);
            if (token != ";")
                throw std::runtime_error("Config Error: Missing ';' after 'server_name' directive.");
		}
		else if (token == ERROR_PAGES)
		{
			int code;
			std::string path;
			ss >> code >> path;
			server_conf.error_pages[code] = path;
			if (!(ss >> token) || token != ";")
				throw std::runtime_error("Config Error: Missing ';' after 'error_page' directive.");
		}
		else if (token == LOCATION)
		{
			server_conf.locations.push_back(parseLocationBlock(ss));
		}
		else
			throw std::runtime_error("Config Error: Unknown directive '" + token + "' in server block.");
	}
	if(!server_conf.client_max_body_size_set)
	{
		server_conf.client_max_body_size = 10485760;
		server_conf.client_max_body_size_set = true;
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
			while (ss >> token && this->isMethod(token) && token != ";")
				location_conf.allowed_methods.push_back(token);
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
		else if(token == "auth_required")
		{
			std::string val;
			if(location_conf.auth_set)
				throw std::runtime_error("Config Error: Duplicate 'auth_required' directive in location.");
			ss >> val;
			location_conf.auth_required = (val == "on");
			location_conf.auth_set = true;
			if(!(ss >> token) || token != ";")
				throw std::runtime_error("Config Error: Missing ';' after 'auth_required' directive.");
		}
		else
			throw std::runtime_error("Config Error: Unknown directive '" + token + "' in location block.");
	}
	if (token != "}")
		throw std::runtime_error("Config Error: Location block not closed properly.");

	if(!location_conf.root_set && !location_conf.redirection_set)
		throw std::runtime_error("Config Error: 'root' directive is required in location block.");
	if(location_conf.allowed_methods.empty() && !location_conf.redirection_set)
		throw std::runtime_error("Config Error: 'allow_methods' directive is required in location block.");
	if(std::find(location_conf.allowed_methods.begin(), location_conf.allowed_methods.end(), "POST") != location_conf.allowed_methods.end()
		&& !location_conf.upload_path_set && !location_conf.cgi_set)
		throw std::runtime_error("Config Error: 'upload_path' directive is required when POST method is allowed.");
	if(!location_conf.autoindex_set)
	{
		location_conf.autoindex = false;
		location_conf.autoindex_set = true;
	}
	if(!location_conf.auth_set)
	{
		location_conf.auth_required = false;
		location_conf.auth_set = true;
	}

	return location_conf;
}


void ConfigParser::validateServers() const
{
    std::map<std::string, std::set<std::string> > serverNameMap;
    std::set<std::string> defaultServers;
    std::set<std::string> serversHost;

    for (size_t i = 0; i < this->_servers.size(); ++i){
        const ServerConfigs& server = this->_servers[i];
        std::set<std::string> locationPaths;

        for (size_t j = 0; j < server.locations.size(); ++j)
            if (!locationPaths.insert(server.locations[j].path).second)
                throw std::runtime_error("Config Error: Duplicate location path '" + server.locations[j].path + "' in a server block.");


        for (size_t p = 0; p < server.ports.size(); ++p){
            std::stringstream ss;
            ss << server.host << ":" << server.ports[p];
			// std::cout << "server.host + ports: '" << ss.str() <<"'"<< std::endl;
            std::string listenKey = ss.str();
            if (server.server_names.empty())
            {
                if (defaultServers.count(listenKey))
                    throw std::runtime_error("Config Error: Multiple default servers defined for " + listenKey);
                defaultServers.insert(listenKey);
            }
            else
                for (size_t j = 0; j < server.server_names.size(); ++j)
                    if (!serverNameMap[listenKey].insert(server.server_names[j]).second)
                        throw std::runtime_error("Config Error: Duplicate server_name '" + server.server_names[j] + "' for " + listenKey);
        }

		if(!server.host.empty())
		{
			if(serversHost.count(server.host))
				throw std::runtime_error("Config Error: Duplicate host '" + server.host + "' in server blocks.");
			serversHost.insert(server.host);
		}

    }
}
