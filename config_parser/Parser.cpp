/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:17:38 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/19 18:17:06 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

const std::string WHITESPACE = "\f\t\n\r\v ";
/**
 * global variable: vector containing all servers in parsed config file.
**/
std::vector<Server> servers_g;

/**************************************/
//           EXCEPTIONS               //
/**************************************/
char const *Parser::MissingBracketException::what() const throw() { return ("Missing a bracket after 'server' or 'location' directive."); }
char const *Parser::OutsideServerException::what() const throw() { return ("Some directive is outside a server definition."); }
char const *Parser::LonelyBracketException::what() const throw() { return ("Lonely opening bracket(s)."); }
char const *Parser::EmbeddedServersException::what() const throw() { return ("Found a server in another server."); }
char const *Parser::NoSuchDirectiveException::what() const throw() { return ("Unknown directive in the file."); }
char const *Parser::FailedToOpenException::what() const throw() { return ("Failed to open <config_file>."); }

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Parser::Parser(): Server(), 
	_in_server(false),
	_in_location(false)
{}

Parser::~Parser() {}

Parser::Parser(const Parser &x) {
	*this = x;
}

Parser&	Parser::operator=(const Parser &x) {
	if (this != &x) {
		_serverNb = x.getServerNb();
		_in_server = x.getInServer();
		_in_location = x.getInLocation();
		_ip = x.getIP();
		_port = x.getPort();
		_serverName = x.getServerName();
		_errorPage = x.getErrorPage();
		_maxBodySize = x.getMaxBodySize();
		_location = x.getLocation();
	}
	return *this;
}

/**************************************/
//				GETTERS				  //
/**************************************/

bool		 				Parser::getInServer() const { return _in_server; }
bool		 				Parser::getInLocation() const { return _in_location; }

/**************************************/
//			PARSING HELPERS			  //
/**************************************/

/**
 * addPort(): add a new port to the vector containing ports.
 * Called by dirListen (see below)
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
	size_t ret = 1;

	for (; it != vend; it++) {
		size_t	pos = it->find(":");
		size_t	end = it->find(";");

		//listen IP:port
		if (pos != std::string::npos) {
			setIP(it->substr(0, pos));
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
 * dirMaxBodySize(): sets client_max_body_size from parsing (called by interpret())
**/
size_t Parser::dirMaxBodySize(vec_str::iterator it, vec_str::iterator vend) {
	setMaxBodySize(*it);
	(void)vend;
	return 2;
}

