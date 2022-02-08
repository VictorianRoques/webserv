#include "ResponseHeader.hpp"

ResponseHeader::ResponseHeader() {}

std::string     ResponseHeader::getStatus() { return _status; }
std::string     ResponseHeader::getContentType() { return _contentType; }
size_t          ResponseHeader::getBodyLength() { return _bodyLength; }
std::string     ResponseHeader::getDate() { return _date; }
std::string     ResponseHeader::getLocation() { return _location; }
std::string     ResponseHeader::getHeader() { return _header; }

void            ResponseHeader::setStatusError(std::string code, size_t length) 
{
    if (code == "400")
        _status = "400 Bad Request";
    else if (code == "413")
        _status = "413 Payload Too Large";
    else if (code == "403")
        _status = "403 Forbidden";
    else if (code == "404")
        _status = "404 Not Found";
    else if (code == "500")
        _status = "500 Internal Servor Error";
    _contentType = "text/html";
    _bodyLength = length;
}

void            ResponseHeader::setStatusRedirect(std::string code, size_t length)
{
    if (code == "310")
    {
        _status = "310 Too Many Redirects";
        _contentType = "text/html";
        _bodyLength = length;
    }
    else if (code == "308")
    {
        _status = "308 Permanent Redirect";
        _contentType = "";
        _bodyLength = 0;
    }
}

void            ResponseHeader::setStatus(std::string status) {_status = status; }
void            ResponseHeader::setContentType(std::string contentType) {_contentType = contentType; }
void            ResponseHeader::setBodyLength(size_t length) {_bodyLength = length; }

void            ResponseHeader::setHeader(std::string status, std::string contentType, size_t length)
{
    _status = status;
    _contentType = contentType;
    _bodyLength = length;
}

void               ResponseHeader::setCgiHeader(std::string cgiHeader)
{
    size_t pos = cgiHeader.find("Status: ");
	if (pos != std::string::npos)
		_status = cgiHeader.substr(pos + 8, cgiHeader.find("\n", pos));
	pos = cgiHeader.find("Content-type: ") + 14;
	_contentType = cgiHeader.substr(pos, cgiHeader.find("\n", pos));
}

void            ResponseHeader::writeHeader()
{
    _header = "HTTP/1.1 " + _status + "\r\n";
    if (_contentType.empty() == false)
        _header += "Content-Type: " + _contentType + "\r\n";
    if (_bodyLength > 0)
        _header += "Content-Length: " + sizeToString(_bodyLength) + "\r\n";
    if (_location.empty() == false)
        _header += "Location: " + _location + "\r\n";
	if (_date.empty() == false)
		_header += "Date: " + _date + "\r\n";
    _header += "\r\n";
}


