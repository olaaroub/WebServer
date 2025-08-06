/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 14:55:18 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/06 21:50:08 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib.hpp"


std::string joinPaths(const std::string& p1, const std::string& p2);
std::string normalizePath(const std::string& uri);
std::string getExtension(const std::string& path);
std::string getFileContents(const std::string& filePath);
 std::string trimWhitespace(const std::string& s);