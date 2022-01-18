/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request_parser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/17 12:56:34 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/18 17:35:28 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request_Parser.hpp"

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Request::Request() {}

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
		_body = x._body;
	}
	return *this;
}

/**************************************/
//              GETTERS               //
/**************************************/

std::string	Request::getMethod() { return _method; }
std::string	Request::getPath() { return _path; }
std::string	Request::getProtocolVersion() { return _protocolVersion; }

size_t		Request::getContentLength() { return _contentLength; }
std::string Request::getContentType() { return _contentType; }
std::string	Request::getAcceptEncoding() { return _acceptEncoding; }

vec_str	Request::getBody() { return _body; }

/**************************************/
//              SETTERS               //
/**************************************/

void	Request::setMethod(std::string method) { _method = method; }
void	Request::setPath(std::string path) { _path = path; }
void	Request::setProtocolVersion(std::string protocolVersion) { _protocolVersion = protocolVersion; }

void	Request::setContentLength(size_t contentLength) { _contentLength = contentLength; }
void	Request::setContentType(std::string contentType) { _contentType = contentType; }
void	Request::setAcceptEncoding(std::string acceptEncoding) { _acceptEncoding = acceptEncoding; }

void	Request::setBody(vec_str body) { _body = body; }

/**************************************/
//              PARSING               //
/**************************************/

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

//	vars.push_back(std::make_pair("Content-Length", &Request::setContentLength));
	vars.push_back(std::make_pair("Content-Type", &Request::setContentType));
	vars.push_back(std::make_pair("Accept-Encoding", &Request::setAcceptEncoding));

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
	std::cout << COLOR_REQ << "Protocol Version: " << NC << _protocolVersion << std::endl;
	std::cout << std::endl;
	std::cout << COLOR_REQ << "Content Length: " << NC << _contentLength << std::endl;
	std::cout << COLOR_REQ << "Content Type: " << NC << _contentType << std::endl;
	std::cout << COLOR_REQ << "Accept Encoding: " << NC << _acceptEncoding << std::endl;
	std::cout << std::endl;
	std::cout << COLOR_REQ << "Body: " << NC; ft_putvec(_body, " "); std::cout << std::endl;
}

void	requestParser(std::string rq) {
	Request		request;
	std::string	line;
	int			i = 0;
	size_t 		pos;

	request.setContentLength(rq.length());
	while (rq.find("\r\n") != 0) {
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
	request.print_request();
}
