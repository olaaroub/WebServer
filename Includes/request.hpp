#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Headers.hpp"
#include "RequestLine.hpp"

class Request
{
private:
	std::string 								_buffer;
	unsigned long 								_chunkSize;
	unsigned long 								_contentSize;
	bool 										_waiting_for_new_chunk;

	void 										is_finished();
	void 										is_number(std::string string);
	void 										ParsRequstLine();
	void 										ParsHeaders();
	void 										ParsBody();
	void 										StateOFParser();
	void 										ChunkReaContent();
	void 										ContentLenghtRead();
	std::string 								_ignoreExtension(std::string line);

	enum ParserStute
	{
		_InRequestLine,
		_InHeaders,
		_InPost
	};

public:
	Headers 									headers;
	RequestLine 								requestLine;
	short 										state;
	bool 										request_ended;
	unsigned long  								max_body_size;
	std::stringstream 							body_content;
	bool 										run_parser(int socket_fd);
	Request();
	~Request();
};

#endif