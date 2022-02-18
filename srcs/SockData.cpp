/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockData.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 18:47:48 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/18 02:53:28 by fhamel           ###   ########.fr       */
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
	clients_[fd].setRequest(request);
	clients_[fd].setResponseHeader(response.getResponseHeader());
	clients_[fd].setResponse(response);
	clients_[fd].getTmpRequest().clear();
	clients_[fd].getFinalRequest().clear();
	clients_[fd].setChunk(false);
	if (dataFds_[fd] != CGI) {
		FD_SET(dataFds_[fd], &activeSet_);
	}
	FD_SET(fd, &writeSet_);
}

void	SockData::setResponse(int fd)
{
	try {
		Request	request = requestParser(clients_[fd].getFinalRequest(), servers_);
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
	catch (std::exception &e) {
		exceptionError(fd, e);
		setInternalError(fd);
	}
}

void	SockData::setInternalError(int fd)
{
	int	fd_error_500;
	if ((fd_error_500 = open("error_pages/500.html", O_RDONLY)) == ERROR) {
		clearClient(fd);
		openFailure500(fd);
		return ;
	}
	dataFds_[fd] = fd_error_500;
	FD_SET(fd, &activeSet_);
}

void	SockData::setBadRequest(int fd)
{
	int	fd_error_400;
	if ((fd_error_400 = open("error_pages/400.html", O_RDONLY)) == ERROR) {
		clearClient(fd);
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

bool	SockData::isReadingOver(int ret) const
	{ return (ret < BUF_SIZE - 1); }

bool	SockData::isBeginPipeReady(int fd)
	{ return FD_ISSET(clients_[fd].getBeginPipe(), &writeSet_); }

bool	SockData::isEndPipeReady(int fd)
	{ return FD_ISSET(clients_[fd].getEndPipe(), &readSet_); }

bool	SockData::isFileRequest(int fd)
	{ return (dataFds_[fd] != CGI && FD_ISSET(dataFds_[fd], &readSet_)); }

/* getters */
fd_set	*SockData::getReadSet(void)
	{ return &readSet_; }

fd_set	*SockData::getWriteSet(void)
	{ return &writeSet_; }

size_t	SockData::getSizeListen(void) const
	{ return sockListen_.size(); }

int		SockData::getSockListen(size_t index) const
	{ return sockListen_[index].first; }

/* client manager */
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
			if (setsockopt(newSock, SOL_SOCKET, SO_NOSIGPIPE,
			reinterpret_cast<const char*>(&tv), sizeof(tv)) == ERROR) {
				cnxFailed();
			}
			FD_SET(newSock, &activeSet_);
			cnxAccepted(sockClient);
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
	clients_[fd].getTmpRequest() += std::string(buffer);
	if (isReadingOver(ret)) {
		clients_[fd].getFinalRequest() += clients_[fd].getTmpRequest();
		if (!clients_[fd].isChunk()) {
			if (clients_[fd].isTmpRequestChunk()) {
				clients_[fd].setChunk(true);
			}
			else {
				requestReceived(fd);
				return ;
			}
		}
		if (clients_[fd].isChunkEof()) {
			requestReceived(fd);
			return ;
		}
		clients_[fd].getTmpRequest().clear();
	}
}

void	SockData::sendClient(int fd)
{
	if (isFileRequest(fd)) {
		std::cout << "fileRequest() called" << std::endl;
		fileRequest(fd);
	}
	else if (dataFds_[fd] == CGI) {
		std::cout << "cgiRequest() called" << std::endl;
		cgiRequest(fd);
	}
	else if (dataFds_[fd] == STR_DATA) {
		// strDataRequest(fd);
	}
}

/* requests */
void	SockData::fileRequest(int fd)
{
	char	buffer[BUF_SIZE];
	int		ret;
	if ((ret = read(dataFds_[fd], buffer, BUF_SIZE - 1)) == ERROR) {
		clearClient(fd);
		clearDataFd(fd);
		openFailureData(fd);
		return ;
	}
	buffer[ret] = '\0';
	clients_[fd].getResponseBody() += std::string(buffer, ret);
	if (isReadingOver(ret)) {
		if (clients_[fd].isCgi()) {
			clients_[fd].getResponseHeader().setCgiStatus(clients_[fd].getResponseBody());
			clients_[fd].getResponseBody() = cleanBody(clients_[fd].getResponseBody());
		}
		clients_[fd].getResponseHeader().setBodyLength(clients_[fd].getResponseBody().size());
		clients_[fd].getResponseHeader().writeHeader();
		std::string	data = clients_[fd].getResponseHeader().getHeader() + clients_[fd].getResponseBody();
		if (send(fd, data.c_str(), data.size(), 0) == ERROR) {
			clearClient(fd);
			cnxCloseSend(fd);
		}
		if (clients_[fd].isCgi()) {
			std::stringstream	ss;
			ss << fd;
			std::string	pathFile = "./cgi_binary/.cgi_output_" + ss.str();
			close(clients_[fd].getOutputFd());
			clients_[fd].setCgi(false);
			// unlink(pathFile.c_str());
		}
		clearDataFd(fd);
		clients_[fd].getResponseBody().clear();
		FD_CLR(fd, &writeSet_);
	}
}

void	SockData::cgiRequest(int fd)
{
	if (!isBeginPipeReady(fd)) {
		// Need to pass Server to cgiHandler
		cgiHandler cgi(clients_[fd].getRequest());
		if (cgi.startCgi(fd, clients_[fd].getEndPipe()) == ERROR) {
			close(clients_[fd].getEndPipe());
			close(clients_[fd].getBeginPipe());
			clearClient(fd);
			clearDataFd(fd);
			return ;
		}
		std::stringstream	ss;
		ss << fd;
		std::string	pathFile = "./cgi_binary/.cgi_output_" + ss.str();
		int outputFd = open(pathFile.c_str(), O_RDONLY);
		clients_[fd].setOutputFd(outputFd);
		FD_SET(clients_[fd].getBeginPipe(), &writeSet_);
	}
	else if (isBeginPipeReady(fd)) {
		writeToCgi(fd);
		close(clients_[fd].getBeginPipe());
		FD_CLR(clients_[fd].getBeginPipe(), &writeSet_);
		dataFds_[fd] = clients_[fd].getOutputFd();
		FD_SET(clients_[fd].getOutputFd(), &activeSet_);
	}
}

// void	SockData::strDataRequest(int fd)
// {
// 	std::string	data = getResponse();
// 	write(fd, data.c_str(), data.size());
// 	FD_CLR(fd, &writeSet_);
// }

void	SockData::requestReceived(int fd)
{
	msgRecv(fd);
	setResponse(fd);
	if (dataFds_[fd] == CGI) {
		setUpCgi(fd);
	}
	else {
		FD_SET(dataFds_[fd], &activeSet_);
	}
}

/* cgi */
void	SockData::writeToCgi(int fd)
{
	if (write(clients_[fd].getBeginPipe(), clients_[fd].getRequest().getBody().c_str(),
	clients_[fd].getRequest().getBody().size()) == ERROR) {
		clearClient(fd);
		clearDataFd(fd);
		writeFailure(fd);
	}
}

void	SockData::setUpCgi(int fd)
{
	clients_[fd].setCgi(true);
	int	fdTmp[2];
	if (pipe(fdTmp) == ERROR) {
		clearClient(fd);
		clearDataFd(fd);
		setInternalError(fd);
		pipeFailure(fd);
		return ;
	}
	clients_[fd].getEndPipe() = fdTmp[0];
	clients_[fd].getBeginPipe() = fdTmp[1];
}

/* utils */
void	SockData::recvClientClose(int fd, int ret)
{
	if (ret == ERROR) {
		cnxCloseRecv(fd);
	}
	else {
		cnxCloseRecv2(fd);
	}
	clearClient(fd);
}

void	SockData::clearDataFd(int fd)
{
	close(dataFds_[fd]);
	FD_CLR(dataFds_[fd], &activeSet_);
	FD_CLR(dataFds_[fd], &writeSet_);
	dataFds_.erase(fd);
}

void	SockData::clearClient(int fd)
{
	close(fd);
	FD_CLR(fd, &activeSet_);
	FD_CLR(fd, &writeSet_);
	clients_.erase(fd);
}

void	SockData::closeListen(size_t endInd)
{
	for (size_t i = 0; i < endInd; ++i) {
		close(sockListen_[i].first);
	}
}

std::string	SockData::cleanBody(std::string &responseBody)
	{ return responseBody.substr(responseBody.find("\r\n\r\n") + 4); }

/* msg connection */
void	SockData::cnxFailed(void)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << red;
	std::cerr << "Server: connection with client failed";
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
}

void	SockData::cnxRefused(SockClient sockClient)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << red;
	std::cerr << "Server: connection refused from " << sockClient.getIp();
	std::cerr << " via port " << sockClient.getPort();
	std::cerr << ": too many clients connected" << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
}

