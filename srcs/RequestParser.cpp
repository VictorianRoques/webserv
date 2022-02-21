/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 14:44:32 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/21 16:36:27 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"
#include "Server.hpp"
#include "Location.hpp"

/**************************************/
//           EXCEPTIONS               //
/**************************************/

char const *Request::NoHostException::what() const throw() { return "Missing a 'Host:' directive in the request header."; }

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Request::Request(): _chunked(false), _tooBig(false), _redirCode(0) {}

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
		_secFetchDest = x._secFetchDest;
		_body = x._body;
		_generalRoot = x._generalRoot;
		_chunked = x._chunked;
		_tooBig = x._tooBig;
		_fullPath = x._fullPath;
		_queryString = x._queryString;
		_uploadDest = x._uploadDest;
		_redirCode = x._redirCode;
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
std::string Request::getSecFetchDest() { return _secFetchDest; }

std::string	Request::getBody() { return _body; }

bool		Request::getChunked() { return _chunked; }
bool		Request::getTooBig() { return _tooBig; }
std::string	Request::getFullPath() { return _fullPath; }
std::string	Request::getQueryString() { return _queryString; }
std::string	Request::getUploadDest() { return _uploadDest; }
std::string	Request::getGeneralRoot() { return _generalRoot; }
size_t		Request::getRedirCode() { return _redirCode; }

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
void	Request::setSecFetchDest(std::string secFetchDest) { _secFetchDest = secFetchDest; }

void	Request::setBody(std::string body) { _body = body; }

void	Request::setChunked(bool chunked) { _chunked = chunked; }
void	Request::setTooBig(bool tooBig) { _tooBig = tooBig; }
void	Request::setFullPath(std::string fullPath) { _fullPath = fullPath; }
void	Request::setQueryString(std::string queryString) { _queryString = queryString; }
void	Request::setUploadDest(std::string uploadDest) { _uploadDest = uploadDest; }
void	Request::setGeneralRoot(std::string generalRoot) { _generalRoot = generalRoot; }
void	Request::setRedirCode(size_t redirCode) { _redirCode = redirCode; }

/**************************************/
//              PARSING               //
/**************************************/

/**
 * findRightLocation(): find the relevant Location {}
 **/
Location findRightLocation(std::vector<Location> loc, Request req) {
	if (req.getPath()[0] != '/')
		req.setPath("/" + req.getPath());
	std::vector<Location>::iterator it = loc.begin();
	// if queryString attached to path, remove it
	if (req.getPath().find("?") != std::string::npos) {
		req.setPath(req.getPath().substr(0, req.getPath().find("?")));
//		std::cout << "YOYOY " << req.getPath() << std::endl;
	}
	// looking for exact match
	for (; it != loc.end(); it++) {
//	std::cout << "YOOOOOOOOOOOOOOOO" << std::endl;
//	std::cout << "LocationMatch = " << it->getLocationMatch() << " and getPath = " << req.getPath() << std::endl;
		if (it->getLocationMatch() == req.getPath()) {
			return *it;
		}
	}
	// no exact match found: looking for the longest match	
	it = loc.begin();
	size_t pos = req.getPath().length();

	while (pos != 0) {
		pos = req.getPath().rfind("/", pos - 1);
		for (; it != loc.end(); it++) {
			if (!strncmp(req.getPath().c_str(), it->getLocationMatch().c_str(), pos))
				return *it ;
			if (it->getLocationMatch() == "/")
				return *it;
		}
	}
	return *it;
}

/**
 * findRightServer(): finds correct server
 **/
Server findRightServer(std::vector<Server> servers_g, Request request) {
	std::vector<Server>::iterator it = servers_g.begin();
	for (; it != servers_g.end(); it++) {
		if (vector_contains_str(it->getServerName(), request.getHost())) {
			return *it;
		}
	}
	// if no exact match found: first server is default server
	return *(servers_g.begin());

}

/**
 * isChunked(): sets chunked = true/false
 **/
void	Request::isChunked() {
	if (_transferEncoding.find("chunked") != std::string::npos)
		setChunked(true);
}

/**
 * buildFullPath(): append correct root to path without query string (symbol: '?')
 **/
void	Request::buildFullPath(Location loc) {

//	loc.print_loc();
	if (getRedirCode() == 308) { // if redirection 
		if (loc.getRedirection().second[0] == '/') { //absolute
			_fullPath = loc.getRedirection().second;
			std::cout << "ABSOLUTE REDIR" << std::endl;
		}
		else { //relative
			_fullPath = loc.getRoot() + "/" + _path;
			std::cout << "relative REDIR" << std::endl;
		}
	}
	else {
		if (_contentType.find("multipart/form-data; boundary=") != std::string::npos) { // check if this is an upload
				_fullPath = findRightPath(loc.getIndex(), loc.getRoot(), loc.getAutoIndex(), loc.getIndex());
				_uploadDest = loc.getUploadDest();
				std::cout << "FULL PATH = " << _fullPath << " et uploadDest = " << _uploadDest;
		}
		else {
			std::cout << "GET RIGHT PATH" << std::endl;
			_fullPath = findRightPath(_path, loc.getRoot(), loc.getAutoIndex(), loc.getIndex());
		}
	}
	_fullPath = cleanSlash(_fullPath);
	return ;
}

