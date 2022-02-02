/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockData.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorianroques <victorianroques@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 18:47:48 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/02 10:35:09 by victorianro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SockData.hpp"


SockData::SockData(void)
{
	initActiveSet();
	initReadSet();
	initWriteSet();
}

SockData::SockData(const SockData &sockData)
	{ *this = sockData; }

SockData::~SockData(void)
	{ return ; }

SockData	&SockData::operator=(const SockData &sockData)
{
	servers_ = sockData.servers_;
	sockListen_ = sockData.sockListen_;
	requestStr_ = sockData.requestStr_;
	answer_ = sockData.answer_;
	activeSet_ = sockData.activeSet_;
	readSet_ = sockData.readSet_;
	writeSet_ = sockData.writeSet_;
	return *this;
}

/* setters */
void	SockData::setServers(std::vector<Server> servers)
	{ servers_ = servers; }

void	SockData::initActiveSet(void)
	{ FD_ZERO(&activeSet_); }

void	SockData::initReadSet(void)
	{ FD_ZERO(&readSet_); }

void	SockData::initWriteSet(void)
	{ FD_ZERO(&writeSet_); }

void	SockData::addActiveSet(int fd)
	{ FD_SET(fd, &activeSet_); }

void	SockData::setSockListen(std::vector<int> sockListen)
	{ sockListen_ = sockListen; }

void	SockData::setReadToActive(void)
	{ readSet_ = activeSet_; }

/* checkers */
bool	SockData::isSockListen(int fd) const
{
	std::vector<int>::const_iterator	it = sockListen_.begin();
	std::vector<int>::const_iterator	ite = sockListen_.end();
	for (; it != ite; ++it) {
		if (fd == *it) {
			return true;
		}
	}
	return false;
}

bool	SockData::isReadSet(int fd) const
	{ return FD_ISSET(fd, &readSet_); }

bool	SockData::isWriteSet(int fd) const
	{ return FD_ISSET(fd, &writeSet_); }

/* getters */
fd_set	*SockData::getReadSet(void)
	{ return &readSet_; }

fd_set	*SockData::getWriteSet(void)
	{ return &writeSet_; }

size_t	SockData::getSizeListen(void) const
	{ return sockListen_.size(); }

int		SockData::getSockListen(size_t index) const
	{ return sockListen_[index]; }

/*
** client manager
**
** After finding there was some activity on a listening fd
** a new socket is created in order to create a connexion with the client.
** This fd is then added to the active set of fds to be monitored
*/
void	SockData::addClient(int fd)
{
	std::string	red = "\033[0;31m";
	std::string	green = "\033[0;32m";
	std::string white = "\033[0m";
	int			sockClient;
	sockaddr_in addrClient;
	socklen_t	addrClientLen = sizeof(addrClient);
	if ((sockClient = accept(fd, reinterpret_cast<sockaddr*>(&addrClient),
	&addrClientLen)) == ERROR) {
		std::cout << "-----------------------------" << std::endl;
		std::cout << red;
		std::cout << "Server: connection with client failed";
		std::cout << white;
		std::cout << "-----------------------------" << std::endl;
	}
	else {
		if (sockClient > FD_SETSIZE) {
			std::cout << "-----------------------------" << std::endl;
			std::cout << red;
			std::cout << "Server: connection refused from " << inet_ntoa(addrClient.sin_addr);
			std::cout << " on port " << ntohs(addrClient.sin_port);
			std::cout << ": too many clients connected" << std::endl;
			std::cout << white;
			std::cout << "-----------------------------" << std::endl;
			close(sockClient);
		}
		else {
			std::cout << "-----------------------------" << std::endl;
			std::cout << green;
			std::cout << "Server: connection from " << inet_ntoa(addrClient.sin_addr);
			std::cout << " on port " << ntohs(addrClient.sin_port) << std::endl;
			std::cout << white;
			std::cout << "-----------------------------" << std::endl;
			FD_SET(sockClient, &activeSet_);
		}
	}
}

/*
**
*/
void	SockData::readClient(int fd)
{
	std::string	red = "\033[0;31m";
	std::string	blue = "\033[0;34m";
	std::string white = "\033[0m";
	char		buffer[BUF_SIZE];
	int			ret = read(fd, buffer, BUF_SIZE - 1);
	if (ret == ERROR || ret == 0) {
		std::cout << "-----------------------------" << std::endl;
		std::cout << red;
		std::cout << "Server: client " << fd;
		std::cout << ": connection lost" << std::endl;
		std::cout << white;
		std::cout << "-----------------------------" << std::endl;
		close(fd);
		FD_CLR(fd, &activeSet_);
	}
	else {
		buffer[ret] = '\0';
		requestStr_ += std::string(buffer);
		if (ret != BUF_SIZE - 1) {
			std::cout << "-----------------------------" << std::endl;
			std::cout << blue;
			std::cout << "Server: new message from ";
			std::cout << "client " << fd << ":" <<std::endl;
			std::cout << white;
			std::cout << "-----------------------------" << std::endl;
			std::cout << requestStr_ << std::endl;
			std::cout << "-----------------------------" << std::endl;
			FD_SET(fd, &writeSet_);
			Request	*request = requestParser(requestStr_, servers_);
			std::vector<Server>::iterator	it = servers_.begin();
			std::vector<Server>::iterator	ite = servers_.end();
			for (; it != ite; ++it) {
				if (vector_contains_str(it->getServerName(), request->getHost())) {
					Response	response(*it);
					response.makeAnswer(*request);
					answer_[fd] = response.getResponse();
					break;
				}
			}
			if (it == ite)
				answer_[fd] = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length:37\r\n\r\n<html><body>Bad Request</body></html>";
			requestStr_.clear();
			delete request;
		}
	}
}

void	SockData::writeClient(int fd)
{
	std::string	red = "\033[0;31m";
	std::string	blue = "\033[0;34m";
	std::string white = "\033[0m";
	if (write(fd, answer_[fd].c_str(), answer_[fd].size()) == ERROR) {
		std::cout << "-----------------------------" << std::endl;
		std::cout << red;
		std::cout << "Server: couldn't write response to ";
		std::cout << "client " << fd << ": " <<std::endl;
		std::cout << "closing connection" << std::endl;
		std::cout << white;
		std::cout << "-----------------------------" << std::endl;
		close(fd);
		answer_.erase(fd);
		FD_CLR(fd, &activeSet_);
		FD_CLR(fd, &writeSet_);
	}
	else {
		std::cout << "-----------------------------" << std::endl;
		std::cout << blue;
		std::cout << "Server: message successfully sent to ";
		std::cout << "client " << fd << ": " <<std::endl;
		std::cout << white;
		std::cout << "-----------------------------" << std::endl;
		answer_.erase(fd);
		FD_CLR(fd, &writeSet_);
	}
}
