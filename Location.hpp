/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnielly <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/05 18:01:55 by pnielly           #+#    #+#             */
/*   Updated: 2022/01/13 17:13:59 by pnielly          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "utils.hpp"

class Location {

		/**
		 *  method pointer typedef: redirects to parsing helper functions (see below)
		 *  return type: size_t (number of elements iterated over in the vector... 
		 *  ...while calling the helper function)
		 *  params:
		 *  -it = current position in the vector
		 *  -vend = vector.end()
		**/
	public:
		typedef size_t	(Location::*methodPointer)(vec_str::iterator it, vec_str::iterator vend);

	// Coplian Class
	public:
		Location();
		~Location();
		Location(const Location &);
		Location&	operator=(const Location &);

	// directives
	private:
		std::string	_matchModifier;
		std::string	_locationMatch;
		std::string	_root;
		vec_str		_methods;

		// getters
	public:
		std::string getMatchModifier() const;
		std::string getLocationMatch() const;
		std::string getRoot() const;
		vec_str		getMethods() const;

		//setters
	public:
		void	setMatchModifier(std::string _matchModifier);
		void	setLocationMatch(std::string _locationMatch);
		void	setRoot(std::string root);
		void	setMethods(vec_str _methods);

		// parsing helpers
	public:
		size_t	dirRoot(vec_str::iterator it, vec_str::iterator vend);
		size_t	dirMethods(vec_str::iterator it, vec_str::iterator vend);

		// utils
	public:
		void	print_loc();

		// EXCEPTION
		class WrongMethodException: public std::exception {
			public:
				virtual char const *what() const throw();
		};
};

#endif
