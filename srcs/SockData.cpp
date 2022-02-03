/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockData.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 18:47:48 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/03 18:11:09 by fhamel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SockData.hpp"


SockData::SockData(void)
{
	initActiveSet();
	initReadSet();
	initWriteSet();
	red = "\033[0;31m";
	green = "\033[0;32m";
	blue = "\033[0;34m";
	white = "\033[0m";
}

SockData::SockData(const SockData &sockData)
	{ *this = sockData; }

SockData::~SockData(void)
	{ return ; }

SockData	&SockData::operator=(const SockData &sockData)
{
	servers_ = sockData.servers_;
	sockListen_ = sockData.sockListen_;
	response_ = sockData.response_;
	// chunk_ = sockData.chunk_;
	clients_ = sockData.clients_;
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

void	SockData::setResponse(int fd)
{
	Request	*request = requestParser(clients_[fd].getRequest(), servers_);
	std::vector<Server>::iterator	it = servers_.begin();
	std::vector<Server>::iterator	ite = servers_.end();
	for (; it != ite; ++it) {
		if (vector_contains_str(it->getServerName(), request->getHost())) {
			Response	response(*it);
			response.makeAnswer(*request);
			response_[fd] = response.getResponse();
			break;
		}
	}
	delete request;
	clients_[fd].getTmpRequest().clear();
	clients_[fd].getRequest().clear();
	clients_[fd].setChunk(false);
	FD_SET(fd, &writeSet_);
}

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

bool	SockData::isChunkFd(int fd) const
{
	fd = (int)fd;
	return false;
}

bool	SockData::isChunkRequest(std::string request) const
{
	request = (std::string)request;
	return false;
}

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
	int			newSock;
	sockaddr_in addrClient;
	socklen_t	addrClientLen = sizeof(addrClient);
	if ((newSock = accept(fd, reinterpret_cast<sockaddr*>(&addrClient),
	&addrClientLen)) == ERROR) {
		cnxFailed();
	}
	else {
		SockClient	sockClient;
		// std::cout << "test\n";
		// std::cout << inet_ntoa(addrClient.sin_addr);
		// std::cout << "test\n";
		sockClient.setIp(inet_ntoa(addrClient.sin_addr));
		sockClient.setPort(ntohs(addrClient.sin_port));
		if (newSock > FD_SETSIZE) {
			close(newSock);
			cnxRefused(sockClient);
		}
		else {
			FD_SET(newSock, &activeSet_);
			clients_[newSock] = sockClient;
			cnxAccepted(sockClient);
		}
	}
}

void	SockData::readClient(int fd)
{
	char		buffer[BUF_SIZE];
	int			ret = read(fd, buffer, BUF_SIZE - 1);
	if (ret == ERROR) {
		close(fd);
		cnxCloseRead(fd);
		clients_.erase(fd);
		FD_CLR(fd, &activeSet_);
		return ;
	}
	else if (ret == 0) {
		close(fd);
		cnxCloseRead2(fd);
		clients_.erase(fd);
		FD_CLR(fd, &activeSet_);
		return ;
	}
	buffer[ret] = '\0';
	clients_[fd].getTmpRequest() += std::string(buffer);
	if (ret < BUF_SIZE - 1) {
		clients_[fd].getRequest() += clients_[fd].getTmpRequest();
		if (!clients_[fd].isChunk()) {
			/* it's a new request: 1) it's a chunk 2) it's not */
			if (clients_[fd].isTmpRequestChunk()) {
				/* start of a chunk request */
				clients_[fd].setChunk(true);
			}
			else {
				/* not a chunk request: set response for client */
				msgRecv(fd);
				setResponse(fd);
				return ;
			}
		}
		if (clients_[fd].isChunkEof()) {
			msgRecv(fd);
			setResponse(fd);
			return ;
		}
		clients_[fd].getTmpRequest().clear();
	}
}

void	SockData::writeClient(int fd)
{
	if (write(fd, response_[fd].c_str(), response_[fd].size()) == ERROR) {
		response_.erase(fd);
		FD_CLR(fd, &activeSet_);
		FD_CLR(fd, &writeSet_);
		close(fd);
		cnxCloseWrite(fd);
	}
	else {
		response_.erase(fd);
		FD_CLR(fd, &writeSet_);
		msgSent(fd);
	}
}

void	SockData::cnxFailed(void)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << red;
	std::cout << "Server: connection with client failed";
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

void	SockData::cnxRefused(SockClient sockClient)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << red;
	std::cout << "Server: connection refused from " << sockClient.getIp();
	std::cout << " on port " << sockClient.getPort();
	std::cout << ": too many clients connected" << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

void	SockData::cnxAccepted(SockClient sockClient)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << green;
	std::cout << "Server: connection from " << sockClient.getIp();
	std::cout << " on port " << sockClient.getPort() << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

void	SockData::cnxCloseRead(int fd)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << red;
	std::cout << "Server: couldn't read request from " << clients_[fd].getIp();
	std::cout << " on port " << clients_[fd].getPort();
	std::cout << " | socket fd: " << fd;
	std::cout << " | closing connection";
	std::cout << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

void	SockData::cnxCloseRead2(int fd)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << red;
	std::cout << "Server: EOF sent by " << clients_[fd].getIp();
	std::cout << " on port " << clients_[fd].getPort();
	std::cout << " | socket fd: " << fd;
	std::cout << " | closing connection";
	std::cout << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

void	SockData::cnxCloseWrite(int fd)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << red;
	std::cout << "Server: couldn't write response to " << clients_[fd].getIp();
	std::cout << " on port " << clients_[fd].getPort();
	std::cout << " | socket fd: " << fd;
	std::cout << " | closing connection";
	std::cout << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

void	SockData::msgRecv(int fd)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << blue;
	std::cout << "Server: new message from " << clients_[fd].getIp();
	std::cout << " on port " << clients_[fd].getPort();
	std::cout << " | socket fd: " << fd;
	std::cout << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
	std::cout << blue;
	std::cout << "Message: " << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
	std::cout << clients_[fd].getRequest() << std::endl;
}

void	SockData::msgSent(int fd)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << blue;
	std::cout << "Server: message successfully sent to " << clients_[fd].getIp();
	std::cout << " on port " << clients_[fd].getPort();
	std::cout << " | socket fd: " << fd;
	std::cout << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}