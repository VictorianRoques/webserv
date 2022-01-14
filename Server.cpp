/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:17:38 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/14 13:22:30 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Server::Server():
	_ip("127.0.0.1"),
	_root("/default"),
	_errorPage(1, "defaultErrorPage.html"),
	_maxBodySize(1000000), //nginx default value
	_autoIndex(true),
	_serverNb(0)
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
		_root = x.getRoot();
		_errorPage = x.getErrorPage();
		_maxBodySize = x.getMaxBodySize();
		_autoIndex = x.getAutoIndex();
		_location = x.getLocation();
		_serverNb = x.getServerNb();
		_redirection = x.getRedirection();
		_index = x.getIndex();
	}
	return *this;
}

/**************************************/
//				GETTERS				  //
/**************************************/

std::string 					Server::getIP() const { return _ip; }
std::vector<size_t>				Server::getPort() const { return _port; }
vec_str							Server::getServerName() const { return _serverName; }
std::string						Server::getRoot() const { return _root; }
vec_str							Server::getErrorPage() const { return _errorPage; }
size_t							Server::getMaxBodySize() const { return _maxBodySize; }
bool							Server::getAutoIndex() const { return _autoIndex; }
std::vector<Location *>			Server::getLocation() const { return _location; }
size_t							Server::getServerNb() const { return _serverNb; }
std::pair<size_t, std::string>	Server::getRedirection() const { return _redirection; }
std::string						Server::getIndex() const { return _index; }

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
		if (maxBodySize.at(pos) == 'K' || maxBodySize.at(pos) == 'k')
			_maxBodySize *= 1000;
		else if (maxBodySize.at(pos) == 'M' || maxBodySize.at(pos) == 'm')
			_maxBodySize *= 1000000;
		else if (maxBodySize.at(pos) == 'G' || maxBodySize.at(pos) == 'g')
			_maxBodySize *= 1000000000;
	}
}
void	Server::setAutoIndex(bool autoIndex) { _autoIndex = autoIndex; }
void	Server::setLocation(std::vector<Location *> location) { _location = location; }
void	Server::setServerNb(size_t serverNb) { _serverNb = serverNb; }
void	Server::setRedirection(std::pair<size_t, std::string> redirection) { _redirection = redirection; }
void	Server::setIndex(std::string index) { _index = index; }

/**************************************/
//			SERVER PARSED			  //
/**************************************/

void	Server::addServer(Meta meta) {
	meta.addServer(*this);
}

/**************************************/
//				UTILS				  //
/**************************************/

void	Server::print_serv() {
	std::cout << "____________________________________________________" << std::endl << std::endl;
	std::cout << COLOR_SERV << "Server no. " << NC << _serverNb << std::endl;
	std::cout << COLOR_SERV << "Server Name: " << NC; ft_putvec(_serverName, " ");
	std::cout << COLOR_SERV << "IP: " << NC << _ip << std::endl;
	std::cout << COLOR_SERV << "Port: " << NC; ft_putvec(_port, " ");
	std::cout << COLOR_SERV << "Root: " << NC << _root << std::endl;
	std::cout << COLOR_SERV << "Index (reponse file if request is directory): " << NC << _index << std::endl;
	std::cout << COLOR_SERV << "Error Page: " << NC; ft_putvec(_errorPage, " ");
	std::cout << COLOR_SERV << "Client_Max_Body_Size: " << NC << _maxBodySize << std::endl;
	std::cout << COLOR_SERV << "AutoIndex: " << NC << (_autoIndex ? "on" : "off") << std::endl;
	std::cout << COLOR_SERV << "Redirection: " << NC << (_redirection.first ? "Yes" : "No") << std::endl;
   	if (_redirection.first) {
		std::cout << COLOR_SERV <<  "   Status code: " << NC << _redirection.first << std::endl 
				<< COLOR_SERV << "   URI: " << NC << _redirection.second << std::endl;
	}

	std::cout << std::endl;
	std::vector<Location *>::iterator	it = _location.begin();
	for (; it != _location.end(); it++) {
		std::cout << COLOR_SERV << "Location: " << NC; 
		(*it)->print_loc();
	}
}
