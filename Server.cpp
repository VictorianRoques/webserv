/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:17:38 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/05 19:14:57 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Server::Server():
	_serverNb(0),
	_ip("127.0.0.1"),
	_root("/"),
	_maxBodySize(1000000), //nginx default value
	_location(NULL)
{
	std::vector<size_t> port(1, 80);
	_port = port;
	
	vec_str	serverName(1, "localhost");
	_serverName = serverName;

	vec_str errorPage(1, "");
	_errorPage = errorPage;
}

Server::~Server() {}

Server::Server(const Server &x) {
	*this = x;
}

Server&	Server::operator=(const Server &x) {
	if (this != &x) {
		_serverNb = x.getServerNb();
		_ip = x.getIP();
		_port = x.getPort();
		_serverName = x.getServerName();
		_root = x.getRoot();
		_errorPage = x.getErrorPage();
		_maxBodySize = x.getMaxBodySize();
		_location = x.getLocation();
	}
	return *this;
}

/**************************************/
//				GETTERS				  //
/**************************************/

size_t		 				Server::getServerNb() const { return _serverNb; }
std::string 				Server::getIP() const { return _ip; }
std::vector<size_t>			Server::getPort() const { return _port; }
vec_str						Server::getServerName() const { return _serverName; }
std::string					Server::getRoot() const { return _root; }
vec_str						Server::getErrorPage() const { return _errorPage; }
size_t						Server::getMaxBodySize() const { return _maxBodySize; }

/**************************************/
//				SETTERS				  //
/**************************************/

void	Server::setIP(std::string ip) { _ip = ip; }
void	Server::setPort(std::vector<size_t> port) { _port = port; }
void	Server::setServerName(vec_str serverName) { _serverName = serverName; }
void	Server::setRoot(std::string root) { _root = root; }
void	Server::setErrorPage(vec_str errorPage) { _errorPage = errorPage; }
void	Server::setMaxBodySize(size_t maxBodySize) { _maxBodySize = maxBodySize; }
void	Server::setMaxBodySize(std::string maxBodySize) {
	size_t pos;

	_maxBodySize = static_cast<size_t>(std::atoi(maxBodySize.c_str()));
	if ((pos = maxBodySize.find_first_not_of("0123456789")) != std::string::npos) {
		if (maxBodySize.at(pos) == 'K')
			_maxBodySize *= 1000;
		else if (maxBodySize.at(pos) == 'M')
			_maxBodySize *= 1000000;
		else if (maxBodySize.at(pos) == 'G')
			_maxBodySize *= 1000000000;
	}
}
void	Server::setLocation(Location* location) {
	_location->setRoot(location->getRoot());
}

/**************************************/
//			SERVER PARSED			  //
/**************************************/

void	Server::addServer(Meta meta) {
	meta.addServer(*this);
}
