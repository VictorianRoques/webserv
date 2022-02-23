/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: viroques <viroques@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:17:38 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/23 16:58:23 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/**************************************/
//           EXCEPTIONS               //
/**************************************/
char const *Server::ExceededMaxBodySizeException::what() const throw() { return "Exceeded max body size!"; }

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Server::Server():
	_ip("127.0.0.1"),
	_maxBodySize(1000000), //nginx default value
	_serverNb(-1)
{
	_errorPage["310"] = "error_pages/310.html";
	_errorPage["400"] = "error_pages/400.html";
	_errorPage["403"] = "error_pages/403.html";
	_errorPage["404"] = "error_pages/404.html";
	_errorPage["405"] = "error_pages/405.html";
	_errorPage["406"] = "error_pages/406.html";
	_errorPage["413"] = "error_pages/413.html";
	_errorPage["429"] = "error_pages/429.html";
	_errorPage["499"] = "error_pages/499.html";
	_errorPage["500"] = "error_pages/500.html";
}

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
		_errorPageContent = x.getErrorPageContent();
		_maxBodySize = x.getMaxBodySize();
		_location = x.getLocation();
		_serverNb = x.getServerNb();
		_generalRoot = x.getGeneralRoot();
	}
	return *this;
}

/**************************************/
//				GETTERS				  //
/**************************************/

std::string 					Server::getIP() const { return _ip; }
std::vector<size_t>				Server::getPort() const { return _port; }
vec_str							Server::getServerName() const { return _serverName; }
map_str							Server::getErrorPage() const { return _errorPage; }
map_str							Server::getErrorPageContent() const { return _errorPageContent; }
long long						Server::getMaxBodySize() const { return _maxBodySize; }
std::vector<Location>			Server::getLocation() const { return _location; }
size_t							Server::getServerNb() const { return _serverNb; }

std::string						Server::getGeneralRoot() const { return _generalRoot; }

/**************************************/
//				SETTERS				  //
/**************************************/

void	Server::setIP(std::string ip) { _ip = ip; }
void	Server::setPort(std::vector<size_t> port) { _port = port; }
void	Server::setServerName(vec_str serverName) { _serverName = serverName; }
void	Server::setErrorPage(map_str errorPage) { _errorPage = errorPage; }
void	Server::setErrorPageContent(map_str errorPageContent) { _errorPageContent = errorPageContent; }
void	Server::setMaxBodySize(long long maxBodySize) { _maxBodySize = maxBodySize; }
void	Server::setMaxBodySize(std::string maxBodySize) { _maxBodySize = maxBodyAtoi(maxBodySize); }
void	Server::setLocation(std::vector<Location> location) { _location = location; }
void	Server::setServerNb(size_t serverNb) { _serverNb = serverNb; }

void	Server::setGeneralRoot(std::string generalRoot) { _generalRoot = generalRoot; }

/**************************************/
//				UTILS				  //
/**************************************/

void	Server::print_serv() {
	if (_serverNb > 1)
		std::cout << "____________________________________________________" << std::endl << std::endl;
	std::cout << COLOR_SERV << "/**************************************/" << NC << std::endl;
	std::cout << COLOR_SERV << "//               SERVER               //" << NC << std::endl;
	std::cout << COLOR_SERV << "/**************************************/" << NC << std::endl << std::endl;
	std::cout << COLOR_SERV << "Server no. " << NC << _serverNb << std::endl;
	std::cout << COLOR_SERV << "Server Name: " << NC; ft_putvec(_serverName, " ");
	std::cout << COLOR_SERV << "IP: " << NC << _ip << std::endl;
	std::cout << COLOR_SERV << "Port: " << NC; ft_putvec(_port, " ");
	std::cout << COLOR_SERV << "Error Page: " << NC; ft_putmap(_errorPage, " ");
	std::cout << COLOR_SERV << "Client_Max_Body_Size: " << NC << _maxBodySize << std::endl;
//	std::cout << COLOR_SERV << "Error Page Content: " << NC; ft_putmap(_errorPageContent, " ");

	std::cout << std::endl;
	std::vector<Location>::iterator	it = _location.begin();
	for (; it != _location.end(); it++) {
		std::cout << COLOR_SERV << "Location: " << NC; 
		it->print_loc();
	}
}
