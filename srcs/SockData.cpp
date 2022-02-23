
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
	tv.tv_sec = 10; tv.tv_usec = 0;
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
			if (setsockopt(newSock, SOL_SOCKET, SO_RCVTIMEO,
			reinterpret_cast<const char*>(&tv), sizeof(tv)) == ERROR) {
				cnxFailed();
				clearClient(newSock);
			}
			if (setsockopt(newSock, SOL_SOCKET, SO_SNDTIMEO,
			reinterpret_cast<const char*>(&tv), sizeof(tv)) == ERROR) {
				cnxFailed();
				clearClient(newSock);
			}
			FD_SET(newSock, &activeSet_);
			cnxAccepted(sockClient);
		}
	}
}

void	SockData::recvClient(int fd)
{
	char		buffer[BUF_SIZE] = { 0 };
	int			ret = recv(fd, buffer, BUF_SIZE - 1, 0);
	if (ret == ERROR || ret == 0) {
		recvClientClose(fd, ret);
		return ;
	}
	clients_[fd].getTmpRequest() += std::string(buffer, ret);
	if (isReadingOver(ret)) {
		try {
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
		catch (std::exception &e) {
			exceptionError(fd, e);
			setInternalError(fd);
		}
	}
}

void	SockData::sendClient(int fd)
{
	if (send(fd, clients_[fd].getData().c_str(),
	clients_[fd].getData().size(), 0) == ERROR) {
		systemFailure("send", fd);
		FD_CLR(fd, &writeSet_);
		clearClient(fd);
		return ;
	}
	msgSent(fd);
	FD_CLR(fd, &writeSet_);
	if (clients_[fd].getRequest().getConnection() == "keep-alive") {
		resetClient(fd);
		return ;
	}
	clearClient(fd);
}

/*******************************/
/*           REQUESTS          */
/*******************************/

void	SockData::cgiInputFile(int fd, std::string strFd)
{
	std::string	pathFile = "./cgi_binary/.cgi_input_" + strFd;
	std::ofstream	ofs;
	ofs.open(pathFile.c_str(), std::ofstream::out);
	if (!ofs.is_open()) {
		throw cgiException();
	}
	ofs << clients_[fd].getRequest().getBody();
	ofs.close();
}

void	SockData::cgiOutputFile(int fd, std::string strFd)
{
	std::string pathFile = "./cgi_binary/.cgi_output_" + strFd;
	std::ifstream	ifs;
	ifs.open(pathFile.c_str(), std::ifstream::in);
	if (!ifs.is_open()) {
		throw cgiException();
	}
	std::string	output, line;
	while (std::getline(ifs, line)) {
		output += line + "\n";
	}
	ifs.close();
	clients_[fd].getResponse().makeResponseCgi(output);
}

void	SockData::setRequestType(int fd, Request &request, Server &server)
{
	Response	response(server);
	clients_[fd].getRequestType() = response.requestType(request);
	clients_[fd].setRequest(request);
	clients_[fd].setResponse(response);
	clients_[fd].setServer(server);
}

void	SockData::setResponse(int fd)
{
	try {
		Request	request = requestParser(clients_[fd].getFinalRequest(), servers_);
		std::vector<Server>::iterator	it = servers_.begin(), ite = servers_.end();
		for (; it != ite; ++it) {
			if (vector_contains_str(it->getServerName(), request.getHost())) {
				setRequestType(fd, request, *it);
				return ;
			}
		}
		setRequestType(fd, request, *servers_.begin());
	}
	catch (std::exception &e) {
		exceptionError(fd, e);
		resetClient(fd);
		setInternalError(fd); // or clear client
	}
}

void	SockData::requestReceived(int fd)
{
	msgRecv(fd);
	setResponse(fd);
	FD_SET(fd, &writeSet_);
	if (clients_[fd].getRequestType() == CGI) {
		try {
			std::stringstream	ss;
			ss << fd;
			cgiInputFile(fd, ss.str());
			cgiHandler	cgi(clients_[fd].getRequest(), clients_[fd].getServer());
			cgi.startCgi(fd);
			cgiOutputFile(fd, ss.str());
		}
		catch (std::exception &e) {
			exceptionError(fd, e);
			resetClient(fd);
			setInternalError(fd);
		}
	}
	clients_[fd].getData() = clients_[fd].getResponse().getData();
}

void	SockData::setInternalError(int fd)
{
	Response	response(500);
	clients_[fd].setResponse(response);
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

void	SockData::clearCgiFiles(int fd)
{
	std::stringstream	ss;
	ss << fd;
	std::string	pathFileIn = "./cgi_binary/.cgi_input_" + ss.str();
	std::string	pathFileOut = "./cgi_binary/.cgi_output_" + ss.str();;
	unlink(pathFileIn.c_str());
	unlink(pathFileOut.c_str());
}

void	SockData::clearClient(int fd)
{
	clearCgiFiles(fd);
	FD_CLR(fd, &activeSet_);
	FD_CLR(fd, &writeSet_);
	close(fd);
	clients_.erase(fd);
}

void	SockData::resetClient(int fd)
{
	clearCgiFiles(fd);
	clients_[fd].setChunk(false);
	clients_[fd].getTmpRequest().clear();
	clients_[fd].getFinalRequest().clear();
	clients_[fd].getRequestType() = NO_CGI;
	clients_[fd].getTotalLength() = 0;
	clients_[fd].getData().clear();
}

void	SockData::closeListen(size_t endInd)
{
	for (size_t i = 0; i < endInd; ++i) {
		close(sockListen_[i].first);
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

SockData::unchunk_t	SockData::unchunk(std::string str) {
	std::stringstream	ss;
	unsigned int		lenHex;
	size_t 				pos = str.find("\r\n");
	if (str.empty() || str.substr(str.length() - 2) != "\r\n") {
		throw SockData::badChunkRequestException();
	}
	str.erase(str.length() - 2, 2);
	ss.clear();
	ss << std::hex << str.substr(0, pos);
	ss >> lenHex;
	std::pair<unsigned int, std::string>	retPair;
	retPair.first = std::min(lenHex, static_cast<unsigned int>(str.length() - (pos + 2)));
	retPair.second = str.substr(pos + 2);
	return retPair;
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
	if (clients_[fd].getFinalRequest().size() < 10000)
		std::cerr << clients_[fd].getFinalRequest() << std::endl;
	else
		std::cerr << "body too long" << std::endl;
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
