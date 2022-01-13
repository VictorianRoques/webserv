/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/30 19:17:38 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/13 19:45:33 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

const std::string WHITESPACE = "\f\t\n\r\v ";

/**************************************/
//           EXCEPTIONS               //
/**************************************/

char const *Parser::MissingBracketException::what() const throw() { return ("Missing a bracket after 'server' or 'location' directive."); }
char const *Parser::OutsideServerException::what() const throw() { return ("Some directive is outside a server definition."); }
char const *Parser::LonelyBracketException::what() const throw() { return ("Lonely opening bracket(s)."); }
char const *Parser::EmbeddedServersException::what() const throw() { return ("Found a server in another server."); }
char const *Parser::NoSuchDirectiveException::what() const throw() { return ("Non valid directive in the file."); }
char const *Parser::FailedToOpenException::what() const throw() { return ("Failed to open <config_file>."); }
char const *Parser::WrongValue_AutoIndexException::what() const throw() { return ("Wrong value for autoindex."); }

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Parser::Parser(): Server(), 
	_serverNb(0),
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
bool		 				Parser::getInServer() const { return _in_server; }
bool		 				Parser::getInLocation() const { return _in_location; }

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
	size_t ret = 1;

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
	return 2;
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
 * dirAutoIndex(): sets autoindex (called by interpret())
**/
size_t	Parser::dirAutoIndex(vec_str::iterator it, vec_str::iterator vend) {
	if (*it == "on;")
		setAutoIndex(true);
	else if (*it == "off;")
		setAutoIndex(false);
	else
		throw	WrongValue_AutoIndexException();
	(void)vend;
	return 2;
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
	
	std::vector<std::pair<std::string, Location::methodPointer> >::iterator idir;
	while (it != vend) {

		idir = dir.begin();
		iter = 0;
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

		if (iter == 0) {
			throw NoSuchDirectiveException();
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
	_autoIndex = true;
}

/**
 * dirServer(): add the new server to 'meta_g' class (called by interpret())
**/
size_t	Parser::dirServer(vec_str::iterator it, vec_str::iterator vend) {

	if (_in_server == true)
		throw EmbeddedServersException();

	_serverNb++;
	_in_server = true;

	// need _serverNb > 1 because the "server {}" directive comes up first (when _serverNb == 0)
	// so we need to end the parsing of the first server before sending the first server to cgi.
	if (_serverNb > 1) {
		Server& server = dynamic_cast<Server&>(*this);
		// testing
		server.print_serv();
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
	dir.push_back(std::make_pair("root", &Parser::dirRoot));
	dir.push_back(std::make_pair("client_max_body_size", &Parser::dirMaxBodySize));
	dir.push_back(std::make_pair("autoindex", &Parser::dirAutoIndex));
	dir.push_back(std::make_pair("server_name", &Parser::dirServerName));
	dir.push_back(std::make_pair("error_page", &Parser::dirErrorPage));
	dir.push_back(std::make_pair("{", &Parser::dirOpen));
	dir.push_back(std::make_pair("}", &Parser::dirClose));

	vec_str::iterator itok = tok.begin();
	std::vector<std::pair<std::string, methodPointer> >::iterator idir;
	while (itok != tok.end()) {

		//TESTING ******************
//		std::cout << "ITOK ORIGIN = " << *itok << " and _in_server = " << (_in_server == true ? "true" : "false") << " and in location = " << (_in_location == true ? "true" : "false") << std::endl;
		if (*itok != "server" && _in_server == false)
			throw OutsideServerException();

		idir = dir.begin();
		for (; idir < dir.end(); idir++) {
			if (*itok == idir->first) {
				mp = idir->second;
				itok += (this->*mp)(itok + 1, tok.end());
				break ;
			}
		}
		if (idir == dir.end()) {
			std::cout << "IDIR = " << *itok << std::endl;
			throw NoSuchDirectiveException();
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

/**
 * check_file_extension(): self-explanatory
**/
bool	check_file_extension(char *file_name) {
	std::string file(file_name);
	if (file.find(".") == std::string::npos)
		return 1;

	std::string ext;
	ext = file.substr(file.find(".") + 1, std::string::npos);

	vec_str	good_ext;
	good_ext.push_back("conf");
	good_ext.push_back("php");

	vec_str::iterator it = good_ext.begin();
	for (; it != good_ext.end(); it++) {
		if (*it == ext)
			return 0;
	}
	return 1;
}

/**
 * main(): gets a config file in param and parses it
 **/
int		main(int ac, char **av) {

	Parser	parser;

	if (ac != 2)
		std::cout << RED << "Error: " << NC << "Need one and only one argument\n";
	else if (check_file_extension(av[1]))
		std::cout << RED << "Error: " << NC << "Unauthorized file extension\n";
	else {
		try {
			parser.tokenizer(av);
		}
		catch (std::exception &e) {
			std::cout << RED << "Error: " << NC << e.what() << std::endl;
		}
	}
}
