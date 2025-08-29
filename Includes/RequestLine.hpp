#ifndef REQUESTLINE_HPP
#define REQUESTLINE_HPP

#include "lib.hpp"

class RequestLine
{
private:
	std::string _method;
	std::string _HttpVerction;
	std::string _line;
	std::string _url;

	void SeparateUrlAndQuerys(std::string buff);
	;

public:
	void ParsRequestLine();
	void set_line(std::string line);
	std::string &get_line();
	std::string queryLine;
	std::string getHttpVerction();
	const std::string &get_method() const;
	const std::string &getUrl() const;
};

#endif