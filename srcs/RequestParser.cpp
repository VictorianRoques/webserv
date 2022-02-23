/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/04 14:44:32 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/23 17:41:58 by viroques         ###   ########.fr       */
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
 * hasLongerLocationMatch(): compare function used in findRightLocationMatch() (see below)
**/
bool hasLongerLocationMatch(Location a, Location b) {
	return a.getLocationMatch().size() > b.getLocationMatch().size();
}

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
	}
	// looking for exact match
	for (; it != loc.end(); it++) {
		if (it->getLocationMatch() == req.getPath()) {
			return *it;
		}
	}
	// no exact match found: looking for the longest match	
	//sorting the Locations from longest Location Match to shortest and then find the longest match.
	std::sort(loc.begin(), loc.end(), hasLongerLocationMatch);
	it = loc.begin();
	std::string path = req.getPath();
	size_t pos;
	for (; it != loc.end(); it++) {
		if ((pos = path.find(it->getLocationMatch())) != std::string::npos)
			return *it;
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
 * needIndex(): checks if need to add index at the end
**/
bool	needIndex(std::string path, std::string root) {
	path = cleanSlash(path + "/");
	root = cleanSlash(root + "/");
	path.erase(path.length() - 1);
	root.erase(root.length() - 1);
	path = path.substr(path.find_last_of("/"));
	root = root.substr(root.find_last_of("/"));
	if (path == root)
		return true;
	return false;
}

/**
 * buildFullPath(): append correct root to path without query string (symbol: '?')
 **/
void	Request::buildFullPath(Location loc) {

	_fullPath = _path.substr(0, _path.find("?"));
	
	//special cases
	if (_fullPath.find_first_not_of('/') == std::string::npos) {
		_fullPath = loc.getRoot();
	}
	if (_contentType.find("multipart/form-data; boundary=") != std::string::npos) {
		_uploadDest = loc.getUploadDest();
	}
	// general cases
	std::string lm = loc.getLocationMatch();
	size_t pos = _fullPath.find(lm);
	if (lm != "/" && pos != std::string::npos) {
		_fullPath.replace(pos, lm.length(), loc.getRoot() + "/");
	}
	if (loc.getAutoIndex() == false && pathIsDirectory(_fullPath) && needIndex(_fullPath, loc.getRoot())) {
		_fullPath += "/" + loc.getIndex();
	}
	if (_fullPath[0] != '/') {
		_fullPath = getPWD() + "/" + _fullPath;
	}
	if (!pathIsDirectory(_fullPath) && !pathIsFile(_fullPath)) {
		_fullPath = loc.getRoot() + "/" + _fullPath;
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
	if (_body.size()< 10000)
		std::cout << COLOR_REQ << "Body: " << NC << _body << std::endl;
	else
		std::cout << COLOR_REQ << "Body: " << NC << "body size too long" << std::endl;
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
		std::string lm = loc.getLocationMatch();
		pos = request.getPath().find(lm);
		
		std::string newPath  = loc.getRedirection().second;
		std::string	path = request.getPath();

		path.replace(pos, lm.length(), newPath + "/");
		path = cleanSlash(path);

		request.setPath(path);
		request.setRedirCode(loc.getRedirection().first);
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
