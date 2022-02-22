/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckerParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 09:59:27 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/22 14:32:20 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

const std::string WHITESPACE = "\f\t\n\r\v ";

/**************************************/
//           EXCEPTIONS               //
/**************************************/
char const *Parser::WrongServerNameException::what() const throw() { return "For this project, you'll need localhost as a server name."; }
char const *Parser::MissingDefaultLocationException::what() const throw() { return "You need at least one location / { } context in your server {}."; }
char const *Parser::ErrorPagePathException::what() const throw() { return "error_page path isn't an existing file."; }

/**************************************/
//           PARSER CHECKERS          //
/**************************************/

/**
 * serverNameChecker(): makes sure the serverName has the right format : <address>:<port> or www.<address>:<port>
**/
bool	Parser::serverNameChecker() {
	std::string			correct;
	std::stringstream	out;
	size_t	i, end;

	out << *getPort().begin();
	vec_str::iterator it = getServerName().begin();
	i = 0;
	end = getServerName().size();
	for (; i < end; i++) {
		correct = *it;
		correct += ":";
		correct += out.str();
		_serverName.push_back(correct);
		_serverName.push_back("www." + correct);
		it++;
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
	serverNameChecker();

	if (!locationChecker())
		throw MissingDefaultLocationException();

	if (!errorPageChecker())
		throw ErrorPagePathException();
}
