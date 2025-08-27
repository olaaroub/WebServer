#include "request.hpp"
#include "Utils.hpp"

Request::Request() : _chunkSize(0), _contentSize(0), _waiting_for_new_chunk(true), state(_InRequestLine), request_ended(false) {}
Request::~Request() {}

void Request::is_finished()
{
	if (state == 2)
		request_ended = true;
	else
		request_ended = false;
}

void Request::ParsRequstLine()
{
	size_t cont = _buffer.find("\r\n");

	if (cont != std::string::npos)
	{
		requestLine.set_line(_buffer.substr(0, cont));
		requestLine.ParsRequestLine();
		_buffer.erase(0, cont + 2);
		state++;
		std::string verction = requestLine.getHttpVerction();
		if (verction != "HTTP/1.1" && verction != "HTTP/1.0")
			throw ParseError("RequestLine Error: verstion of HTTP not seported!", badRequest);
	}
}

void Request::ParsHeaders()
{
	size_t cont = _buffer.find("\r\n\r\n");

	if (_buffer.size() > MAX_HEADERS_SIZE)
		throw ParseError("Request Error: Headers Size too Large!", requestHeaderTooLarge);
	if (cont != std::string::npos)
	{

		headers.set_buffer(_buffer.substr(0, cont + 2));
		_buffer = _buffer.substr(cont + 4);
		headers.HeadersParser();
		headers.cookieParser();
		// cookie here
		if (requestLine.get_method() != "POST")
			request_ended = true;
		else
			request_ended = false;

		state++;
	}
}

std::string Request::_ignoreExtension(std::string line)
{
	size_t index = line.find(";");
	if (index == std::string::npos)
		return line;
	else
		return line.substr(0, index);
}

void Request::ChunkReaContent()
{
	unsigned int len;
	while (true)
	{
		if (_buffer.empty())
			return;
		if (_waiting_for_new_chunk)
		{
			size_t findNewLine = _buffer.find("\r\n");
			if (findNewLine == std::string::npos)
				throw ParseError("Request Error: format of chunked POST not correct", badRequest);
			std::string line = _buffer.substr(0, findNewLine);
			line = _ignoreExtension(line);
			is_number(line);
			std::istringstream ff(line);
			ff >> std::hex >> len;
			_chunkSize = len;
			_buffer.erase(0, findNewLine + 2);
			if (!len)
			{
				request_ended = true;
				return;
			}
			_waiting_for_new_chunk = false;
		}
		if (_buffer.length() < _chunkSize + 2)
			return;
		std::string chunkData = _buffer.substr(0, _chunkSize);
		if (_chunkSize > max_body_size)
			throw ParseError("Request Error: Body too large!", payloadTooLarge);
		max_body_size -= _chunkSize;
		body_content.write(chunkData.c_str(), _chunkSize);
		_buffer.erase(0, _chunkSize + 2);
		_waiting_for_new_chunk = true;
		_chunkSize = 0;
	}
}

void Request::is_number(std::string string)
{
	for (size_t i = 0; i < string.length(); i++)
	{
		if (!std::isxdigit(string[i]) || string[i] == '-')
			throw ParseError("Request Error: size of data passed does not  number!", badRequest);
	}
}

void Request::ContentLenghtRead()
{
	std::string number;

	if (!_chunkSize)
	{
		number = headers.map["content-length"].at(0);
		is_number(number);
		_chunkSize = atol(number.c_str());
		if (_chunkSize == 0)
		{
			body_content.write("\0", 1);
			request_ended = true;
		}
		if (_chunkSize > max_body_size)
			throw ParseError("Request Error: the content lenght too large!", payloadTooLarge);
	}
	if (_buffer.empty())
		return;
	if (_contentSize < _chunkSize)
		_contentSize += _buffer.size();
	if (_contentSize > _chunkSize)
		throw ParseError("Request Error: ContentLenghtRead Error: size of content readed large of content-lenght", badRequest);
	if (_contentSize == _chunkSize)
		request_ended = true;
	body_content.write(_buffer.c_str(), _buffer.length());
	_buffer.clear();
}

void Request::ParsBody()
{
	if (headers.map.find("content-length") != headers.map.end())
		ContentLenghtRead();
	else if (headers.map.find("transfer-encoding") != headers.map.end() && headers.map["transfer-encoding"].at(0) == "chunked")
		ChunkReaContent();
	else
		throw ParseError("Request Error: this method to transfer data not allowed!", badRequest);
}

void Request::StateOFParser()
{
	if (state == _InRequestLine)
		ParsRequstLine();
	if (state == _InHeaders)
		ParsHeaders();
	if (state == _InPost && requestLine.get_method() == "POST")
		ParsBody();
}

bool Request::run_parser(int socket_fd)
{
	char bfr[1024];

	int count = read(socket_fd, &bfr, 1023);
	if (count < 0)
		throw ParseError("Request Error: read failed!", ServerError);
	if (count == 0)
		throw ParseError("Request Error: Client closed connection unexpectedly", closeConnection);
	if (count > 3 && bfr[0] == 0x16 && bfr[1] == 0x03 && bfr[2] >= 0x00 && bfr[2] <= 0x04)
		throw ParseError("Request Error: Client sent a TLS handshake", closeConnection);
	_buffer.append(bfr, count);
	StateOFParser();
	return request_ended;
}