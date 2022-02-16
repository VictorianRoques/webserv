/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckerParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 09:59:27 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/15 18:18:14 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

const std::string WHITESPACE = "\f\t\n\r\v ";

/**************************************/
//           EXCEPTIONS               //
/**************************************/
char const *Parser::WrongServerNameException::what() const throw() { return "For this project, you'll need localhost as a server name."; }
char const *Parser::DuplicatePortException::what() const throw() { return "You've assigned a port multiple times..."; }
char const *Parser::MissingDefaultLocationException::what() const throw() { return "You need at least one location / { } context in your server {}."; }
char const *Parser::ErrorPagePathException::what() const throw() { return "error_page path isn't an existing file."; }

/**************************************/
//           PARSER CHECKERS          //
/**************************************/

/**
 * serverNameChecker(): corrects server name : adds <server_name>:<server_port> and "www".<server_name>:<server_port> to the _serverName vector
**/
bool	Parser::serverNameCompleter() {
	std::string			correct;
	std::stringstream	out;

//	if (_serverName.size() > 0 && !vector_contains_str(getServerName(), "localhost") && !vector_contains_str(getServerName(), "www.localhost"))
//		return false;
//	correct = "localhost:";
	out << *getPort().begin();
	std::vector<std::string>::iterator it = _serverName.begin();
	std::vector<std::string>::iterator itend = _serverName.end();
	for (; it != itend; it++){
		correct = *it + ":" + out.str();
		_serverName.push_back(correct);
		_serverName.push_back("www." + correct);
	}
//	correct += out.str();
//	_serverName.push_back(it);
	return true;
}

/**
 * portChecker(): checks if a port is defined several times (forbidden)
**/
bool	Parser::portsChecker() {
	std::vector<size_t>::iterator it = _ports_g.begin();
	std::vector<size_t>::iterator it1;
	for (; it != _ports_g.end() - 1; it++) {
		it1 = it + 1;
		for (; it1 != _ports_g.end(); it1++) {
			if (*it1 == *it)
				return false;
		}
	}
	return true;
}

/**
 * locationChecker(): checks if server has at least one location / { }
**/
bool	Parser::locationChecker() {
	if (_location.size() < 1)
		return false;
	std::vector<Location>::iterator it = _location.begin();
	for (; it != _location.end(); it++) {
		if (it->getLocationMatch() == "/")
			return true;
	}
	return false;
}

/**
 * errorPageChecker(): check if error_page directive is ok
**/
bool	Parser::errorPageChecker() {
	map_str::iterator it = _errorPage.begin();
	for (; it != _errorPage.end(); it++) {
		if (!pathIsFile(it->second))
			return false;
	}
	return true;
}

/**
 * serverChecker(): checks if Server meets minimum requirements
**/
void	Parser::serverChecker() {

	// check server name
	serverNameCompleter();

	if (!portsChecker())
		throw DuplicatePortException();

	if (!locationChecker())
		throw MissingDefaultLocationException();

	if (!errorPageChecker())
		throw ErrorPagePathException();
}
