
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SockData.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhamel <fhamel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/28 18:47:48 by fhamel            #+#    #+#             */
/*   Updated: 2022/02/21 16:06:29 by fhamel           ###   ########.fr       */
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

/*******************************/
/*           SETTERS           */
/*******************************/
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
	clients_[fd].setResponse(response);
	clients_[fd].setServer(server);
	clients_[fd].getTmpRequest().clear();
	clients_[fd].getFinalRequest().clear();
	clients_[fd].setChunk(false);
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
		setDataFd(fd, request, *servers_.begin());
	}
	catch (std::exception &e) {
		setInternalError(fd);
		exceptionError(fd, e);
	}
}

void	SockData::setInternalError(int fd)
{
	int	fd_error_500;
	if ((fd_error_500 = open("error_pages/500.html", O_RDONLY)) == ERROR) {
		clearClient(fd);
		systemFailure("opening [error_pages/500.html]", fd);
		return ;
	}
	dataFds_[fd] = fd_error_500;
	clients_[fd].setRequest(Request());
	clients_[fd].setResponse(Response(500));
	clients_[fd].setServer(Server());
}

/*******************************/
/*           CHECKERS          */
/*******************************/
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

/*******************************/
/*           GETTERS           */
/*******************************/
fd_set	*SockData::getReadSet(void)
	{ return &readSet_; }

fd_set	*SockData::getWriteSet(void)
	{ return &writeSet_; }

size_t	SockData::getSizeListen(void) const
	{ return sockListen_.size(); }

int		SockData::getSockListen(size_t index) const
	{ return sockListen_[index].first; }

/*******************************/
/*        CLIENT MANAGER       */
/*******************************/

void	SockData::addClient(int fd)
{
	int			newSock = -42;
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
			cnxRefused(sockClient);
			clearClient(newSock);
		}
		else {
			fcntl(fd, F_SETFL, O_NONBLOCK);
			if (setsockopt(newSock, SOL_SOCKET, SO_NOSIGPIPE,
			reinterpret_cast<const char*>(&tv), sizeof(tv)) == ERROR) {
				cnxFailed();
				clearClient(newSock);
			}
			FD_SET(newSock, &activeSet_);
			cnxAccepted(sockClient);
		}
	}
}

void	SockData::modifyChunkRequest(int fd) {
	std::stringstream	ss;
	ss << clients_[fd].getTotalLength();
	std::string	contentLength = "\r\nContent-Length: " + ss.str();
	size_t	pos = clients_[fd].getFinalRequest().find("\r\n\r\n");
	if (pos == std::string::npos) {
		throw badChunkRequestException();
	}
	clients_[fd].getFinalRequest().insert(pos, contentLength);
	std::string	transferEncoding("Transfer-Encoding: chunked\r\n");
	pos = clients_[fd].getFinalRequest().find(transferEncoding);
	if (pos == std::string::npos) {
		throw badChunkRequestException();
	}
	clients_[fd].getFinalRequest().erase(pos, transferEncoding.size());
}

void	SockData::normalRequest(int fd)
{
	clients_[fd].getFinalRequest() = clients_[fd].getTmpRequest();

	requestReceived(fd);
}

void	SockData::startChunkRequest(int fd)
{
	clients_[fd].setChunk(true);
	size_t	end = clients_[fd].getTmpRequest().find("\r\n\r\n");
	if (end == std::string::npos) {
		clients_[fd].getFinalRequest() = clients_[fd].getTmpRequest();
		requestReceived(fd);
	}
	else {
		clients_[fd].getFinalRequest() += clients_[fd].getTmpRequest().substr(0, end + 4);
		clients_[fd].getTmpRequest() = clients_[fd].getTmpRequest().substr(end + 4);
	}
}

void	SockData::endChunkRequest(int fd)
{
	try {
		modifyChunkRequest(fd);
		requestReceived(fd);
	}
	catch (std::exception &e) {
		exceptionError(fd, e);
		requestReceived(fd);
		return ;
	}
}

