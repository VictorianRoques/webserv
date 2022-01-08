/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:17:38 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/08 13:05:13 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

const std::string WHITESPACE = "\f\t\n\r\v ";

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
	std::string	root;
	size_t		pos;
	size_t		posend;

	//remove the trailing ';'
	pos = (*it).find_first_not_of(";");
	posend = std::min((*it).find_first_of(";", pos), (*it).length());
	root = (*it).substr(pos, posend - pos);

	setRoot(root);
	(void)vend;
	return 1;
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
	std::string	name;
	size_t		pos;
	size_t		posend;


	for (; it != vend; it++) {
		//remove the trailing ';'
		pos = (*it).find_first_not_of(";");
		posend = std::min((*it).find_first_of(";", pos), (*it).length());
		name = (*it).substr(pos, posend - pos);
	
		// set serverName
		_serverName.push_back(name);
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
	std::string	errorPage;
	size_t		pos;
	size_t		posend;

	for (; it != vend; it++) {
		//remove the trailing ';'
		pos = (*it).find_first_not_of(";");
		posend = std::min((*it).find_first_of(";", pos), (*it).length());
		errorPage = (*it).substr(pos, posend - pos);

		// set ErrorPage
		_errorPage.push_back(errorPage);
		ret++;

		// met a ';' == end of the directive
		if (it->find(";") != std::string::npos)
			return ret;
	}
	return ret;
}

/**
 * dirLocation(): sets location from parsing (called by interpret())
**/
size_t	Parser::dirLocation(vec_str::iterator it, vec_str::iterator vend) {
	size_t ret = 1;
	size_t iter;
	Location *location = new Location();

	std::vector<std::pair<std::string, Location::methodPointer> > dir;
	Location::methodPointer mp;

	dir.push_back(std::make_pair("root", &Location::dirRoot));
	
	std::vector<std::pair<std::string, Location::methodPointer> >::iterator idir;
	for (; it != vend; it++) {

		idir = dir.begin();
		for (; idir < dir.end(); idir++) {
			if (*it == idir->first) {
				mp = idir->second;
				iter = (location->*mp)(it + 1, vend);
				ret += iter;
			}

			// meets a '}' == end of the location directive
			if (it->find("}") != std::string::npos) {
				this->_location.push_back(location);
				return ret;
			}
		}
	}
	return ret;
}

/**
 * clear(): clears up variables from Parser (to make it available to next server parsing)
**/
void	Parser::clear() {
	_port.clear();
	_serverName.clear();
	// just keep the first file (defaultErrorPage.html)
	_errorPage.erase(_errorPage.begin() + 1, _errorPage.end());
	_location.clear();
}

/**
 * dirServer(): add the new server to 'meta_g' class (called by interpret())
**/
size_t	Parser::dirServer(vec_str::iterator it, vec_str::iterator vend) {

	_serverNb++;
	// need _serverNb > 1 because the "server {}" directive comes up first (when _serverNb == 0)
	// so we need to end the parsing of the first server before sending the first server to cgi.
	if (_serverNb > 1) {
		Server& server = dynamic_cast<Server&>(*this);
		// testing
		server.print_serv();
		// clearing Parser class
		this->clear();
	}
	(void)it;
	(void)vend;
	return 1;
}

/**************************************/
//			PARSING MAIN			  //
/**************************************/

/**
 * interpreter(): goes through the 'tok' vector and sets 'Parser' class variables accordingly
 * using the corresponding "dir" (for 'directive') functions (see above)
 **/
void	Parser::interpreter(vec_str tok) {

	std::vector<std::pair<std::string, methodPointer> > dir;
	methodPointer mp;

	dir.push_back(std::make_pair("location", &Parser::dirLocation));
	dir.push_back(std::make_pair("server", &Parser::dirServer));
	dir.push_back(std::make_pair("listen", &Parser::dirListen));
	dir.push_back(std::make_pair("root", &Parser::dirRoot));
	dir.push_back(std::make_pair("client_max_body_size", &Parser::dirMaxBodySize));
	dir.push_back(std::make_pair("server_name", &Parser::dirServerName));
	dir.push_back(std::make_pair("error_page", &Parser::dirErrorPage));

	vec_str::iterator itok = tok.begin();
	std::vector<std::pair<std::string, methodPointer> >::iterator idir;
	while (itok != tok.end()) {

		idir = dir.begin();
		for (; idir < dir.end(); idir++) {
			if (*itok == idir->first) {
				mp = idir->second;
				itok += (this->*mp)(itok + 1, tok.end());
				break ;
			}
		}
		itok++;
	}
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
			// remove comments before parsing
			pos = line.find_first_of("#");
			if (pos == 0)
				break ;
			else if (pos != std::string::npos)
				line.erase(pos);

			// trim out WHITESPACES
			pos = line.find_first_not_of(WHITESPACE);
			posend = std::min(line.find_first_of(WHITESPACE, pos), line.length());
			tok.push_back(line.substr(pos, posend - pos));
			line.erase(0, posend);
		}
	}
	// DELETE FOLLOWING LINE WHEN DONE (used for testing)
	//ft_putvec(tok, "\n");
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
