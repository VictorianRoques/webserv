/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockData.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 18:47:48 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/15 01:52:29 by fhamel           ###   ########.fr       */
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
	clients_ = sockData.clients_;
	activeSet_ = sockData.activeSet_;
	readSet_ = sockData.readSet_;
	writeSet_ = sockData.writeSet_;
	return *this;
}

/* setters */
void	SockData::setServers(std::vector<Server> servers)
	{ servers_ = servers; }

/*
** A socket can only listen on one port at a time
** so we bind a socket with every ports specified
*/
void	SockData::setSockListen(std::vector<size_t>	ports)
{
	int					sockListen;
	sockaddr_in			addr;
	for (size_t i = 0; i < ports.size(); ++i) {
		if ((sockListen = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
			perror("socket");
			closeListen(i);
			exit(EXIT_FAILURE);
		}
		int opt = 1;
		if (setsockopt(sockListen, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == ERROR) {
			perror("setsockopt");
			closeListen(i);
			exit(EXIT_FAILURE);
		}
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(ports[i]);
		if (bind(sockListen, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == ERROR) {
			perror("bind");
			closeListen(i);
			exit(EXIT_FAILURE);
		}
		if (listen(sockListen, 1024) == ERROR) {
			perror("listen");
			closeListen(i);
			exit(EXIT_FAILURE);
		}
		sockListen_.push_back(std::make_pair<int, size_t>(sockListen, i));
	}
}

void	SockData::initActiveSet(void)
	{ FD_ZERO(&activeSet_); }

void	SockData::initReadSet(void)
	{ FD_ZERO(&readSet_); }

void	SockData::initWriteSet(void)
	{ FD_ZERO(&writeSet_); }

void	SockData::addActiveSet(int fd)
	{ FD_SET(fd, &activeSet_); }

void	SockData::setReadToActive(void)
	{ readSet_ = activeSet_; }

void	SockData::setDataFd(int fd, Request &request, Server &server)
{
	Response	response(server);
	dataFds_[fd] = response.searchFd(request);
	clients_[fd].getTmpRequest().clear();
	clients_[fd].getRequest().clear();
	clients_[fd].setChunk(false);
	FD_SET(fd, &writeSet_);
}

void	SockData::setResponse(int fd)
{
	Request	request = requestParser(clients_[fd].getRequest(), servers_);
	std::vector<Server>::iterator	it = servers_.begin(), ite = servers_.end();
	for (; it != ite; ++it) {
		if (vector_contains_str(it->getServerName(), request.getHost())) {
			setDataFd(fd, request, *it);
			return ;
		}
	}
	if (clients_[fd].isDeleteRequest()) {
		setDataFd(fd, request, *servers_.begin());
	}
	else {
		setBadRequest(fd);
	}
}

void	SockData::setInternalError(int fd)
{
	int	fd_error_500;
	if ((fd_error_500 = open("../error_pages/500.html", O_RDONLY)) == ERROR) {
		close(fd);
		clients_.erase(fd);
		dataFds_.erase(fd);
		openFailure500(fd);
		return ;
	}
	dataFds_[fd] = fd_error_500;
	FD_SET(fd, &activeSet_);
}

void	SockData::setInternalError(int fd)
{
	int	fd_error_400;
	if ((fd_error_400 = open("../error_pages/400.html", O_RDONLY)) == ERROR) {
		close(fd);
		clients_.erase(fd);
		dataFds_.erase(fd);
		openFailure400(fd);
		return ;
	}
	dataFds_[fd] = fd_error_400;
	FD_SET(fd, &activeSet_);
}

/* checkers */
bool	SockData::isSockListen(int fd) const
{
	std::vector<std::pair<int, size_t> >::const_iterator	it = sockListen_.begin();
	std::vector<std::pair<int, size_t> >::const_iterator	ite = sockListen_.end();
	for (; it != ite; ++it) {
		if (fd == it->first) {
			return true;
		}
	}
	return false;
}

bool	SockData::isSockClient(int fd) const
	{ return (clients_.count(fd) == 1); }

bool	SockData::isReadSet(int fd) const
	{ return FD_ISSET(fd, &readSet_); }

bool	SockData::isWriteSet(int fd) const
	{ return FD_ISSET(fd, &writeSet_); }

bool	SockData::isBeginPipeReady(int fd)
	{ return FD_ISSET(clients_[fd].getBeginPipe(), &writeSet_); }

bool	SockData::isEndPipeReady(int fd)
	{ return FD_ISSET(clients_[fd].getEndPipe(), &readSet_); }

/* getters */
fd_set	*SockData::getReadSet(void)
	{ return &readSet_; }

fd_set	*SockData::getWriteSet(void)
	{ return &writeSet_; }

size_t	SockData::getSizeListen(void) const
	{ return sockListen_.size(); }

int		SockData::getSockListen(size_t index) const
	{ return sockListen_[index].first; }

size_t	SockData::clientsAlloc(void)
{
	std::map<int, SockClient>::iterator	it = clients_.begin();
	std::map<int, SockClient>::iterator	ite = clients_.end();
	size_t	totalSize = 0;
	for (; it != ite; ++it) {
		totalSize += it->second.getTmpRequest().size();
		totalSize += it->second.getRequest().size();
	}
	return totalSize;
}

/* client manager */
/*
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
	tv.tv_sec = 5; tv.tv_usec = 0;
	if ((newSock = accept(fd, reinterpret_cast<sockaddr*>(&addrClient),
	&addrClientLen)) == ERROR) {
		cnxFailed();
	}
	else {
		SockClient	sockClient;
		sockClient.setIp(inet_ntoa(addrClient.sin_addr));
		sockClient.setPort(ntohs(addrClient.sin_port));
		try {
			clients_[newSock] = sockClient;
			if (newSock >= FD_SETSIZE) {
				close(newSock);
				cnxRefused(sockClient);
			}
			else {
				if (setsockopt(newSock, SOL_SOCKET, SO_RCVTIMEO,
				reinterpret_cast<const char*>(&tv), sizeof(tv)) == ERROR) {
					cnxFailed();
				}
				FD_SET(newSock, &activeSet_);
				cnxAccepted(sockClient);
			}
		}
		catch (std::exception &e) {
			setInternalError(fd);
			exceptionError(newSock, e);
		}
	}
}

void	SockData::recvClient(int fd)
{
	char		buffer[BUF_SIZE];
	int			ret = recv(fd, buffer, BUF_SIZE - 1, 0);
	if (ret == ERROR || ret == 0) {
		recvClientClose(fd, ret);
		return ;
	}
	buffer[ret] = '\0';
	try {
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
	catch (std::exception &e) {
		setInternalError(fd);
		exceptionError(fd, e);
	}
}

void	SockData::sendClient(int fd)
{
	char	buffer[BUF_SIZE];
	int		ret;
	if (FD_ISSET(dataFds_[fd], &readSet_)) {
		if ((ret = read(dataFds_[fd], buffer, BUF_SIZE - 1)) == ERROR) {
			clearClient(fd);
			clearDataFd(fd);
			openFailureData(fd);
		}
		buffer[ret] = '\0';
		clients_[fd].getData() += std::string(buffer);
		if (ret < BUF_SIZE - 1) {
			if (send(fd, clients_[fd].getData().c_str(),
			clients_[fd].getData().size(), 0) == ERROR) {
				clearClient(fd);
				cnxCloseSend(fd);
			}
			clearDataFd(fd);
			FD_CLR(fd, &writeSet_);
		}
	}
	// else if (isBeginPipeReady(fd) && !isEndPipeReady(fd)) {
	// 	SockExec sockExec = setSockExec(fd);
	// 	startCgi(sockExec);
	// }
	// else if (isBeginPipeReady(fd) && isEndPipeReady(fd)) {
	// 	writeToCgi(fd);
	// }
}

/* client manager utils */
void	SockData::recvClientClose(int fd, int ret)
{
	clearClient(fd);
	if (ret == ERROR) {
		cnxCloseRecv(fd);
	}
	else {
		cnxCloseRecv2(fd);
	}
}

void	SockData::clearDataFd(int fd)
{
	close(dataFds_[fd]);
	FD_CLR(dataFds_[fd], &activeSet_);
	dataFds_.erase(fd);
}

void	SockData::clearClient(int fd)
{
	close(fd);
	clients_.erase(fd);
	dataFds_.erase(fd);
	FD_CLR(fd, &activeSet_);
	FD_CLR(fd, &writeSet_);
}

/* msg connection */
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
	std::cout << " via port " << sockClient.getPort();
	std::cout << ": too many clients connected" << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

void	SockData::cnxAccepted(SockClient sockClient)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << green;
	std::cout << "Server: connection from " << sockClient.getIp();
	std::cout << " via port " << sockClient.getPort() << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

/* msg close recv */
void	SockData::cnxCloseRecv(int fd)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << red;
	std::cout << "Server: couldn't receive request from " << clients_[fd].getIp();
	std::cout << " via port " << clients_[fd].getPort();
	std::cout << " | socket fd: " << fd;
	std::cout << " | closing connection";
	std::cout << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

void	SockData::cnxCloseRecv2(int fd)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << red;
	std::cout << "Server: EOF sent by " << clients_[fd].getIp();
	std::cout << " via port " << clients_[fd].getPort();
	std::cout << " | socket fd: " << fd;
	std::cout << " | closing connection";
	std::cout << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

/* msg close send */
void	SockData::cnxCloseSend(int fd)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << red;
	std::cout << "Server: couldn't send response to " << clients_[fd].getIp();
	std::cout << " on port " << clients_[fd].getPort();
	std::cout << " | socket fd: " << fd;
	std::cout << " | closing connection";
	std::cout << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

/* msg recv/send */
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

/* msg exception */
void	SockData::exceptionError(int fd, std::exception &e)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << red;
	std::cout << "Server: " << e.what() << ": " << clients_[fd].getIp();
	std::cout << " on port " << clients_[fd].getPort();
	std::cout << " | socket fd: " << fd;
	std::cout << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

void	SockData::openFailure500(int fd)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << red;
	std::cout << "Server: couldn't open '../error_pages/500.html'";
	std::cout << " | socket fd " << fd;
	std::cout << " | closing connection"; 
	std::cout << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

void	SockData::openFailure400(int fd)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << red;
	std::cout << "Server: couldn't open '../error_pages/400.html'";
	std::cout << " | socket fd " << fd;
	std::cout << " | closing connection"; 
	std::cout << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

void	SockData::openFailureData(int fd)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << red;
	std::cout << "Server: couldn't open data file";
	std::cout << " | socket fd " << fd;
	std::cout << " | closing connection"; 
	std::cout << std::endl;
	std::cout << white;
	std::cout << "-----------------------------" << std::endl;
}

/* utils */

/*
** If an error occurs before the server is actually started,
** all the socket fds are closed before exit
*/

void	SockData::closeListen(size_t endInd)
{
	for (size_t i = 0; i < endInd; ++i) {
		close(sockListen_[i].first);
	}
}

void	SockData::printBuffer(char buffer[BUF_SIZE]) const
{
	std::cout << red;
	std::cout << "buffer: \n";
	std::cout << white;
	std::cout << "#################################" << std::endl;
	std::cout << buffer << std::endl;
	std::cout << "#################################" << std::endl;
}
