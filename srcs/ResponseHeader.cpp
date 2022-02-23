#include "ResponseHeader.hpp"

ResponseHeader::ResponseHeader(): _bodyLength(0) {}
ResponseHeader::ResponseHeader(const ResponseHeader &resHeader) { *this = resHeader; }
ResponseHeader::~ResponseHeader() {}

ResponseHeader&     ResponseHeader::operator=(const ResponseHeader &resHeader)
{
    _status = resHeader._status;
    _contentType = resHeader._contentType;
    _bodyLength = resHeader._bodyLength;
    _date = resHeader._date;
    _location = resHeader._location;
    _header = resHeader._header;
    return *this;
}

std::string     ResponseHeader::getStatus() const       { return _status; }
std::string     ResponseHeader::getContentType() const  { return _contentType; }
size_t          ResponseHeader::getBodyLength() const   { return _bodyLength; }
std::string     ResponseHeader::getDate() const         { return _date; }
std::string     ResponseHeader::getLocation() const     { return _location; }
std::string     ResponseHeader::getHeader() const       { return _header; }

void            ResponseHeader::setStatusError(std::string code, size_t len) 
{
    if (code == "400")
        _status = "400 Bad Request";
    else if (code == "413")
        _status = "413 Payload Too Large";
    else if (code == "403")
        _status = "403 Forbidden";
    else if (code == "404")
        _status = "404 Not Found";
    else if (code == "405")
        _status = "405 Method Not Allowed";
    else if (code == "500")
        _status = "500 Internal Error";
    _contentType = "text/html";
    _bodyLength = len;
}

void            ResponseHeader::setStatus(std::string status)           {_status = status; }
void            ResponseHeader::setContentType(std::string contentType) {_contentType = contentType; }
void            ResponseHeader::setBodyLength(size_t length)            {_bodyLength = length; }
void            ResponseHeader::setDate()                               { _date = getTime(); }
void            ResponseHeader::setLocation(std::string location)       { _location = location; }
void            ResponseHeader::setHeader(std::string status, std::string contentType, size_t length)
{
    _status = status;
    _contentType = contentType;
    _bodyLength = length;
}
void            ResponseHeader::setRes(std::string head) { _header = head; }
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
    if (_header.empty() == false)
        return ;
    _header = "HTTP/1.1 " + _status + "\r\n";
    if (_contentType.empty() == false)
        _header += "Content-Type: " + _contentType + "\r\n";
    if (_bodyLength >= 0)
        _header += "Content-Length: " + sizeToString(_bodyLength) + "\r\n";
    if (_location.empty() == false)
        _header += "Location: " + _location + "\r\n";
	if (_date.empty() == false)
		_header += "Date: " + _date + "\r\n";
    _header += "\r\n";
}

void            ResponseHeader::setCgiStatus(std::string body)
{
    setCgiHeader(body.substr(0, body.find("\r\n\r\n")));
}

void			ResponseHeader::searchContentType(std::string path)
{
	if (_contentType != "")
		return ;
    
	std::string type = path.substr(path.rfind(".") + 1, path.size() - path.rfind("."));
	if (type == "html")
		_contentType = "text/html";
	else if (type == "css")
		_contentType = "text/css";
	else if (type == "js")
		_contentType = "text/javascript";
	else if (type == "jpeg" || type == "jpg")
		_contentType = "image/jpeg";
	else if (type == "png")
		_contentType = "image/png";
	else if (type == "bmp")
		_contentType = "image/bmp";
	else
		_contentType = "text/plain";
}