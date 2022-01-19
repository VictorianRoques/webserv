/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 18:09:23 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/19 18:16:04 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

/**************************************/
//           EXCEPTIONS               //
/**************************************/

char const *Location::WrongMethodException::what() const throw() { return ("Wrong method."); }
char const *Location::WrongValue_AutoIndexException::what() const throw() { return ("Wrong value for autoindex."); }
char const *Location::NonValidRootException::what() const throw() { return ("Non valid root\nUsage: root <path>; (you probably forgot a \";\")."); }
char const *Location::NonValidIndexException::what() const throw() { return ("Non valid index\nUsage: index <file_name>; (you probably forgot a \";\")."); }
char const *Location::NonValidCgiHandlerException::what() const throw() {
	return ("Non valid CGI Handler\nUsage: cgi_handler <file_extension> <CGI binary>; (you probably forgot a \";\").");
}
char const *Location::NonValidRedirectionException::what() const throw() {
	return ("Non valid redirection (status code should belong to [300;308]\nUsage: return <status> <URI>;).");
}

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Location::Location():
	_root("/"),
	_autoIndex(true)
{}

Location::~Location() {}

Location::Location(const Location &x) { *this = x; }

Location&	Location::operator=(const Location &x) {
	if (this != &x) {
		_root = x.getRoot();
		_methods = x.getMethods();
		_errorPage = x.getErrorPage();
		_redirection = x.getRedirection();
		_autoIndex = x.getAutoIndex();
		_index = x.getIndex();
		_cgiHandler = x.getCgiHandler();
		_uploadDest = x.getUploadDest();
	}
	return *this;
}

/**************************************/
//				GETTERS				  //
/**************************************/

std::string							Location::getMatchModifier() const { return _matchModifier; }
std::string							Location::getLocationMatch() const { return _locationMatch; }
std::string							Location::getRoot() const { return _root; }
vec_str								Location::getMethods() const { return _methods; }
vec_str								Location::getErrorPage() const { return _errorPage; }
std::pair<size_t, std::string>		Location::getRedirection() const { return _redirection; }
std::string							Location::getIndex() const { return _index; }
bool								Location::getAutoIndex() const { return _autoIndex; }
std::pair<std::string, std::string>	Location::getCgiHandler() const { return _cgiHandler; }
std::string							Location::getUploadDest() const { return _uploadDest; }

/**************************************/
//				GETTERS				  //
/**************************************/

void	Location::setMatchModifier(std::string matchModifier) { _matchModifier = matchModifier; }
void	Location::setLocationMatch(std::string locationMatch) { _locationMatch = locationMatch; }
void	Location::setRoot(std::string root) { _root = root; }
void	Location::setMethods(vec_str methods) { _methods = methods; }
void	Location::setErrorPage(vec_str errorPage) { _errorPage = errorPage; }
void	Location::setRedirection(std::pair<size_t, std::string> redirection) { _redirection = redirection; }
void	Location::setAutoIndex(bool autoIndex) { _autoIndex = autoIndex; }
void	Location::setIndex(std::string index) { _index = index; }
void	Location::setCgiHandler(std::pair<std::string, std::string> cgiHandler) { _cgiHandler = cgiHandler; }
void	Location::setUploadDest(std::string uploadDest) { _uploadDest = uploadDest; }

/**************************************/
//			PARSING HELPERS			  //
/**************************************/

