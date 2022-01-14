/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 18:09:23 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/14 11:40:24 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include "Location.hpp"

/**************************************/
//           EXCEPTIONS               //
/**************************************/

char const *Location::WrongMethodException::what() const throw() { return ("Wrong method."); }

/**************************************/
//           COPLIAN CLASS            //
/**************************************/

Location::Location():
	_root("/")
{}

Location::~Location() {}

Location::Location(const Location &x) { *this = x; }

Location&	Location::operator=(const Location &x) {
	if (this != &x) {
		_root = x.getRoot();
		_methods = x.getMethods();
	}
	return *this;
}

/**************************************/
//				GETTERS				  //
/**************************************/

std::string	Location::getMatchModifier() const { return _matchModifier; }
std::string	Location::getLocationMatch() const { return _locationMatch; }
std::string	Location::getRoot() const { return _root; }
vec_str		Location::getMethods() const { return _methods; }

/**************************************/
//				GETTERS				  //
/**************************************/

void	Location::setMatchModifier(std::string matchModifier) { _matchModifier = matchModifier; }
void	Location::setLocationMatch(std::string locationMatch) { _locationMatch = locationMatch; }
void	Location::setRoot(std::string root) { _root = root; }
void	Location::setMethods(vec_str methods) { _methods = methods; }

/**************************************/
//			PARSING HELPERS			  //
/**************************************/

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
	std::cout << COLOR_LOC << "Method(s): " << NC; ft_putvec(_methods, " ");
	std::cout << std::endl;
}
