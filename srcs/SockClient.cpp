/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/02 16:58:14 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/05 02:09:12 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SockClient.hpp"

SockClient::SockClient(void) : chunk_(false)
	{ return ; }

SockClient::~SockClient(void)
	{ return ; }

SockClient	&SockClient::operator=(const SockClient &sockClient)
{
	ip_ = sockClient.ip_;
	port_ = sockClient.port_;
	request_ = sockClient.request_;
	tmpRequest_ = sockClient.tmpRequest_;
	chunk_ = sockClient.chunk_;
	return *this;
}

/* setters */
void	SockClient::setIp(char *ip)
	{ ip_ = ip; }

void	SockClient::setPort(size_t port)
	{ port_ = port; }

void	SockClient::setChunk(bool chunk)
	{ chunk_ = chunk; }

/* checkers */
bool	SockClient::isChunk(void) const
	{ return chunk_; }

#include <iostream>

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
	std::cout << "line content: " << std::endl;
	std::string::const_iterator	it = line.begin(), ite = line.end();
	for (; it != ite; ++it) {
		std::cout << static_cast<int>(*it) << " | ";
	}
	std::cout << std::endl;
	if (line == "0" || line == "0\r") {
		return true;
	}
	return false;
}

/* getters */
char 	*SockClient::getIp(void) const
	{ return ip_; }

size_t	SockClient::getPort(void) const
	{ return port_; }

std::string	&SockClient::getTmpRequest(void)
	{ return tmpRequest_; }

std::string	&SockClient::getRequest(void)
	{ return request_; }