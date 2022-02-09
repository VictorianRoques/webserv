/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CheckerParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/09 09:59:27 by pnielly           #+#    #+#             */
/*   Updated: 2022/02/09 10:22:32 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

const std::string WHITESPACE = "\f\t\n\r\v ";

/**************************************/
//           EXCEPTIONS               //
/**************************************/
char const *Parser::WrongServerNameException::what() const throw() { return "For this project, you'll need localhost as a server name."; }

/**************************************/
//           PARSER CHECKERS          //
/**************************************/

/**
 * serverNameChecker(): returns true is serverName contains a correct server name : localhost:<server_port>
**/
bool	Parser::serverNameChecker() {
	std::string			correct;
	std::stringstream	out;

	if (!vector_contains_str(getServerName(), "localhost") && !vector_contains_str(getServerName(), "www.localhost"))
		return false;
	correct = "localhost:";
	out << *getPort().begin();
	correct += out.str();
	_serverName.push_back(correct);
	_serverName.push_back("www." + correct);
	return true;
}

/**
 * serverChecker(): checks if Server meets minimum requirements
**/
void	Parser::serverChecker() {

		// check server name
		if (!serverNameChecker())
			throw WrongServerNameException();
}