/**
 * queryString(): parse the path and store query string if any.
 **/
void	Request::queryString() {
	size_t	pos;

	if ((pos = _path.find("?")) != std::string::npos) {
		_queryString = _path.substr(pos + 1, std::string::npos);
	}
}

/**
 * firstLine(): parse the first line, for ex.: "GET /index.html HTTP/1.1"
 **/
void	Request::firstLine(std::string line) {
	vec_str	rl;

	rl = ft_split(line, " ");
	setMethod(rl[0]);
	setPath(rl[1]);
	setProtocolVersion(rl[2]);
}

/**
 * headerLine(): parse the rest of the header
 **/
void	Request::headerLine(std::string line) {

	std::vector<std::pair<std::string, methodPointer> > vars;
	methodPointer mp;

	vars.push_back(std::make_pair("Content-Length", &Request::setContentLength));
	vars.push_back(std::make_pair("Transfer-Encoding", &Request::setTransferEncoding));
	vars.push_back(std::make_pair("Content-Type", &Request::setContentType));
	vars.push_back(std::make_pair("Accept-Encoding", &Request::setAcceptEncoding));
	vars.push_back(std::make_pair("Cache-Control", &Request::setCacheControl));
	vars.push_back(std::make_pair("Connection", &Request::setConnection));
	vars.push_back(std::make_pair("Sec-Fetch-Dest", &Request::setSecFetchDest));
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

/**
 * bodyLine(): parse the body.
 **/
void	Request::bodyLine(std::string line) {
	setBody(line);
}

/**
 *  print_request(): self-explanatory
 **/
void	Request::print_request() {
	std::cout << COLOR_REQ << "/**************************************/" << NC << std::endl;
	std::cout << COLOR_REQ << "//            PARSED REQUEST          //" << NC << std::endl;
	std::cout << COLOR_REQ << "/**************************************/" << NC << std::endl << std::endl;
	std::cout << COLOR_REQ << "Method: " << NC << _method << std::endl;
	std::cout << COLOR_REQ << "Path: " << NC << _path << std::endl;
	std::cout << COLOR_REQ << "Full Path: " << NC << _fullPath << std::endl;
	std::cout << COLOR_REQ << "Query String: " << NC << _queryString << std::endl;
	std::cout << COLOR_REQ << "Protocol Version: " << NC << _protocolVersion << std::endl;
	std::cout << std::endl;
	std::cout << COLOR_REQ << "Transfer Encoding: " << NC << _transferEncoding << std::endl;
	std::cout << COLOR_REQ << "Content Length: " << NC << _contentLength << std::endl;
	std::cout << COLOR_REQ << "Content Type: " << NC << _contentType << std::endl;
	std::cout << COLOR_REQ << "Accept Encoding: " << NC << _acceptEncoding << std::endl;
	std::cout << COLOR_REQ << "Cache Control: " << NC << _cacheControl << std::endl;
	std::cout << COLOR_REQ << "Connection: " << NC << _connection << std::endl;
	std::cout << COLOR_REQ << "Host: " << NC << _host << std::endl;
	std::cout << COLOR_REQ << "Sec-Fetch-Dest: " << NC << _secFetchDest << std::endl;
	std::cout << COLOR_REQ << "Chunked: " << NC << (_chunked ? "Yes" : "No") << std::endl;
	std::cout << COLOR_REQ << "Redir Code: " << NC << _redirCode << std::endl;
	std::cout << std::endl;
	std::cout << COLOR_REQ << "Body: " << NC << _body << std::endl;
}

/**
 * requestParser(): rq is the 'request.Header' string and servers_g is the parsed config
 **/
Request requestParser(std::string rq, std::vector<Server> servers_g) {
	Server	serv;
	Location	loc;
	Request	request;
	std::string	line;
	int			i = 0;
	size_t 		pos;

	while (rq.find("\r\n") != 0 && !rq.empty()) {
		i++;
		pos = rq.find("\r\n");
		line = rq.substr(0, pos);
		rq.erase(0, pos + 2);
		if (i == 1)
			request.firstLine(line);
		else
			request.headerLine(line);
	}

	serv = findRightServer(servers_g, request);
	loc = findRightLocation(serv.getLocation(), request);

	// check if there is a body
	if (rq.length() > 2)
		request.bodyLine(rq.substr(2));

	// check if bodySize is too big
	if ((int)request.getBody().length() > loc.getMaxBodySize()) {
		request.setTooBig(true);
		return request;
	}

	// handle redirection
	i = 0; // i = nb of redirecitons
	while (loc.getRedirection().first == 308) {
		i++;
		// If i >= 10: status code 310
		if (i >= 10) {
			request.setRedirCode(310);
			return request;
		}
		request.setPath(loc.getRedirection().second);
		std::cout << "new redir Path = " << request.getPath() << std::endl;
		request.setRedirCode(loc.getRedirection().first);
		if (request.getPath()[0] == '/') // if absolute path, no further redirection possible
			break ;
		loc = findRightLocation(serv.getLocation(), request);
	}
	
	// handle the rest
	request.isChunked();
	request.buildFullPath(loc);
	request.queryString();
	request.setGeneralRoot(serv.getGeneralRoot());
	request.print_request();

	return request;
}