void	SockData::cnxAccepted(SockClient sockClient)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << green;
	std::cerr << "Server: connection from " << sockClient.getIp();
	std::cerr << " via port " << sockClient.getPort() << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
}

/* msg close recv */
void	SockData::cnxCloseRecv(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << red;
	std::cerr << "Server: couldn't receive request from " << clients_[fd].getIp();
	std::cerr << " via port " << clients_[fd].getPort();
	std::cerr << " | socket fd: " << fd;
	std::cerr << " | closing connection";
	std::cerr << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
}

void	SockData::cnxCloseRecv2(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << red;
	std::cerr << "Server: EOF sent by " << clients_[fd].getIp();
	std::cerr << " via port " << clients_[fd].getPort();
	std::cerr << " | socket fd: " << fd;
	std::cerr << " | closing connection";
	std::cerr << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
}

/* msg close send */
void	SockData::cnxCloseSend(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << red;
	std::cerr << "Server: couldn't send response to " << clients_[fd].getIp();
	std::cerr << " on port " << clients_[fd].getPort();
	std::cerr << " | socket fd: " << fd;
	std::cerr << " | closing connection";
	std::cerr << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
}

/* msg recv/send */
void	SockData::msgRecv(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << blue;
	std::cerr << "Server: new message from " << clients_[fd].getIp();
	std::cerr << " on port " << clients_[fd].getPort();
	std::cerr << " | socket fd: " << fd;
	std::cerr << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << blue;
	std::cerr << "Message: " << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << clients_[fd].getFinalRequest() << std::endl;
}

