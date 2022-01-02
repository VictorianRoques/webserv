/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:17:38 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/02 17:45:39 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

const std::string WHITESPACE = "\f\t\n\r\v ";

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Parser::Parser():
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
	if (this != &x) {
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

std::string 				Parser::getIP() { return _ip; }
size_t						Parser::getPort() { return _port; }
std::vector<std::string>	Parser::getServerName() { return _serverName; }
std::string					Parser::getRoot() { return _root; }
std::vector<std::string>	Parser::getErrorPage() { return _errorPage; }
size_t						Parser::getMaxBodySize() { return _maxBodySize; }

/**************************************/
//				SETTERS				  //
/**************************************/

void	Parser::setIP(std::string ip) { _ip = ip; }
void	Parser::setPort(size_t port) { _port = port; }
void	Parser::setServerName(std::vector<std::string> serverName) { _serverName = serverName }
void	Parser::setRoot(std::string root) { _root = root; }
void	Parser::setErrorPage(std::vector<std::string> errorPage) { _errorPage = errorPage; }
void	Parser::setMaxBodySize(size_t maxBodySize) { _maxBodySize = maxBodySize; }
void	Parser::setLocation(t_location location) {
	_location.root = location.root;
}

/**************************************/
//				PARSERS				  //
/**************************************/

/**
 * interpret(): goes through the 'tok' vector and sets 'Server' class accordingly
**/
void	Parser::interpret(std::vector<std::string> tok) {
		
}

/**
 * tokenizer(): splits the file content by WHITESPACE and stores results into the 'tok' vector
**/
void	Parser::tokenize(char **av) {
	std::ifstream	file;
	std::string fileName(av[1]);
	std::string	line;
	std::vector<std::string> tok;
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
