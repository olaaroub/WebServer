/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configs.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 18:29:07 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/29 21:45:27 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib.hpp"

struct LocationConfigs
{
	LocationConfigs();
	std::string 							path;
	std::string 							root;
	std::string 							index_file;
	std::string 							redirection_url;
	std::string 							upload_path;
	std::vector<std::string> 				allowed_methods;
	std::map<std::string, std::string>		cgi_handlers;

	bool 									autoindex;
	int 									redirection_code;
	bool 									auth_required;
	bool 									root_set;
	bool 									autoindex_set;
	bool 									index_file_set;
	bool 									redirection_set;
	bool 									upload_path_set;
	bool 									cgi_set;
	bool 									auth_set;
};

struct ServerConfigs
{
	ServerConfigs();
	std::string 							host;
	long 									client_max_body_size;
	bool 									host_set;
	bool 									client_max_body_size_set;
	std::vector<int> 						ports;
	std::vector<std::string>				server_names;
	std::map<int, std::string> 				error_pages;
	std::vector<LocationConfigs> 			locations;

};
