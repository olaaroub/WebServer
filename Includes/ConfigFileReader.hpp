/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileReader.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: olaaroub <olaaroub@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 12:01:23 by olaaroub          #+#    #+#             */
/*   Updated: 2025/08/26 20:23:18 by olaaroub         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "lib.hpp"

class FileReader{
public:
	FileReader(const std::string &path);
	~FileReader();

	const std::string					&getContent() const;

private:
	FileReader();
	std::string 						_confPath;
	std::string							_confContent;

	void 								validateFile() const;
	void 								readFile();
};