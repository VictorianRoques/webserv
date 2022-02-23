/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckerParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 09:59:27 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/23 22:59:53 by pnielly          ###   ########.fr       */
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
 * serverNameChecker(): makes sure the serverName has the right format : <address>:<port>
**/
bool	Parser::serverNameChecker() {
	std::string			correct;
	std::stringstream	out;
	vec_str				v;

	out << *getPort().begin();
	vec_str::iterator it =_serverName.begin();
	for (; it < _serverName.end(); it++) {
		correct = *it;
		correct += ":";
		correct += out.str();
		v.push_back(correct);
	}
	_serverName.insert(_serverName.begin(), v.begin(), v.end());
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
	std::string	path;
	for (; it != _errorPage.end(); it++) {
		path = cleanSlash(getPWD() + "/" + it->second);
		if (!pathIsFile(path) && !pathIsFile(it->second))
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
