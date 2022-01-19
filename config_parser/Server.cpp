/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:17:38 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/19 17:58:32 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Server::Server():
	_ip("127.0.0.1"),
	_maxBodySize(1000000), //nginx default value
	_serverNb(-1)
{}

Server::~Server() {}

Server::Server(const Server &x) {
	*this = x;
}

Server&	Server::operator=(const Server &x) {
	if (this != &x) {
		_ip = x.getIP();
		_port = x.getPort();
		_serverName = x.getServerName();
		_errorPage = x.getErrorPage();
		_maxBodySize = x.getMaxBodySize();
		_location = x.getLocation();
		_serverNb = x.getServerNb();
	}
	return *this;
}

/**************************************/
//				GETTERS				  //
/**************************************/

std::string 					Server::getIP() const { return _ip; }
std::vector<size_t>				Server::getPort() const { return _port; }
vec_str							Server::getServerName() const { return _serverName; }
vec_str							Server::getErrorPage() const { return _errorPage; }
size_t							Server::getMaxBodySize() const { return _maxBodySize; }
std::vector<Location *>			Server::getLocation() const { return _location; }
size_t							Server::getServerNb() const { return _serverNb; }

/**************************************/
//				SETTERS				  //
/**************************************/

void	Server::setIP(std::string ip) { _ip = ip; }
void	Server::setPort(std::vector<size_t> port) { _port = port; }
void	Server::setServerName(vec_str serverName) { _serverName = serverName; }
void	Server::setErrorPage(vec_str errorPage) { _errorPage = errorPage; }
void	Server::setMaxBodySize(size_t maxBodySize) { _maxBodySize = maxBodySize; }
void	Server::setMaxBodySize(std::string maxBodySize) {
	size_t pos;

	_maxBodySize = static_cast<size_t>(std::atoi(maxBodySize.c_str()));
	if ((pos = maxBodySize.find_first_not_of("0123456789")) != std::string::npos) {
		if (maxBodySize.at(pos) == 'K' || maxBodySize.at(pos) == 'k')
			_maxBodySize *= 1000;
		else if (maxBodySize.at(pos) == 'M' || maxBodySize.at(pos) == 'm')
			_maxBodySize *= 1000000;
		else if (maxBodySize.at(pos) == 'G' || maxBodySize.at(pos) == 'g')
			_maxBodySize *= 1000000000;
	}
}
void	Server::setLocation(std::vector<Location *> location) { _location = location; }
void	Server::setServerNb(size_t serverNb) { _serverNb = serverNb; }

/**************************************/
//				UTILS				  //
/**************************************/

void	Server::print_serv() {
	if (_serverNb > 1)
		std::cout << "____________________________________________________" << std::endl << std::endl;
	std::cout << COLOR_SERV << "Server no. " << NC << _serverNb << std::endl;
	std::cout << COLOR_SERV << "Server Name: " << NC; ft_putvec(_serverName, " ");
	std::cout << COLOR_SERV << "IP: " << NC << _ip << std::endl;
	std::cout << COLOR_SERV << "Port: " << NC; ft_putvec(_port, " ");
	std::cout << COLOR_SERV << "Error Page: " << NC; ft_putvec(_errorPage, " ");
	std::cout << COLOR_SERV << "Client_Max_Body_Size: " << NC << _maxBodySize << std::endl;

	std::cout << std::endl;
	std::vector<Location *>::iterator	it = _location.begin();
	for (; it != _location.end(); it++) {
		std::cout << COLOR_SERV << "Location: " << NC; 
		(*it)->print_loc();
	}
}