/**
 * dirRoot(): sets root (called by dirLocation())
**/
size_t	Location::dirRoot(vec_str::iterator it, vec_str::iterator vend) {
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
 * dirUploadDest(): sets uploadDest (called by dirLocation())
**/
size_t	Location::dirUploadDest(vec_str::iterator it, vec_str::iterator vend) {
	std::string	uploadDest;
	size_t		pos;
	size_t		posend;

	//remove the trailing ';'
	pos = (*it).find_first_not_of(";");
	posend = std::min((*it).find_first_of(";", pos), (*it).length());
	uploadDest = (*it).substr(pos, posend - pos);

	setUploadDest(uploadDest);
	(void)vend;
	return 2;
}

/**
 * dirAutoIndex(): sets autoindex (called by dirLocation())
**/
size_t	Location::dirAutoIndex(vec_str::iterator it, vec_str::iterator vend) {
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
 * dirIndex(): sets index from parsing (called by dirLocation())
**/
size_t	Location::dirIndex(vec_str::iterator it, vec_str::iterator vend) {
	std::string	index;
	size_t		pos;
	size_t		posend;

	if ((*it).find(";") == std::string::npos && *(it + 1) != ";")
		throw NonValidIndexException();
	//remove the trailing ';'
	pos = (*it).find_first_not_of(";");
	posend = std::min((*it).find_first_of(";", pos), (*it).length());
	index = (*it).substr(pos, posend - pos);

	setIndex(index);
	(void)vend;
	return 2;
}

/**
 * dirRedirection(): sets redirection (called by dirLocation())
**/
size_t	Location::dirRedirection(vec_str::iterator it, vec_str::iterator vend) {
	size_t code = static_cast<size_t>(std::atoi((*it).c_str()));
	std::string	uri = *(it + 1);
	size_t		pos = uri.find(";");

	if (code < 300 || code > 308)
		throw NonValidRedirectionException();
	else if (it->find(";") != std::string::npos || ((*(it + 1)).find(";") == std::string::npos && *(it + 2) != ";"))
		throw NonValidRedirectionException();
	_redirection.first = code;
	_redirection.second = uri.substr(0, pos);
	(void)vend;
	return 3;
}

/**
 * dirCgiHandler(): sets cgiHandler (called by dirLocation()) (to execute CGI based on certain file extension)
**/
size_t	Location::dirCgiHandler(vec_str::iterator it, vec_str::iterator vend) {
	if (it->find(";") != std::string::npos || ((it + 1)->find(";") == std::string::npos && *(it + 2) != ";"))
		throw NonValidCgiHandlerException();
	_cgiHandler.first = *it;
	size_t	pos = (*(it + 1)).find(";");
	_cgiHandler.second = (*(it + 1)).substr(0, pos);
	(void)vend;
	return 3;
}

/**
 * dirErrorPage(): sets error_page (called by dirLocation())
**/
size_t	Location::dirErrorPage(vec_str::iterator it, vec_str::iterator vend) {
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
 * dirMethods(): sets allowed methods (called by dirLocation())
**/
size_t	Location::dirMethods(vec_str::iterator it, vec_str::iterator vend) {

	vec_str methods;
	bool	no_match;
	size_t	ret = 1;

	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");

	for (; it != vend; it++) {
		ret++;
		std::string tmp = it->substr(0, it->find(";"));
		no_match = true;
		for (vec_str::iterator mit = methods.begin(); mit != methods.end(); mit++) {
			if (*mit == tmp) {
				_methods.push_back(tmp);
				no_match = false;
			}
		}
		if (no_match == true)
			throw WrongMethodException();
		if (it->find(";") != std::string::npos) {
			break ;
		}
	}
	return ret;
}

/**************************************/
//			PRINT_LOC				  //
/**************************************/

void	Location::print_loc() {
	std::cout << std::endl;
	std::cout << COLOR_LOC << "Match Modifier: " << NC << _matchModifier << std::endl;
	std::cout << COLOR_LOC << "Location Match: " << NC << _locationMatch << std::endl;
	std::cout << COLOR_LOC << "Root: " << NC << _root << std::endl;
	std::cout << COLOR_LOC << "Error Page: " << NC; ft_putvec(_errorPage, " ");
	std::cout << COLOR_LOC << "AutoIndex: " << NC << (_autoIndex ? "on" : "off") << std::endl;
	std::cout << COLOR_LOC << "Index (response file if request is directory): " << NC << _index << std::endl;
	std::cout << COLOR_LOC << "Method(s): " << NC; ft_putvec(_methods, " ");
	std::cout << COLOR_LOC << "Redirection: " << NC << (_redirection.first ? "Yes" : "No") << std::endl;
   	if (_redirection.first) {
		std::cout << COLOR_LOC <<  "   Status code: " << NC << _redirection.first << std::endl 
				<< COLOR_LOC << "   URI: " << NC << _redirection.second << std::endl;
	}
	std::cout << COLOR_LOC << "CGI handler: " << NC << std::endl;
	std::cout << COLOR_LOC <<  "   File extension: " << NC << _cgiHandler.first << std::endl; 
	std::cout << COLOR_LOC << "   CGI: " << NC << _cgiHandler.second << std::endl;
	std::cout << COLOR_LOC << "Upload destination directory: " << NC << _uploadDest << std::endl;
	std::cout << std::endl;
}