void	SockData::concatChunks(int fd)
{
	try {
		unchunk_t	tmpPair = unchunk(clients_[fd].getTmpRequest());
		clients_[fd].getTotalLength() += tmpPair.first;
		clients_[fd].getFinalRequest() += tmpPair.second;
		clients_[fd].getTmpRequest().clear();
	}
	catch (std::exception &e) {
		exceptionError(fd, e);
		requestReceived(fd);
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
	clients_[fd].getTmpRequest() += std::string(buffer, ret);
	if (isReadingOver(ret)) {
		if (!clients_[fd].isChunk()) {
			if (clients_[fd].isTmpRequestChunk()) {
				startChunkRequest(fd);
			}
			else {
				normalRequest(fd);
				return ;
			}
		}
		if (clients_[fd].isChunkEof()) {
			endChunkRequest(fd);
			return ;
		}
		concatChunks(fd);
	}
}

void	SockData::sendClient(int fd)
{
	if (clients_[fd].isDataReady()) {
		sendDataClient(fd);
		return ;
	}
	if (dataFds_[fd] == CGI) {
		cgiRequest(fd);
	}
	else if (dataFds_[fd] == STR_DATA) {
		strDataRequest(fd);
	}
	else if (dataFds_[fd] > 2) {
		fileRequest(fd);
	}
}

/*******************************/
/*           REQUESTS          */
/*******************************/
void	SockData::cgiRequest(int fd)
{
	std::stringstream	ss;
	ss << fd;
	if (FD_ISSET(clients_[fd].getInputFd(), &writeSet_)) {
		std::string	pathFile = "./cgi_binary/.cgi_input_" + ss.str();
		FD_CLR(clients_[fd].getInputFd(), &writeSet_);
		if (write(clients_[fd].getInputFd(),
		clients_[fd].getRequest().getBody().c_str(),
		clients_[fd].getRequest().getBody().size()) == ERROR) {
			clearClient(fd);
			systemFailure("write", fd);
			return ;
		}
		FD_SET(clients_[fd].getInputFd(), &activeSet_);
	}
	else if (FD_ISSET(clients_[fd].getInputFd(), &readSet_)) {
		close(clients_[fd].getInputFd());
		FD_CLR(clients_[fd].getInputFd(), &activeSet_);
		cgiHandler cgi(clients_[fd].getRequest(), clients_[fd].getServer());
		if (cgi.startCgi(fd) == ERROR) {
			clearClient(fd);
			systemFailure("cgi", fd);
			return ;
		}
		std::string	pathFile = "./cgi_binary/.cgi_output_" + ss.str();
		if ((clients_[fd].getOutputFd() =
		open(pathFile.c_str(), O_RDONLY)) == ERROR) {
			clearClient(fd);
			systemFailure("open", fd);
			return ;
		}
		dataFds_[fd] = clients_[fd].getOutputFd();
		FD_SET(dataFds_[fd], &activeSet_);
	}
}

void	SockData::strDataRequest(int fd)
{
	clients_[fd].getData() = clients_[fd].getResponse().getData();
	clients_[fd].setDataReady(true);
}

void	SockData::fileRequest(int fd)
{
	char	buffer[BUF_SIZE];
	int		ret;
	if ((ret = read(dataFds_[fd], buffer, BUF_SIZE - 1)) == ERROR) {
		clearClient(fd);
		systemFailure("read", fd);
		return ;
	}
	buffer[ret] = '\0';
	clients_[fd].getResponseBody() += std::string(buffer, ret);
	if (isReadingOver(ret)) {
		close(dataFds_[fd]);
		clients_[fd].getResponse().makeResponse(
			clients_[fd].getResponseBody(),
			clients_[fd].isDataCgi());
		clients_[fd].getData() = clients_[fd].getResponse().getData();
		std::stringstream	ss;
		ss << fd;
		std::string	pathFile = "./cgi_binary/.cgi_output_" + ss.str();
		clearDataFd(fd);
		clients_[fd].setDataReady(true);
	}
}

void	SockData::sendDataClient(int fd)
{
	if (send(fd, clients_[fd].getData().c_str(),
	clients_[fd].getData().size(), 0) == ERROR) {
		clearClient(fd);
		systemFailure("send", fd);
		return ;
	}
	msgSent(fd);
	FD_CLR(fd, &writeSet_);
	if (clients_[fd].getRequest().getConnection() == "keep-alive") {
		resetClient(fd);
	}
	else {
		cnxCloseRecv(fd);
		clearClient(fd);
	}
}

void	SockData::requestReceived(int fd)
{
	msgRecv(fd);
	setResponse(fd);
	FD_SET(fd, &writeSet_);
	if (dataFds_[fd] == CGI) {
		clients_[fd].setDataCgi(true);
		std::stringstream	ss;
		ss << fd;
		std::string	pathFile = "./cgi_binary/.cgi_input_" + ss.str();
		if ((clients_[fd].getInputFd() =
		open(pathFile.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0666)) == ERROR) {
			clearClient(fd);
			systemFailure("open", fd);
			return ;
		}
		FD_SET(clients_[fd].getInputFd(), &writeSet_);
	}
	else if (dataFds_[fd] > 2) {
		FD_SET(dataFds_[fd], &activeSet_);
	}
}

/*******************************/
/*             UTILS           */
/*******************************/
void	SockData::recvClientClose(int fd, int ret)
{
	if (ret == ERROR) {
		systemFailure("read", fd);
	}
	else {
		cnxCloseRecv(fd);
	}
	clearClient(fd);
}

void	SockData::clearDataFd(int fd)
{
	if (0 <= dataFds_[fd] && dataFds_[fd] < FD_SETSIZE) {
		FD_CLR(dataFds_[fd], &activeSet_);
		FD_CLR(dataFds_[fd], &writeSet_);
	}
	if (2 < dataFds_[fd] && dataFds_[fd] < FD_SETSIZE) {
		close(dataFds_[fd]);
	}
	dataFds_.erase(fd);
}

void	SockData::clearClient(int fd)
{
	std::stringstream	ss;
	ss << fd;
	std::string	pathFileIn = "./cgi_binary/.cgi_input_" + ss.str();
	std::string	pathFileOut = "./cgi_binary/.cgi_output_" + ss.str();;
	unlink(pathFileIn.c_str());
	unlink(pathFileOut.c_str());
	clearDataFd(fd);
	if (0 <= fd && fd < FD_SETSIZE) {
		FD_CLR(fd, &activeSet_);
		FD_CLR(fd, &writeSet_);
	}
	close(fd);
	clients_.erase(fd);
}

void	SockData::closeListen(size_t endInd)
{
	for (size_t i = 0; i < endInd; ++i) {
		close(sockListen_[i].first);
	}
}

void	SockData::resetClient(int fd)
{
	std::stringstream	ss;
	ss << fd;
	std::string	pathFileIn = "./cgi_binary/.cgi_input_" + ss.str();
	std::string	pathFileOut = "./cgi_binary/.cgi_output_" + ss.str();;
	unlink(pathFileIn.c_str());
	unlink(pathFileOut.c_str());
	clients_[fd].setChunk(false);
	clients_[fd].setDataReady(false);
	clients_[fd].setDataCgi(false);
	clients_[fd].getTmpRequest().clear();
	clients_[fd].getFinalRequest().clear();
	clients_[fd].getTotalLength() = 0;
	clients_[fd].getResponseBody().clear();
	clients_[fd].getData().clear();
}

SockData::unchunk_t	SockData::unchunk(std::string str) {
	std::stringstream	s;
	unsigned int		lenHex;
	size_t 				pos = str.find("\r\n");
	if (str.empty() || str.substr(str.length() - 2) != "\r\n") {
		throw SockData::badChunkRequestException();
	}
	str.erase(str.length() - 2, 2);
	s.clear();
	s << std::hex << str.substr(0, pos);
	s >> lenHex;
	std::pair<unsigned int, std::string>	retPair;
	retPair.first = std::min(lenHex, static_cast<unsigned int>(str.length() - (pos + 2)));
	retPair.second = str.substr(pos + 2);
	return retPair;
}

/*******************************/
/*          SERVER MSG         */
/*******************************/
/* msg connection */
void	SockData::cnxFailed(void)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << RED;
	std::cerr << "Server: connection with client failed";
	std::cerr << NC;
	std::cerr << "-----------------------------" << std::endl;
}

