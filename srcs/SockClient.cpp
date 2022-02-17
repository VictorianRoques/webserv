/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/02 16:58:14 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/17 17:23:39 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SockClient.hpp"

SockClient::SockClient(void) : chunk_(false), cgi_(false)
	{ return ; }

SockClient::SockClient(const SockClient &sockClient)
	{ *this = sockClient; }

SockClient::~SockClient(void)
	{ return ; }

SockClient	&SockClient::operator=(const SockClient &sockClient)
{
	ip_ = sockClient.ip_;
	port_ = sockClient.port_;
	chunk_ = sockClient.chunk_;
	cgi_ = sockClient.cgi_;
	tmpRequest_ = sockClient.tmpRequest_;
	request_ = sockClient.request_;
	responseBody_ = sockClient.responseBody_;
	responseHeader_ = sockClient.responseHeader_;
	fd_[0] = sockClient.fd_[0];
	fd_[1] = sockClient.fd_[1];
	return *this;
}

/* setters */
void	SockClient::setIp(char *ip)
	{ ip_ = ip; }

void	SockClient::setPort(size_t port)
	{ port_ = port; }

void	SockClient::setChunk(bool chunk)
	{ chunk_ = chunk; }

void	SockClient::setCgi(bool cgi)
	{ cgi_ = cgi; }

void	SockClient::setRequest(const Request &request)
	{ request_ = request; }

void	SockClient::setResponseHeader(const ResponseHeader &responseHeader)
	{ responseHeader_ = responseHeader; }

void	SockClient::setResponse(const Response &response)
	{ response_ = response; }

void	SockClient::setOutputFd(int outputFd)
	{ outputFd_ = outputFd; }

/* checkers */
bool	SockClient::isChunk(void) const
	{ return chunk_; }

bool	SockClient::isCgi(void) const
	{ return cgi_; }

bool	SockClient::isTmpRequestChunk(void) const
{
	std::istringstream	data(tmpRequest_);
	std::string			line;
	while (std::getline(data, line)) {
		if (line == "\r") {
			break ;
		}
		if (line == "Transfer-Encoding: chunked\r") {
			return true;
		}
	}
	return false;
}

bool	SockClient::isChunkEof(void) const
{
	std::istringstream	data(tmpRequest_);
	std::string			line;
	std::getline(data, line);
	if (line == "0\r") {
		return true;
	}
	return false;
}

bool	SockClient::isDeleteRequest(void) const
{
	std::istringstream	data(finalRequest_);
	std::vector<std::string>	arr;
	std::string					line;
	size_t						pos = 0;
	std::getline(data, line);
	while ((pos = line.find(" ")) != std::string::npos) {
		arr.push_back(line.substr(0, pos));
		line.erase(0, pos + 1);
	}
	arr.push_back(line);
	if (arr.size() != 3) {
		return false;
	}
	if (arr[0] != "DELETE" || arr[1].find("/") != 0 || arr[2] != "HTTP/1.1\r") {
		return false;
	}
	return true;
}

bool	SockClient::isBadRequest(void) const
{
	std::istringstream	data(finalRequest_);
	std::string			line;
	std::getline(data, line);
	if (line.find("GET") != 0 && line.find("POST") != 0 &&
	line.find("HEAD") != 0 && line.find("DELETE") != 0) {
		return true;
	}
	if (line.find(" HTTP/1.1\r") != line.size() - 11) {
		return true;
	}
	if (line.find("DELETE") == 0) {
		return false;
	}
	while (std::getline(data, line)) {
		if (line.find("Host: ") == 0 && (line != "Host: \r")) {
			return false;
		}
	}
	return true;
}

/* getters */
char 	*SockClient::getIp(void) const
	{ return ip_; }

size_t	SockClient::getPort(void) const
	{ return port_; }

std::string	&SockClient::getTmpRequest(void)
	{ return tmpRequest_; }

std::string	&SockClient::getFinalRequest(void)
	{ return finalRequest_; }

Request	&SockClient::getRequest(void)
	{ return request_; }

ResponseHeader	&SockClient::getResponseHeader(void)
	{ return responseHeader_; }

Response	&SockClient::getResponse(void)
	{ return response_; }

std::string	&SockClient::getResponseBody(void)
	{ return responseBody_; }

int	&SockClient::getBeginPipe(void)
	{ return fd_[1]; }

int	&SockClient::getEndPipe(void)
	{ return fd_[0]; }

int	&SockClient::getOutputFd(void)
	{ return outputFd_; }
