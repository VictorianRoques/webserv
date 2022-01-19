/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request_parser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/17 12:56:34 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/19 18:51:00 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request_Parser.hpp"

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Request::Request(): _chunked(false) {}

Request::~Request() {}

Request::Request(const Request &x) { *this = x; }

Request& Request::operator=(const Request &x) {
	if (this != &x) {
		_method = x._method;
		_path = x._path;
		_protocolVersion = x._protocolVersion;
		_contentLength = x._contentLength;
		_contentType = x._contentType;
		_acceptEncoding = x._acceptEncoding;
		_transferEncoding = x._transferEncoding;
		_cacheControl = x._cacheControl;
		_connection = x._connection;
		_host = x._host;
		_body = x._body;
		_chunked = x._chunked;
	}
	return *this;
}

/**************************************/
//              GETTERS               //
/**************************************/

std::string	Request::getMethod() { return _method; }
std::string	Request::getPath() { return _path; }
std::string	Request::getProtocolVersion() { return _protocolVersion; }

std::string	Request::getContentLength() { return _contentLength; }
std::string Request::getContentType() { return _contentType; }
std::string	Request::getAcceptEncoding() { return _acceptEncoding; }
std::string Request::getTransferEncoding() { return _transferEncoding; }
std::string Request::getCacheControl() { return _cacheControl; }
std::string Request::getConnection() { return _connection; }
std::string Request::getHost() { return _host; }

vec_str		Request::getBody() { return _body; }

bool		Request::getChunked() { return _chunked; }
std::string	Request::getFullPath() { return _fullPath; }

/**************************************/
//              SETTERS               //
/**************************************/

void	Request::setMethod(std::string method) { _method = method; }
void	Request::setPath(std::string path) { _path = path; }
void	Request::setProtocolVersion(std::string protocolVersion) { _protocolVersion = protocolVersion; }

void	Request::setContentLength(std::string contentLength) { _contentLength = contentLength; }
void	Request::setContentType(std::string contentType) { _contentType = contentType; }
void	Request::setAcceptEncoding(std::string acceptEncoding) { _acceptEncoding = acceptEncoding; }
void	Request::setTransferEncoding(std::string transferEncoding) { _transferEncoding = transferEncoding; }
void	Request::setCacheControl(std::string cacheControl) { _cacheControl = cacheControl; }
void	Request::setConnection(std::string connection) { _connection = connection; }
void	Request::setHost(std::string host) { _host = host; }

void	Request::setBody(vec_str body) { _body = body; }

void	Request::setChunked(bool chunked) { _chunked = chunked; }
void	Request::setFullPath(std::string fullPath) { _fullPath = fullPath; }

/**************************************/
//              PARSING               //
/**************************************/

/**
 * isChunked(): sets chunked = true/false
**/
void	Request::isChunked() {
	if (_contentLength.size() || _transferEncoding.size())
		setChunked(true);
}

/**
 * buildFullPath(): find the relevant Location {} and append correct prefix (root) to path
**/
void	Request::buildFullPath(std::vector<Location *> loc) {
	std::vector<Location *>::iterator it = loc.begin();
	for (; it != loc.end(); it++) {
		if (it.getLocationMatch() == _path) {
			_fullPath = it.getRoot + _path;
			return ;
		}
	}
	// non exact match found: looking for the longest match	
	it = loc.begin();
	size_t max_match = 0;
	size_t match = 0;
	for (; it != loc.end(); it++) {
		match = str_match_size(_path, it.getLocationMatch());
		if (max_match < match) {
			_fullPath = it.getRoot + _path;
			max_match = match;
		}
	}
	return ;
}

void	Request::requestLine(std::string line) {
	vec_str	rl;

	rl = ft_split(line, " ");
	setMethod(rl[0]);
	setPath(rl[1]);
	setProtocolVersion(rl[2]);
}

void	Request::headerLine(std::string line) {

	std::vector<std::pair<std::string, methodPointer> > vars;
	methodPointer mp;

	vars.push_back(std::make_pair("Content-Length", &Request::setContentLength));
	vars.push_back(std::make_pair("Transfer-Encoding", &Request::setTransferEncoding));
	vars.push_back(std::make_pair("Content-Type", &Request::setContentType));
	vars.push_back(std::make_pair("Accept-Encoding", &Request::setAcceptEncoding));
	vars.push_back(std::make_pair("Cache-Control", &Request::setCacheControl));
	vars.push_back(std::make_pair("Connection", &Request::setConnection));
	vars.push_back(std::make_pair("Host", &Request::setHost));

	std::map<std::string, std::string> hl;
	hl = ft_map_split(line, " ");

	std::vector<std::pair<std::string, methodPointer> >::iterator it = vars.begin();
	std::map<std::string, std::string>::iterator hi = hl.begin();
	for (; it != vars.end(); it++) {
		if (it->first == hi->first) {
			mp = it->second;
			(this->*mp)(hi->second);
		}
	}
}

void	Request::bodyLine(std::string line) {
	setBody(ft_split(line, "\r\n"));
}

void	Request::print_request() {
	std::cout << COLOR_REQ << "Method: " << NC << _method << std::endl;
	std::cout << COLOR_REQ << "Path: " << NC << _path << std::endl;
	std::cout << COLOR_REQ << "Full Path: " << NC << _fullPath << std::endl;
	std::cout << COLOR_REQ << "Protocol Version: " << NC << _protocolVersion << std::endl;
	std::cout << std::endl;
	std::cout << COLOR_REQ << "Transfer Encoding: " << NC << _transferEncoding << std::endl;
	std::cout << COLOR_REQ << "Content Length: " << NC << _contentLength << std::endl;
	std::cout << COLOR_REQ << "Content Type: " << NC << _contentType << std::endl;
	std::cout << COLOR_REQ << "Accept Encoding: " << NC << _acceptEncoding << std::endl;
	std::cout << COLOR_REQ << "Cache Control: " << NC << _cacheControl << std::endl;
	std::cout << COLOR_REQ << "Connection: " << NC << _connection << std::endl;
	std::cout << COLOR_REQ << "Host: " << NC << _host << std::endl;
	std::cout << COLOR_REQ << "Chunked: " << NC << (_chunked ? "Yes" : "No") << std::endl;
	std::cout << std::endl;
	std::cout << COLOR_REQ << "Body: " << NC; ft_putvec(_body, " "); std::cout << std::endl;
}

/**
 * requestParser(): rq is the 'Request Header' string
**/
void	requestParser(std::string rq) {
	Request		request;
	std::string	line;
	int			i = 0;
	size_t 		pos;
	std::vector<Server server>::iterator it = servers_g.begin();

	while (rq.find("\r\n") != 0 && !rq.empty()) {
		i++;
		pos = rq.find("\r\n");
		line = rq.substr(0, pos);
		rq.erase(0, pos + 2);
		if (i == 1)
			request.requestLine(line);
		else
			request.headerLine(line);
	}
	line = rq.substr(0, std::string::npos);
	if (rq.length() > 4)
		request.bodyLine(line);
	request.isChunked();
	for (; it != server_g.end(); it++) {
		if (it.getServerName() == request.getHost())
			request.buildFullPath(it);
	}

	request.print_request();
}
