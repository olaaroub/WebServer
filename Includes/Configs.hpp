/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configs.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 18:29:07 by olaaroub          #+#    #+#             */
/*   Updated: 2025/07/01 19:46:42 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib.hpp"

struct LocationConfigs
{
	LocationConfigs();
	std::string path;
    std::string root;
    bool        autoindex;
    std::string index_file;
    std::vector<std::string> allowed_methods;
    int         redirection_code;
    std::string redirection_url;

    std::map<std::string, std::string> cgi_handlers;
    std::string upload_path;

    bool root_set;
    bool autoindex_set;
    bool index_file_set;
    bool redirection_set;
    bool upload_path_set;
    bool cgi_set;
};


struct ServerConfigs
{
	ServerConfigs();
	int         port;
    std::string host;
    std::vector<std::string> server_names;
    long        client_max_body_size;
    std::map<int, std::string> error_pages;
    std::vector<LocationConfigs> locations;

    bool listen_set;
    bool client_max_body_size_set;

};