void	SockData::cnxRefused(SockClient sockClient)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << RED;
	std::cerr << "Server: connection refused from " << sockClient.getIp();
	std::cerr << " via port " << sockClient.getPort();
	std::cerr << ": too many clients connected" << std::endl;
	std::cerr << NC;
	std::cerr << "-----------------------------" << std::endl;
}

void	SockData::cnxAccepted(SockClient sockClient)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << GREEN;
	std::cerr << "Server: connection from " << sockClient.getIp();
	std::cerr << " via port " << sockClient.getPort() << std::endl;
	std::cerr << NC;
	std::cerr << "-----------------------------" << std::endl;
}

/* msg close recv */
void	SockData::cnxCloseRecv(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << GREEN;
	std::cerr << "Server: connexion terminated with EOF " << clients_[fd].getIp();
	std::cerr << " via port " << clients_[fd].getPort();
	std::cerr << " | socket fd: " << fd;
	std::cerr << " | closing connection";
	std::cerr << std::endl;
	std::cerr << NC;
	std::cerr << "-----------------------------" << std::endl;
}

/* msg close send */
void	SockData::cnxCloseSend(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << RED;
	std::cerr << "Server: couldn't send response to " << clients_[fd].getIp();
	std::cerr << " on port " << clients_[fd].getPort();
	std::cerr << " | socket fd: " << fd;
	std::cerr << " | closing connection";
	std::cerr << std::endl;
	std::cerr << NC;
	std::cerr << "-----------------------------" << std::endl;
}

