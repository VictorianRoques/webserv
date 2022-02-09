/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckerParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 09:59:27 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/09 17:07:20 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

const std::string WHITESPACE = "\f\t\n\r\v ";

/**************************************/
//           EXCEPTIONS               //
/**************************************/
char const *Parser::WrongServerNameException::what() const throw() { return "For this project, you'll need localhost as a server name."; }
char const *Parser::MissingDefaultLocationException::what() const throw() { return "You need at least one location / { } context in your server {}."; }

/**************************************/
//           PARSER CHECKERS          //
/**************************************/

/**
 * serverNameChecker(): returns true is serverName contains a correct server name : localhost:<server_port>
**/
bool	Parser::serverNameChecker() {
	std::string			correct;
	std::stringstream	out;

	if (_serverName.size() > 0 && !vector_contains_str(getServerName(), "localhost") && !vector_contains_str(getServerName(), "www.localhost"))
		return false;
	correct = "localhost:";
	out << *getPort().begin();
	correct += out.str();
	_serverName.push_back(correct);
	_serverName.push_back("www." + correct);
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
 * serverChecker(): checks if Server meets minimum requirements
**/
void	Parser::serverChecker() {

	// check server name
	if (!serverNameChecker())
		throw WrongServerNameException();

	if (!locationChecker())
		throw MissingDefaultLocationException();
}
