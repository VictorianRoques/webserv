/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:17:38 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/07 19:16:34 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Server::Server():
	_ip("127.0.0.1"),
	_root("/"),
	_maxBodySize(1000000) //nginx default value
{
	std::vector<size_t> port(1, 80);
	_port = port;
	
	vec_str	serverName(1, "localhost");
	_serverName = serverName;

	vec_str errorPage(1, "");
	_errorPage = errorPage;

//	_location = new std::vector<Location *>;
}

Server::~Server() {
//	delete _location; ??? compilation error: why?
}

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
		_location = x.getLocation();
	}
	return *this;
}

/**************************************/
//				GETTERS				  //
/**************************************/

std::string 				Server::getIP() const { return _ip; }
std::vector<size_t>			Server::getPort() const { return _port; }
vec_str						Server::getServerName() const { return _serverName; }
std::string					Server::getRoot() const { return _root; }
vec_str						Server::getErrorPage() const { return _errorPage; }
size_t						Server::getMaxBodySize() const { return _maxBodySize; }
std::vector<Location *>		Server::getLocation() const { return _location; }

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
void	Server::setLocation(std::vector<Location *> location) { _location = location; }

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
	std::cout << "Server Name: "; ft_putvec(_serverName, " ");
	std::cout << "IP: " << _ip << std::endl;
	std::cout << "Port: "; ft_putvec(_port, " ");
	std::cout << "Root: " << _root << std::endl;
	std::cout << "Error Page: "; ft_putvec(_errorPage, " ");
	std::cout << "Client_Max_Body_Size: " << _maxBodySize << std::endl;

	std::cout << std::endl;
	std::vector<Location *>::iterator	it = _location.begin();
	for (; it != _location.end(); it++) {
		std::cout << "Location: "; 
		(*it)->print_loc();
	}
}
