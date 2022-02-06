/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockData.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorianroques <victorianroques@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 18:47:48 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/06 20:11:20 by victorianro      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SockData.hpp"


SockData::SockData(void)
{
	initActiveSet();
	initRecvSet();
	initSendSet();
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
	clients_ = sockData.clients_;
	activeSet_ = sockData.activeSet_;
	recvSet_ = sockData.recvSet_;
	sendSet_ = sockData.sendSet_;
	return *this;
}

/* setters */
void	SockData::setServers(std::vector<Server> servers)
	{ servers_ = servers; }

void	SockData::initActiveSet(void)
	{ FD_ZERO(&activeSet_); }

void	SockData::initRecvSet(void)
	{ FD_ZERO(&recvSet_); }

void	SockData::initSendSet(void)
	{ FD_ZERO(&sendSet_); }

void	SockData::addActiveSet(int fd)
	{ FD_SET(fd, &activeSet_); }

void	SockData::setSockListen(std::vector<int> sockListen)
	{ sockListen_ = sockListen; }

void	SockData::setRecvToActive(void)
	{ recvSet_ = activeSet_; }

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
	FD_SET(fd, &sendSet_);
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

bool	SockData::isRecvSet(int fd) const
	{ return FD_ISSET(fd, &recvSet_); }

bool	SockData::isSendSet(int fd) const
	{ return FD_ISSET(fd, &sendSet_); }

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
fd_set	*SockData::getRecvSet(void)
	{ return &recvSet_; }

fd_set	*SockData::getSendSet(void)
	{ return &sendSet_; }

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
	struct timeval	tv;
	tv.tv_sec = 10; tv.tv_usec = 0;
	if ((newSock = accept(fd, reinterpret_cast<sockaddr*>(&addrClient),
	&addrClientLen)) == ERROR) {
		cnxFailed();
	}
	else {
		SockClient	sockClient;
		sockClient.setIp(inet_ntoa(addrClient.sin_addr));
		sockClient.setPort(ntohs(addrClient.sin_port));
		if (newSock > FD_SETSIZE) {
			close(newSock);
			cnxRefused(sockClient);
		}
		else {
			if (setsockopt(newSock, SOL_SOCKET, SO_RCVTIMEO,
			reinterpret_cast<const char*>(&tv), sizeof(tv)) == ERROR) {
				cnxFailed();
			}
			FD_SET(newSock, &activeSet_);
			clients_[newSock] = sockClient;
			cnxAccepted(sockClient);
		}
	}
}

void	SockData::recvClient(int fd)
{
	char		buffer[BUF_SIZE];
	int			ret = recv(fd, buffer, BUF_SIZE - 1, 0);
	if (ret == ERROR || ret == 0) {
		close(fd);
		if (ret == ERROR) {
			cnxCloseRead(fd);
		}
		else {
			cnxCloseRead2(fd);
		}
		clients_.erase(fd);
		FD_CLR(fd, &activeSet_);
		return ;
	}
	buffer[ret] = '\0';
	std::cout << red;
	std::cout << "buffer: \n";
	std::cout << white;
	std::cout << "#################################" << std::endl;
	std::cout << buffer << std::endl;
	std::cout << "#################################" << std::endl;
	clients_[fd].getTmpRequest() += std::string(buffer);
	if (ret < BUF_SIZE - 1) {
		clients_[fd].getRequest() += clients_[fd].getTmpRequest();
		if (!clients_[fd].isChunk()) {
			if (clients_[fd].isTmpRequestChunk()) {
				clients_[fd].setChunk(true);
			}
			else {
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

void	SockData::sendClient(int fd)
{
	if (send(fd, response_[fd].c_str(), response_[fd].size(), 0) == ERROR) {
		close(fd);
		clients_.erase(fd);
		FD_CLR(fd, &activeSet_);
		FD_CLR(fd, &sendSet_);
		cnxCloseWrite(fd);
	}
	else {
		FD_CLR(fd, &sendSet_);
		msgSent(fd);
	}
	response_.erase(fd);
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