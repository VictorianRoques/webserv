/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:17:38 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/05 14:46:17 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

const std::string WHITESPACE = "\f\t\n\r\v ";

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Parser::Parser():
	_serverNb(0);
	_ip("127.0.0.1")
	_port(80);
	_serverName("localhost");
	_root("/")
	_errorPage("");
	_maxBodySize(1000000); //nginx default value
	_location(NULL);
{}

Parser::~Parser() {}

Parser::Parser(const Parser &x) {
	*this = x;
}

Parser&	operator=(const Parser &x) {
	if (*this != x) {
		_serverNb = x._serverNb;
		_ip = x._ip;
		_port = x._port;
		_serverName = x._serverName;
		_root = x._root;
		_errorPage = x._errorPage;
		_maxBodySize = x._maxBodySize;
		_location = x._location;
	}
	return *this;
}

/**************************************/
//				GETTERS				  //
/**************************************/

std::string 				Parser::getServerNb() const { return _serverNb; }
std::string 				Parser::getIP() const { return _ip; }
std::vector<size_t>			Parser::getPort() const { return _port; }
vec_str						Parser::getServerName() const { return _serverName; }
std::string					Parser::getRoot() const { return _root; }
vec_str						Parser::getErrorPage() const { return _errorPage; }
size_t						Parser::getMaxBodySize() const { return _maxBodySize; }

/**************************************/
//				SETTERS				  //
/**************************************/

void	Parser::setIP(std::string ip) { _ip = ip; }
void	Parser::setPort(std::vector<size_t> port) { _port = port; }
void	Parser::setServerName(vec_str serverName) { _serverName = serverName }
void	Parser::setRoot(std::string root) { _root = root; }
void	Parser::setErrorPage(vec_str errorPage) { _errorPage = errorPage; }
void	Parser::setMaxBodySize(size_t maxBodySize) { _maxBodySize = maxBodySize; }
void	Parser::setMaxBodySize(std::string maxBodySize) {
	size_t pos;

	_maxBodySize = static_cast<size_t>std::atoi(maxBodySize.c_str);
	if ((pos = maxBodySize.find_first_not_of("0123456789")) != std::string::npos) {
		if (maxBodySize.at(pos) == 'K')
			_maxBodySize *= 1000;
		else if (maxBodySize.at(pos) == 'M')
			_maxBodySize *= 1000000;
		else if (maxBodySize.at(pos) == 'G')
			_maxBodySize *= 1000000000;
	}
}
void	Parser::setLocation(t_location location) {
	_location.root = location.root;
}

/**************************************/
//			SETTERS HELPER			  //
/**************************************/

/**
 * addPort(): add a new port to the vector containing ports.
 * Called by drListen (see below)
 **/
void	Parser::addPort(std::string port) { _port.push_back(static_cast<size_t>std::atoi(port.c_str)); }

/**
 * dirListen(): handles setIP() and setPort() (called by interpret())
 * 2 types of directive:
 * 1. listen IP:port;
 * 2. listen port;
 * returns the number of elements iterated over
 **/
size_t	Parser::dirListen(vec_str::iterator it, vec_str::iterator vend) {
	size_t ret = 0;

	for (; it != vend; it++) {
		size_t	pos = it->find(":");
		size_t	end = it->find(";");

		//listen IP:port
		if (pos != std::string::npos) {
			setIP(it->substr(0, pos - 1));
			addPort(it->substr(pos + 1, end));
		}
		//listen port
		else
			addPort(it->substr(0, end));

		ret++;
		// met a ';' == end of the directive
		if (end != std::string::npos)
			break ;
	}
	return ret;
}

/**
 * dirRoot(): sets root from parsing (called by interpret())
**/
size_t	Parser::dirRoot(vec_str::iterator it, vec_str::iterator vend) {
	setRoot(*it);
	(void)vend;
	return (1);
}

/**
 * dirMaxBodySize(): sets client_max_body_size from parsing (called by interpret())
**/
size_t Parser::dirMaxBodySize(vec_str::iterator it, vec_str::iterator vend) {
	setMaxBodySize(*it);
	(void)vend;
	return 1;
}