/* msg recv/send */
void	SockData::msgRecv(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << PURPLE;
	std::cerr << "Server: new message from " << clients_[fd].getIp();
	std::cerr << " on port " << clients_[fd].getPort();
	std::cerr << " | socket fd: " << fd;
	std::cerr << std::endl;
	std::cerr << NC;
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << PURPLE;
	std::cerr << "Message: " << std::endl;
	std::cerr << NC;
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << clients_[fd].getFinalRequest() << std::endl;
}

void	SockData::msgSent(int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << PURPLE;
	std::cerr << "Server: message successfully sent to " << clients_[fd].getIp();
	std::cerr << " on port " << clients_[fd].getPort();
	std::cerr << " | socket fd: " << fd;
	std::cerr << std::endl;
	std::cerr << NC;
	std::cerr << "-----------------------------" << std::endl;
}

/* errors */
void	SockData::exceptionError(int fd, std::exception &e)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << RED;
	std::cerr << "Server: " << e.what() << ": " << clients_[fd].getIp();
	std::cerr << " on port " << clients_[fd].getPort();
	std::cerr << " | socket fd: " << fd;
	std::cerr << std::endl;
	std::cerr << NC;
	std::cerr << "-----------------------------" << std::endl;
}

void	SockData::systemFailure(std::string str, int fd)
{
	std::cerr << "-----------------------------" << std::endl;
	std::cerr << RED;
	std::cerr << "Server: error: " << str;
	std::cerr << " | socket fd " << fd;
	std::cerr << " | closing connection"; 
	std::cerr << std::endl;
	std::cerr << NC;
	std::cerr << "-----------------------------" << std::endl;
}
