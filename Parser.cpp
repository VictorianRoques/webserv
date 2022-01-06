/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:17:38 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/06 15:12:13 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include "Parser.hpp"

const std::string WHITESPACE = "\f\t\n\r\v ";
Meta	meta_g;

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Parser::Parser(): Server(), _serverNb(0)
{}

Parser::~Parser() {}

Parser::Parser(const Parser &x) {
	*this = x;
}

Parser&	Parser::operator=(const Parser &x) {
	if (this != &x) {
		_serverNb = x.getServerNb();
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

size_t		 				Parser::getServerNb() const { return _serverNb; }

/**************************************/
//			PARSING HELPERS			  //
/**************************************/

/**
 * addPort(): add a new port to the vector containing ports.
 * Called by drListen (see below)
 **/
void	Parser::addPort(std::string port) { _port.push_back(static_cast<size_t>(std::atoi(port.c_str()))); }

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

	dir.push_back(std::make_pair("root", &Parser::dirRoot));
	
	std::vector<std::pair<std::string, Parser::methodPointer> >::iterator idir;
	for (; it != vend; it++) {

		idir = dir.begin();
		for (; idir < dir.end(); idir++) {
			if (*it == idir->first) {
				mp = idir->second;
				iter = mp(it + 1, vend);
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
 * dirServer(): add the new server to 'meta_g' class (called by interpret())
**/
size_t	Parser::dirServer(vec_str::iterator it, vec_str::iterator vend) {

	_serverNb++;
	// if there is more than one server, copy the server we just parsed into a new Server instance
	if (_serverNb > 1) {
		Server& server = dynamic_cast<Server&>(*this);	
		server.addServer(meta_g);
	}
	(void)it;
	(void)vend;
	return 1;
}

/**************************************/
//			PARSING MAIN			  //
/**************************************/

/**
 * interpreter(): goes through the 'tok' vector and sets 'Server' class accordingly
 **/
void	Parser::interpreter(vec_str tok) {

	std::vector<std::pair<std::string, methodPointer> > dir;
	methodPointer mp;

	dir.push_back(std::make_pair("listen", &Parser::dirListen));
	dir.push_back(std::make_pair("root", &Parser::dirRoot));
	dir.push_back(std::make_pair("client_max_body_size", &Parser::dirMaxBodySize));
	dir.push_back(std::make_pair("server_name", &Parser::dirServerName));
	dir.push_back(std::make_pair("error_page", &Parser::dirErrorPage));
	dir.push_back(std::make_pair("location", &Parser::dirLocation));
	dir.push_back(std::make_pair("server", &Parser::dirServer));

	vec_str::iterator itok = tok.begin();
	std::vector<std::pair<std::string, methodPointer> >::iterator idir;
	for (; itok != tok.end(); itok++) {

		idir = dir.begin();
		for (; idir < dir.end(); idir++) {
			if (*itok == idir->first) {
				(idir->second)(itok + 1, tok.end());
			//	mp = idir->second;
			//	itok += (*mp)(itok + 1, tok.end());
			}
		}
	}
	//add the last server to the 'meta_g' class (note: the params don't matter here)
	dirServer(itok, itok);
}

/**
 * tokenizer(): splits the file content by WHITESPACE and stores results into the 'tok' vector
 **/
void	Parser::tokenizer(char **av) {
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
	//	ft_putvec(tok, "\n");
	interpreter(tok);
}

/**
 * main(): gets a config file in param and parses it
 **/
int		main(int ac, char **av) {

	Parser	parser;

	if (ac != 2)
		std::cout << "Error: Need one and only one argument\n";
	else {
		parser.tokenizer(av);
	}
}
