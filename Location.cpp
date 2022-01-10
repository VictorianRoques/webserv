/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 18:09:23 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/10 17:53:45 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include "Location.hpp"

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
	}
	return *this;
}

/**************************************/
//				GETTERS				  //
/**************************************/

std::string	Location::getMatchModifier() const { return _matchModifier; }
std::string	Location::getLocationMatch() const { return _locationMatch; }
std::string	Location::getRoot() const { return _root; }

/**************************************/
//				GETTERS				  //
/**************************************/

void	Location::setMatchModifier(std::string matchModifier) { _matchModifier = matchModifier; }
void	Location::setLocationMatch(std::string locationMatch) { _locationMatch = locationMatch; }
void	Location::setRoot(std::string root) { _root = root; }

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

/**************************************/
//			PRINT_LOC				  //
/**************************************/

void	Location::print_loc() {

	std::cout << std::endl;
	std::cout << "Match Modifier: " << _matchModifier << std::endl;
	std::cout << "Location Match: " << _locationMatch << std::endl;
	std::cout << "Root: " << _root << std::endl;
	std::cout << std::endl;
}
