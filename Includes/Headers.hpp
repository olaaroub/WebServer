#ifndef HEADERS_HPP
#define HEADERS_HPP
#include "lib.hpp"
#include "Utils.hpp"

class Headers{
private:
	std::string 										_buffer;
	void 												AddToMap(std::string line);
	void 												_splitCookie(std::string cookie);
	std::string 										_percentEncoding(std::string cookie);
	int 												_toDecimal(std::string::iterator begin, std::string::iterator end);
	bool 												_isValidHeaderKey(const std::string &key);

public:
	std::string 										getCookie(const std::string &key) const;
	void 												set_buffer(std::string buffer);
	std::string 										get_buffer();
	void 												HeadersParser();
	void 												cookieParser();
	std::map<std::string, std::vector<std::string> > 	map;
	std::map<std::string, std::string>					cookieInfo;
};

#endif