/**
 * dirServerName(): sets server_name(s) from parsing (called by interpret())
**/
size_t	Parser::dirServerName(vec_str::iterator it, vec_str::iterator vend) {
	size_t	ret = 0;

	for (; it != vend; it++) {
		_serverName.push_back(*it);
		ret++;

		// met a ';' == end of the directive
		if (it->find(";") != std::string::npos)
			break ;
	}
	return ret;
}

/**
 * dirErrorPage(): sets error_page(s) from parsing (called by interpret())
**/
size_t	Parser::dirErrorPage(vec_str::iterator it, vec_str::iterator vend) {
	size_t	ret = 0;

	for (; it != vend; it++) {
		_errorPage.push_back(*it);
		ret++;

		// met a ';' == end of the directive
		if (it->find(";") != std::string::npos)
			break ;
	}
	return ret;
}

/**
 * dirLocation(): sets location from parsing (called by interpret())
**/
size_t	Parser::dirLocation(vec_str::iterator it, vec_str::iterator vend) {
	size_t ret = 0;
	size_t iter;

	std::vector<std::pair<std::string, methodPointer> > dir;
	methodPointer mp;

	dir.push_back(std::make_pair("root", &dirRoot));
	
	std::vector<std::pair<std::string, methodPointer> >::iterator idir;
	for (; it != vend; it++) {

		idir = dir.begin();
		for (; idir < dir.end(); idir++) {
			if (*it == idir->first) {
				mp = idir->second;
				iter = mp(it + 1, tok.end());
				ret += iter;
				it += iter;

			// met a '}' == end of the location directive
			if (it->find("}") != std::string::npos)
				break ;
			}
		}
	}
	return ret;
}

/**
 * dirServer(): generates a new server (called by interpret())
**/
size_t	Parser::server(vec_str::iterator it, vec_str::iterator vend) {

	_serverNb++;
	// if there is more than one server, copy the server we just parsed into a new Server instance
	if (_serverNb > 1) {
		Server server(this);
		server.addServer(Meta);
	}
	return 1;
}

/**************************************/
//				PARSERS				  //
/**************************************/

/**
 * interpret(): goes through the 'tok' vector and sets 'Server' class accordingly
 **/
void	Parser::interpret(vec_str tok) {

	std::vector<std::pair<std::string, methodPointer> > dir;
	methodPointer mp;

	dir.push_back(std::make_pair("listen", &dirListen));
	dir.push_back(std::make_pair("root", &dirRoot));
	dir.push_back(std::make_pair("client_max_body_size", &dirMaxBodySize));
	dir.push_back(std::make_pair("server_name", &dirServerName));
	dir.push_back(std::make_pair("error_page", &dirErrorPage));
	dir.push_back(std::make_pair("location", &dirLocation));
	dir.push_back(std::make_pair("server", &dirServer));

	vec_str::iterator itok = tok.begin();
	std::vector<std::pair<std::string, methodPointer> >::iterator idir;
	for (; itok != tok.end(); itok++) {

		idir = dir.begin();
		for (; idir < dir.end(); idir++) {
			if (*itok == idir->first) {
				mp = idir->second;
				itok += mp(itok + 1, tok.end());
			}
		}
	}
}

/**
 * tokenizer(): splits the file content by WHITESPACE and stores results into the 'tok' vector
 **/
void	Parser::tokenize(char **av) {
	std::ifstream	file;
	std::string fileName(av[1]);
	std::string	line;
	vec_str tok;
	size_t	pos;
	size_t	posend;

	file.open(fileName);
	if (!file.is_open())
		std::cout << "Error: Failed to open file\n";

	while (!file.eof()) {
		getline(file, line);

		while (line.find_first_not_of(WHITESPACE) != std::string::npos) {
			pos = line.find_first_not_of(WHITESPACE);
			posend = std::min(line.find_first_of(WHITESPACE, pos), std::string::npos);
			tok.push_back(line.substr(pos, posend));
			line.erase(0, posend);
		}
	}
	//	ft_putvec(tok);
	interpret(tok);
}

/**
 * main(): gets a config file in param and parses it
 **/
int		main(int ac, char **av) {

	Parser	parser;

	if (ac != 2)
		std::cout << "Error: Need one and only one argument\n";
	else {
		parser.tokenize(av);
	}
}