/**
 * dirServerName(): sets server_name(s) from parsing (called by interpret())
**/
size_t	Parser::dirServerName(vec_str::iterator it, vec_str::iterator vend) {
	size_t	ret = 1;
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
	size_t	ret = 1;
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
 * dirOpen(): Necessarily lonely '{' (server and location take care of their own '{')
**/
size_t	Parser::dirOpen(vec_str::iterator it, vec_str::iterator vend) {
	throw LonelyBracketException();
	(void)it;
	(void)vend;
	return 1;
}

/**
 * dirClose(): records closing server/location
**/
size_t	Parser::dirClose(vec_str::iterator it, vec_str::iterator vend) {

	if (_in_server == false)
		throw LonelyBracketException();
	else if (_in_location == true)
		_in_location = false;
	else 
		_in_server = false;
	(void)it;
	(void)vend;
	return 1;
}

/**
 * locationContext(): sets MatchModifier and LocationMatch variables (called by dirLocation())
**/
size_t locationContext(vec_str::iterator it, Location *location) {
	size_t ret = 0;

	location->setMatchModifier(*it);
	location->setLocationMatch(*it);
	it++;
	ret++;
	if (*it != "{" && *(it + 1) != "{")
		throw Parser::MissingBracketException();
	else if (*it == "{") {
		location->setMatchModifier("");
		ret += 1;
	}
	else {
		location->setLocationMatch(*it);
		ret += 2;
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
	_in_location = true;

	iter = locationContext(it, location);
	it += iter;
	ret += iter;

	// set directives
	std::vector<std::pair<std::string, Location::methodPointer> > dir;
	Location::methodPointer mp;

	dir.push_back(std::make_pair("root", &Location::dirRoot));
	dir.push_back(std::make_pair("methods", &Location::dirMethods));
	dir.push_back(std::make_pair("error_page", &Location::dirErrorPage));
	dir.push_back(std::make_pair("autoindex", &Location::dirAutoIndex));
	dir.push_back(std::make_pair("index", &Location::dirIndex));
	dir.push_back(std::make_pair("return", &Location::dirRedirection));
	dir.push_back(std::make_pair("cgi_handler", &Location::dirCgiHandler));
	dir.push_back(std::make_pair("upload_dest", &Location::dirUploadDest));
	
	std::vector<std::pair<std::string, Location::methodPointer> >::iterator idir;
	while (it != vend) {

		if (*it == ";")
			it++;
		else {
			idir = dir.begin();
//			iter = 0;
			for (; idir < dir.end(); idir++) {
				if (*it == idir->first) {
					mp = idir->second;
					iter = (location->*mp)(it + 1, vend);
					ret += iter;
					it += iter;
					break ;
				}
			}
			// meets a '}' == end of the location directive
			if (it->find("}") != std::string::npos) {
				this->_location.push_back(location);
				return ret;
			}
	
			if (idir == dir.end()) {
				std::cout << "dirLocation(): Unknown directive: " << *it << std::endl;
				throw NoSuchDirectiveException();
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
	_errorPage.erase(_errorPage.begin(), _errorPage.end());
	_location.clear();
}

/**
 * dirServer(): add the new server to 'servers_g' vector (called by interpret())
**/
size_t	Parser::dirServer(vec_str::iterator it, vec_str::iterator vend) {

	if (_in_server == true)
		throw EmbeddedServersException();

	_serverNb++;
	_in_server = true;

	// since 'server' directive comes up first, need to iterate once before saving it into servers_g.
	if (_serverNb > 0) {
		Server& server = dynamic_cast<Server&>(*this);
		// add Server to servers_g
		servers_g.push_back(server);
		// clearing Parser class
		this->clear();
	}

	if (*it != "{" && it != vend)
		throw MissingBracketException();

	(void)it;
	(void)vend;
	return 2;
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
	dir.push_back(std::make_pair("client_max_body_size", &Parser::dirMaxBodySize));
	dir.push_back(std::make_pair("server_name", &Parser::dirServerName));
	dir.push_back(std::make_pair("error_page", &Parser::dirErrorPage));
	dir.push_back(std::make_pair("{", &Parser::dirOpen));
	dir.push_back(std::make_pair("}", &Parser::dirClose));

	vec_str::iterator itok = tok.begin();
	std::vector<std::pair<std::string, methodPointer> >::iterator idir;
	while (itok != tok.end()) {

		if (*itok != "server" && _in_server == false)
			throw OutsideServerException();

		if (*itok == ";")
			itok++;
		else {
			idir = dir.begin();
			for (; idir < dir.end(); idir++) {
				if (*itok == idir->first) {
					mp = idir->second;
					itok += (this->*mp)(itok + 1, tok.end());
					break ;
				}
			}
			if (idir == dir.end()) {
				std::cout << "Interpreter(): Unknown directive: " << *itok << std::endl;
				throw NoSuchDirectiveException();
			}
		}
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
		throw FailedToOpenException();

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

// TESTING FUNCTION
void	print_test() {
	std::vector<Server>::iterator it;
	
	it = servers_g.begin();
	for (; it != servers_g.end(); it++)
		it->print_serv();
}

/**
 * main(): gets a config file in param and parses it
 **/
int		main(int ac, char **av) {

	Parser	parser;

	if (ac != 2)
		std::cout << RED << "Error: " << NC << "Need one and only one argument\n";
	else {
		try {
			parser.tokenizer(av);
			// TESTING
			print_test();
		}
		catch (std::exception &e) {
			std::cout << RED << "Error: " << NC << e.what() << std::endl;
		}
	}
}