void	SockData::msgSent(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << blue;
	std::cerr << "Server: message successfully sent to " << clients_[fd].getIp();
	std::cerr << " on port " << clients_[fd].getPort();
	std::cerr << " | socket fd: " << fd;
	std::cerr << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
}

/* msg exception */
void	SockData::exceptionError(int fd, std::exception &e)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << red;
	std::cerr << "Server: " << e.what() << ": " << clients_[fd].getIp();
	std::cerr << " on port " << clients_[fd].getPort();
	std::cerr << " | socket fd: " << fd;
	std::cerr << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
}

void	SockData::openFailure500(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << red;
	std::cerr << "Server: couldn't open '../error_pages/500.html'";
	std::cerr << " | socket fd " << fd;
	std::cerr << " | closing connection"; 
	std::cerr << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
}

void	SockData::openFailure400(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << red;
	std::cerr << "Server: couldn't open '../error_pages/400.html'";
	std::cerr << " | socket fd " << fd;
	std::cerr << " | closing connection"; 
	std::cerr << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
}

void	SockData::openFailureData(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << red;
	std::cerr << "Server: couldn't open data file";
	std::cerr << " | socket fd " << fd;
	std::cerr << " | closing connection"; 
	std::cerr << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
}

void	SockData::pipeFailure(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << red;
	std::cerr << "Server: couldn't pipe";
	std::cerr << " | socket fd " << fd;
	std::cerr << " | closing connection"; 
	std::cerr << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
}

void	SockData::writeFailure(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << red;
	std::cerr << "Server: couldn't write";
	std::cerr << " | socket fd " << fd;
	std::cerr << " | closing connection"; 
	std::cerr << std::endl;
	std::cerr << white;
	std::cerr << "-----------------------------" << std::endl;
}
