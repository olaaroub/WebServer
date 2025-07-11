/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Configs.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 19:30:44 by olaaroub          #+#    #+#             */
/*   Updated: 2025/07/01 19:48:10 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Configs.hpp"

LocationConfigs::LocationConfigs()
	: autoindex(false), redirection_code(0),
	  root_set(false), autoindex_set(false),
	  index_file_set(false), redirection_set(false),
	  upload_path_set(false), cgi_set(false) {}

ServerConfigs::ServerConfigs()
	:host("0.0.0.0"), client_max_body_size(1048576),
	  host_set(false), client_max_body_size_set(false) { ; }