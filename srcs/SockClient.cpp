/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/02 16:58:14 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/23 03:47:34 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SockClient.hpp"

SockClient::SockClient(void) : chunk_(false), totalLength_(0)
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
	tmpRequest_ = sockClient.tmpRequest_;
	finalRequest_ = sockClient.finalRequest_;
	request_ = sockClient.request_;
	response_ = sockClient.response_;
	server_ = sockClient.server_;
	return *this;
}

/* setters */
void	SockClient::setIp(char *ip)
	{ ip_ = ip; }

void	SockClient::setPort(size_t port)
	{ port_ = port; }

void	SockClient::setChunk(bool chunk)
	{ chunk_ = chunk; }

void	SockClient::setRequest(const Request &request)
	{ request_ = request; }

void	SockClient::setResponse(const Response &response)
	{ response_ = response; }

void	SockClient::setServer(const Server &server)
	{ server_ = server; }

/* checkers */
bool	SockClient::isChunk(void) const
	{ return chunk_; }

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

/* getters */
char 	*SockClient::getIp(void) const
	{ return ip_; }

size_t	SockClient::getPort(void) const
	{ return port_; }

int	&SockClient::getRequestType(void)
	{ return requestType_; }

std::string	&SockClient::getTmpRequest(void)
	{ return tmpRequest_; }

std::string	&SockClient::getFinalRequest(void)
	{ return finalRequest_; }

unsigned int	&SockClient::getTotalLength(void)
	{ return totalLength_; }

Request	&SockClient::getRequest(void)
	{ return request_; }

Response	&SockClient::getResponse(void)
	{ return response_; }

Server	&SockClient::getServer(void)
	{ return server_; }

std::string	&SockClient::getData(void)
	{ return data_; }
